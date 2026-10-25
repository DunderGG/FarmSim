#include "FarmHUD.h"

#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "FarmDayCycle.h"
#include "FarmHudWidget.h"
#include "FarmingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

AFarmHUD* AFarmHUD::GetFor(const APawn* Pawn)
{
	const APlayerController* PlayerController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	return PlayerController ? Cast<AFarmHUD>(PlayerController->GetHUD()) : nullptr;
}

void AFarmHUD::BeginPlay()
{
	Super::BeginPlay();

	UGameViewportClient* Viewport = GetWorld() ? GetWorld()->GetGameViewport() : nullptr;
	if (!PlayerOwner || !PlayerOwner->IsLocalController() || !Viewport)
	{
		return;
	}

	HudWidget = SNew(SFarmHudWidget).Hud(this);
	Viewport->AddViewportWidgetContent(HudWidget.ToSharedRef());
}

void AFarmHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HudWidget)
	{
		if (UGameViewportClient* Viewport = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
		{
			Viewport->RemoveViewportWidgetContent(HudWidget.ToSharedRef());
		}
		HudWidget.Reset();
	}
	Super::EndPlay(EndPlayReason);
}

void AFarmHUD::ShowMessage(const FString& NewMessage)
{
	Message = NewMessage;
	MessageEndTime = GetWorld()->GetTimeSeconds() + MessageDuration;
}

void AFarmHUD::AppendMessage(const FString& Extra)
{
	ShowMessage(HasMessage() ? Message + TEXT(" ") + Extra : Extra);
}

bool AFarmHUD::HasMessage() const
{
	return !Message.IsEmpty() && GetWorld()->GetTimeSeconds() < MessageEndTime;
}

const UFarmingComponent* AFarmHUD::GetFarming() const
{
	const APawn* Pawn = PlayerOwner ? PlayerOwner->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UFarmingComponent>() : nullptr;
}

const AFarmDayCycle* AFarmHUD::GetDayCycle() const
{
	return AFarmDayCycle::Find(GetWorld());
}
