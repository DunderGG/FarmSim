// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmHUD.h"

#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "FarmDayCycle.h"
#include "FarmFeedbackSubsystem.h"
#include "FarmHudWidget.h"
#include "FarmingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

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
	if (UFarmFeedbackSubsystem* Feedback = GetWorld()->GetSubsystem<UFarmFeedbackSubsystem>())
	{
		Feedback->OnFeedback.AddUObject(this, &AFarmHUD::ShowMessage);
	}

	HudWidget = SNew(SFarmHudWidget).Hud(this);
	Viewport->AddViewportWidgetContent(HudWidget.ToSharedRef());
}

void AFarmHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerOwner)
	{
		PlayerOwner->OnPossessedPawnChanged.RemoveDynamic(this, &AFarmHUD::FollowPawn);
	}
	if (UFarmFeedbackSubsystem* Feedback = UWorld::GetSubsystem<UFarmFeedbackSubsystem>(GetWorld()))
	{
		Feedback->OnFeedback.RemoveAll(this);
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

void AFarmHUD::FollowPawn(APawn* OldPawn, APawn* NewPawn)
{
	Farming = NewPawn ? NewPawn->FindComponentByClass<UFarmingComponent>() : nullptr;
}

void AFarmHUD::ShowMessage(const FText& Message)
{
	if (Messages.Num() >= MaxMessages)
	{
		Messages.RemoveAt(0);
	}
	Messages.Add({Message, GetWorld()->GetTimeSeconds() + MessageDuration});
	RebuildMessagesText();

	// Every message lasts as long, so the oldest fades first; one timer for it is enough.
	if (!GetWorldTimerManager().IsTimerActive(FadeTimer))
	{
		GetWorldTimerManager().SetTimer(FadeTimer, this, &AFarmHUD::RemoveFadedMessages, MessageDuration);
	}
}

void AFarmHUD::RemoveFadedMessages()
{
	const double Now = GetWorld()->GetTimeSeconds();
	const int32 FadedCount = Messages.IndexOfByPredicate(
		[Now](const FMessage& Message)
		{
			return Message.EndTime > Now;
		});
	Messages.RemoveAt(0, FadedCount == INDEX_NONE ? Messages.Num() : FadedCount);
	RebuildMessagesText();

	if (!Messages.IsEmpty())
	{
		GetWorldTimerManager().SetTimer(FadeTimer, this, &AFarmHUD::RemoveFadedMessages, Messages[0].EndTime - Now);
	}
}

void AFarmHUD::RebuildMessagesText()
{
	TArray<FText> Lines;
	Lines.Reserve(Messages.Num());
	for (const FMessage& Message : Messages)
	{
		Lines.Add(Message.Text);
	}
	MessagesText = FText::Join(FText::AsCultureInvariant(TEXT("\n")), Lines);
}
