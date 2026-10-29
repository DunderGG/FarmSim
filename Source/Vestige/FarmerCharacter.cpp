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
#include "FarmToolData.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

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

	HeldTool = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldTool"));
	HeldTool->SetupAttachment(GetMesh(), FarmerMesh::HandSocket);
	HeldTool->SetMobility(EComponentMobility::Movable);
	HeldTool->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFarmerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Follow the component's tool so the hand also updates when a save is loaded.
	Farming->OnToolChanged.AddUObject(this, &AFarmerCharacter::ShowHeldTool);
	ShowHeldTool(Farming->GetCurrentTool());
}

void AFarmerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		return;
	}

	FarmingMappingContext = NewObject<UInputMappingContext>(this);
	FarmingActions.Reset();

	auto MakeAction = [this](FKey Key) -> UInputAction*
	{
		UInputAction* Action = NewObject<UInputAction>(this);
		FarmingMappingContext->MapKey(Action, Key);
		FarmingActions.Add(Action);
		return Action;
	};

	EnhancedInput->BindAction(MakeAction(EKeys::E), ETriggerEvent::Started, Farming.Get(), &UFarmingComponent::UseTool);
	EnhancedInput->BindAction(MakeAction(EKeys::N), ETriggerEvent::Started, Rest.Get(), &UFarmRestComponent::Sleep);
	EnhancedInput->BindAction(MakeAction(EKeys::F9), ETriggerEvent::Started, this, &AFarmerCharacter::StartNewGame);

	// The number keys pick toolbar slots; which tool sits in each slot is the farming component's data.
	const FKey ToolbarKeys[] = {
		EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine};
	for (int32 Slot = 0; Slot < UE_ARRAY_COUNT(ToolbarKeys); ++Slot)
	{
		EnhancedInput->BindAction(
			MakeAction(ToolbarKeys[Slot]), ETriggerEvent::Started, Farming.Get(), &UFarmingComponent::SelectToolbarSlot, Slot);
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FarmingMappingContext, FarmerInput::MappingContextPriority);
		}
	}
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
