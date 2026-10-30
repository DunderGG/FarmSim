// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "UI/FarmHUD.h"

#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Time/FarmDayCycle.h"
#include "Player/FarmerCharacter.h"
#include "Core/FarmFeedbackSubsystem.h"
#include "UI/FarmHudWidget.h"
#include "Farming/FarmingComponent.h"
#include "Content/FarmToolData.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "FarmHUD"

namespace FarmHudClock
{
	constexpr int32 MinutesPerHour = 60;
	constexpr int32 HoursPerDay = 24;

	/** The clock moves in steps of this many minutes, so it doesn't flicker every game minute. */
	constexpr int32 StepMinutes = 10;

	/** Hours since midnight, rounded down to the clock's step, in minutes. */
	int32 ToShownMinutes(float Hours)
	{
		return FMath::FloorToInt(Hours * MinutesPerHour / StepMinutes) * StepMinutes;
	}

	/** A 24-hour clock, like "18:40". Minutes past midnight of the next day wrap around to 00:00. */
	FText Format(int32 Minutes)
	{
		FNumberFormattingOptions TwoDigits;
		TwoDigits.SetMinimumIntegralDigits(2).SetUseGrouping(false);
		return FText::FormatNamed(LOCTEXT("Clock", "{Hours}:{Minutes}"), TEXT("Hours"),
			FText::AsNumber((Minutes / MinutesPerHour) % HoursPerDay, &TwoDigits), TEXT("Minutes"),
			FText::AsNumber(Minutes % MinutesPerHour, &TwoDigits));
	}
}

namespace FarmHudHints
{
	FText Format(const FFarmKeyHint& Hint)
	{
		return FText::FormatNamed(LOCTEXT("KeyHint", "{Key} {Action}"), TEXT("Key"), Hint.Key, TEXT("Action"), Hint.Action);
	}
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
	StopFollowingFarming();
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

FText AFarmHUD::GetStatusText() const
{
	const AFarmDayCycle* Cycle = DayCycle.Get();
	if (!Cycle)
	{
		return FText::GetEmpty();
	}

	const int32 Day = Cycle->GetCurrentDay();
	const int32 ClockMinutes = FarmHudClock::ToShownMinutes(Cycle->GetTimeOfDay());
	if (Day != StatusDay || ClockMinutes != StatusClockMinutes)
	{
		StatusDay = Day;
		StatusClockMinutes = ClockMinutes;
		StatusText = FText::FormatNamed(LOCTEXT("Status", "Day {Day}   {Clock}   |   {Tool}"), TEXT("Day"), Day, TEXT("Clock"),
			FarmHudClock::Format(ClockMinutes), TEXT("Tool"), ToolText);
	}
	return StatusText;
}

void AFarmHUD::FollowPawn(APawn* OldPawn, APawn* NewPawn)
{
	StopFollowingFarming();
	Farmer = Cast<AFarmerCharacter>(NewPawn);
	Farming = NewPawn ? NewPawn->FindComponentByClass<UFarmingComponent>() : nullptr;

	UFarmingComponent* NewFarming = Farming.Get();
	if (NewFarming)
	{
		NewFarming->OnToolChanged.AddUObject(this, &AFarmHUD::RefreshToolTexts);
		NewFarming->OnInventoryChanged.AddUObject(this, &AFarmHUD::RefreshInventoryText);
	}
	RefreshToolTexts(NewFarming ? NewFarming->GetCurrentTool() : nullptr);
	RefreshInventoryText();
}

void AFarmHUD::StopFollowingFarming()
{
	if (UFarmingComponent* OldFarming = Farming.Get())
	{
		OldFarming->OnToolChanged.RemoveAll(this);
		OldFarming->OnInventoryChanged.RemoveAll(this);
	}
}

void AFarmHUD::RefreshToolTexts(const UFarmToolData* Tool)
{
	ToolText = Tool ? Tool->GetDisplayName() : LOCTEXT("NoTool", "No tool");

	// The status line shows the tool, so it is built again on its next read.
	StatusDay = INDEX_NONE;
	RefreshControlsText();
}

void AFarmHUD::RefreshInventoryText()
{
	const UFarmingComponent* CurrentFarming = Farming.Get();
	if (!CurrentFarming)
	{
		InventoryText = FText::GetEmpty();
		return;
	}
	InventoryText = FText::FormatNamed(LOCTEXT("Inventory", "Seeds: {Seeds}   Harvested: {Harvested}"), TEXT("Seeds"),
		CurrentFarming->GetSeeds(), TEXT("Harvested"), CurrentFarming->GetHarvestedCrops());
}

void AFarmHUD::RefreshControlsText()
{
	const AFarmerCharacter* CurrentFarmer = Farmer.Get();
	if (!CurrentFarmer)
	{
		ControlsText = FText::GetEmpty();
		return;
	}

	TArray<FText> ToolbarHints;
	for (const FFarmKeyHint& Hint : CurrentFarmer->GetToolbarKeyHints())
	{
		ToolbarHints.Add(FarmHudHints::Format(Hint));
	}

	// The toolbar hints belong together; the other hints are set apart from them and from each other.
	TArray<FText> Groups;
	if (!ToolbarHints.IsEmpty())
	{
		Groups.Add(FText::Join(FText::AsCultureInvariant(TEXT("   ")), ToolbarHints));
	}
	for (const FFarmKeyHint& Hint : CurrentFarmer->GetActionKeyHints())
	{
		Groups.Add(FarmHudHints::Format(Hint));
	}
	ControlsText = FText::Join(FText::AsCultureInvariant(TEXT("   |   ")), Groups);
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

#undef LOCTEXT_NAMESPACE
