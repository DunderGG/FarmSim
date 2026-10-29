// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmerCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "FarmingComponent.h"
#include "FarmRestComponent.h"
#include "FarmSaveSubsystem.h"
#include "FarmTargetHighlightComponent.h"
#include "FarmToolData.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

#define LOCTEXT_NAMESPACE "FarmerCharacter"

namespace FarmerInput
{
	/** Above the template's movement contexts (priority 0), so farming keys win if a key is ever mapped twice. */
	constexpr int32 MappingContextPriority = 1;
}

namespace FarmerMesh
{
	/** The right hand bone on the UE5 mannequin skeleton. */
	const FName HandSocket(TEXT("hand_r"));
}

AFarmerCharacter::AFarmerCharacter()
{
	Farming = CreateDefaultSubobject<UFarmingComponent>(TEXT("Farming"));
	Rest = CreateDefaultSubobject<UFarmRestComponent>(TEXT("Rest"));
	TargetHighlight = CreateDefaultSubobject<UFarmTargetHighlightComponent>(TEXT("TargetHighlight"));
	TargetHighlight->SetupAttachment(GetRootComponent());

	HeldTool = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldTool"));
	HeldTool->SetupAttachment(GetMesh(), FarmerMesh::HandSocket);
	HeldTool->SetMobility(EComponentMobility::Movable);
	HeldTool->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeldTool->SetGenerateOverlapEvents(false);
}

void AFarmerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CreateInput();
}

void AFarmerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Follow the component's tool so the hand also updates when a save is loaded.
	Farming->OnToolChanged.AddUObject(this, &AFarmerCharacter::ShowHeldTool);
	ShowHeldTool(Farming->GetCurrentTool());
}

void AFarmerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Farming->OnToolChanged.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void AFarmerCharacter::CreateInput()
{
	FarmingMappingContext = NewObject<UInputMappingContext>(this, NAME_None, RF_Transient);

	UseToolAction = AddInputAction(EKeys::E, LOCTEXT("UseTool", "Use tool"));
	SleepAction = AddInputAction(EKeys::N, LOCTEXT("Sleep", "Sleep"));
	NewGameAction = AddInputAction(EKeys::F9, LOCTEXT("NewGame", "New game"));

	// The number keys pick toolbar slots. Their hints name the tool in the slot, so the actions have no description.
	const FKey ToolbarKeys[] = {
		EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine};
	for (const FKey& Key : ToolbarKeys)
	{
		ToolbarSlotActions.Add(AddInputAction(Key, FText::GetEmpty()));
	}
}

UInputAction* AFarmerCharacter::AddInputAction(const FKey& Key, const FText& Description)
{
	UInputAction* Action = NewObject<UInputAction>(this, NAME_None, RF_Transient);
	Action->ActionDescription = Description;
	FarmingMappingContext->MapKey(Action, Key);
	return Action;
}

void AFarmerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		return;
	}

	EnhancedInput->BindAction(UseToolAction, ETriggerEvent::Started, Farming.Get(), &UFarmingComponent::UseTool);
	EnhancedInput->BindAction(SleepAction, ETriggerEvent::Started, Rest.Get(), &UFarmRestComponent::Sleep);
	EnhancedInput->BindAction(NewGameAction, ETriggerEvent::Started, this, &AFarmerCharacter::StartNewGame);
	for (int32 Slot = 0; Slot < ToolbarSlotActions.Num(); ++Slot)
	{
		EnhancedInput->BindAction(
			ToolbarSlotActions[Slot], ETriggerEvent::Started, Farming.Get(), &UFarmingComponent::SelectToolbarSlot, Slot);
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem())
	{
		Subsystem->AddMappingContext(FarmingMappingContext, FarmerInput::MappingContextPriority);
	}
}

void AFarmerCharacter::UnPossessed()
{
	// Removed while the old controller is still known, so a later possession doesn't stack a second copy.
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem())
	{
		Subsystem->RemoveMappingContext(FarmingMappingContext);
	}
	Super::UnPossessed();
}

UEnhancedInputLocalPlayerSubsystem* AFarmerCharacter::GetInputSubsystem() const
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	return PlayerController ? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()) : nullptr;
}

TArray<FFarmKeyHint> AFarmerCharacter::GetToolbarKeyHints() const
{
	TArray<FFarmKeyHint> Hints;
	const TArray<TObjectPtr<const UFarmToolData>>& Tools = Farming->GetToolbar().GetTools();
	for (int32 Slot = 0; Slot < FMath::Min(Tools.Num(), ToolbarSlotActions.Num()); ++Slot)
	{
		if (const UFarmToolData* Tool = Tools[Slot])
		{
			Hints.Add({GetMappedKey(ToolbarSlotActions[Slot]).GetDisplayName(false), Tool->GetDisplayName()});
		}
	}
	return Hints;
}

TArray<FFarmKeyHint> AFarmerCharacter::GetActionKeyHints() const
{
	TArray<FFarmKeyHint> Hints;
	if (!FarmingMappingContext)
	{
		return Hints;
	}
	for (const FEnhancedActionKeyMapping& Mapping : FarmingMappingContext->GetMappings())
	{
		if (Mapping.Action && !Mapping.Action->ActionDescription.IsEmpty())
		{
			Hints.Add({Mapping.Key.GetDisplayName(false), Mapping.Action->ActionDescription});
		}
	}
	return Hints;
}

FKey AFarmerCharacter::GetMappedKey(const UInputAction* Action) const
{
	if (FarmingMappingContext)
	{
		for (const FEnhancedActionKeyMapping& Mapping : FarmingMappingContext->GetMappings())
		{
			if (Mapping.Action == Action)
			{
				return Mapping.Key;
			}
		}
	}
	return EKeys::Invalid;
}

void AFarmerCharacter::StartNewGame()
{
	if (UFarmSaveSubsystem* Saves = GetWorld()->GetSubsystem<UFarmSaveSubsystem>())
	{
		Saves->StartNewGame();
	}
}

void AFarmerCharacter::ShowHeldTool(const UFarmToolData* Tool)
{
	HeldTool->SetStaticMesh(Tool ? Tool->GetHeldMesh() : nullptr);
}

#undef LOCTEXT_NAMESPACE
