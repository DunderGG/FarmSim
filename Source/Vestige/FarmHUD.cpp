// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

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

	DayCycle = AFarmDayCycle::Get(GetWorld());
	PlayerOwner->OnPossessedPawnChanged.AddDynamic(this, &AFarmHUD::FollowPawn);
	FollowPawn(nullptr, PlayerOwner->GetPawn());

	HudWidget = SNew(SFarmHudWidget).Hud(this);
	Viewport->AddViewportWidgetContent(HudWidget.ToSharedRef());
}

void AFarmHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerOwner)
	{
		PlayerOwner->OnPossessedPawnChanged.RemoveDynamic(this, &AFarmHUD::FollowPawn);
	}
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

void AFarmHUD::FollowPawn(APawn* OldPawn, APawn* NewPawn)
{
	Farming = NewPawn ? NewPawn->FindComponentByClass<UFarmingComponent>() : nullptr;
}
