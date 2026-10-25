#include "FarmerCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

AFarmerCharacter::AFarmerCharacter()
{
	Farming = CreateDefaultSubobject<UFarmingComponent>(TEXT("Farming"));

	// hand_r is the right hand bone on the UE5 mannequin skeleton.
	HeldTool = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldTool"));
	HeldTool->SetupAttachment(GetMesh(), TEXT("hand_r"));
	HeldTool->SetMobility(EComponentMobility::Movable);
	HeldTool->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFarmerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SelectTool(Farming->GetCurrentTool());
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
	EnhancedInput->BindAction(MakeAction(EKeys::N), ETriggerEvent::Started, Farming.Get(), &UFarmingComponent::Sleep);
	EnhancedInput->BindAction(MakeAction(EKeys::One), ETriggerEvent::Started, this, &AFarmerCharacter::SelectHoe);
	EnhancedInput->BindAction(MakeAction(EKeys::Two), ETriggerEvent::Started, this, &AFarmerCharacter::SelectSeeds);
	EnhancedInput->BindAction(MakeAction(EKeys::Three), ETriggerEvent::Started, this, &AFarmerCharacter::SelectWateringCan);
	EnhancedInput->BindAction(MakeAction(EKeys::Four), ETriggerEvent::Started, this, &AFarmerCharacter::SelectHand);

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FarmingMappingContext, 1);
		}
	}
}

void AFarmerCharacter::SelectHoe() { SelectTool(EFarmTool::Hoe); }
void AFarmerCharacter::SelectSeeds() { SelectTool(EFarmTool::Seeds); }
void AFarmerCharacter::SelectWateringCan() { SelectTool(EFarmTool::WateringCan); }
void AFarmerCharacter::SelectHand() { SelectTool(EFarmTool::Hand); }

void AFarmerCharacter::SelectTool(EFarmTool Tool)
{
	Farming->SelectTool(Tool);

	const TObjectPtr<UStaticMesh>* ToolMesh = ToolMeshes.Find(Tool);
	HeldTool->SetStaticMesh(ToolMesh ? ToolMesh->Get() : nullptr);
}
