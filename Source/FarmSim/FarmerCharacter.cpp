#include "FarmerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "FarmingComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

AFarmerCharacter::AFarmerCharacter()
{
	Farming = CreateDefaultSubobject<UFarmingComponent>(TEXT("Farming"));
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

void AFarmerCharacter::SelectHoe() { Farming->SelectTool(EFarmTool::Hoe); }
void AFarmerCharacter::SelectSeeds() { Farming->SelectTool(EFarmTool::Seeds); }
void AFarmerCharacter::SelectWateringCan() { Farming->SelectTool(EFarmTool::WateringCan); }
void AFarmerCharacter::SelectHand() { Farming->SelectTool(EFarmTool::Hand); }
