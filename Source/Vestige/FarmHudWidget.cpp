// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmHudWidget.h"

#include "FarmDayCycle.h"
#include "FarmHUD.h"
#include "FarmingComponent.h"
#include "FarmToolData.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

namespace FarmHudStyle
{
	const FLinearColor Backdrop(0.f, 0.f, 0.f, 0.45f);
	const FLinearColor Text(1.f, 1.f, 1.f);
	const FLinearColor Hint(0.8f, 0.8f, 0.8f);
	const FLinearColor Message(1.f, 0.9f, 0.55f);
	const FVector2D ShadowOffset(1.f, 1.f);

	const FName BackdropBrush(TEXT("WhiteBrush"));
	const FName BoldFont(TEXT("Bold"));
	const FName RegularFont(TEXT("Regular"));
	constexpr int32 StatusFontSize = 20;
	constexpr int32 InventoryFontSize = 16;
	constexpr int32 MessageFontSize = 18;
	constexpr int32 HintFontSize = 13;

	// Spacing in Slate units, which scale with the screen's DPI.
	constexpr float ScreenMargin = 24.f;
	constexpr float LineSpacing = 4.f;
	constexpr float MessageBottomMargin = 80.f;
	const FMargin PanelPadding(16.f, 10.f);
	const FMargin MessagePadding(16.f, 8.f);
}

namespace
{
	constexpr int32 MinutesPerHour = 60;
	constexpr int32 HoursPerDay = 24;

	/** The clock moves in steps of this many minutes, so it doesn't flicker every game minute. */
	constexpr int32 ClockStepMinutes = 10;

	/** Formats hours since midnight as a 24-hour clock in ClockStepMinutes steps, like "18:40". */
	FString FormatClock(float Hours)
	{
		const int32 TotalMinutes = FMath::FloorToInt(Hours * MinutesPerHour / ClockStepMinutes) * ClockStepMinutes;
		return FString::Printf(TEXT("%02d:%02d"), (TotalMinutes / MinutesPerHour) % HoursPerDay, TotalMinutes % MinutesPerHour);
	}
}

void SFarmHudWidget::Construct(const FArguments& InArgs)
{
	Hud = InArgs._Hud;

	const FSlateBrush* Backdrop = FCoreStyle::Get().GetBrush(FarmHudStyle::BackdropBrush);

	// clang-format off
	ChildSlot
	[
		SNew(SOverlay)
		.Visibility(EVisibility::HitTestInvisible)

		// Status panel, top left.
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(FarmHudStyle::ScreenMargin)
		[
			SNew(SBorder)
			.BorderImage(Backdrop)
			.BorderBackgroundColor(FarmHudStyle::Backdrop)
			.Padding(FarmHudStyle::PanelPadding)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle(FarmHudStyle::BoldFont, FarmHudStyle::StatusFontSize))
					.ColorAndOpacity(FarmHudStyle::Text)
					.ShadowOffset(FarmHudStyle::ShadowOffset)
					.Text(this, &SFarmHudWidget::GetStatusText)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, FarmHudStyle::LineSpacing, 0.f, 0.f)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle(FarmHudStyle::RegularFont, FarmHudStyle::InventoryFontSize))
					.ColorAndOpacity(FarmHudStyle::Text)
					.ShadowOffset(FarmHudStyle::ShadowOffset)
					.Text(this, &SFarmHudWidget::GetInventoryText)
				]
			]
		]

		// Feedback message, above the controls hint.
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(0.f, 0.f, 0.f, FarmHudStyle::MessageBottomMargin))
		[
			SNew(SBorder)
			.Visibility(this, &SFarmHudWidget::GetMessageVisibility)
			.BorderImage(Backdrop)
			.BorderBackgroundColor(FarmHudStyle::Backdrop)
			.Padding(FarmHudStyle::MessagePadding)
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle(FarmHudStyle::BoldFont, FarmHudStyle::MessageFontSize))
				.ColorAndOpacity(FarmHudStyle::Message)
				.ShadowOffset(FarmHudStyle::ShadowOffset)
				.Text(this, &SFarmHudWidget::GetMessageText)
			]
		]

		// Controls hint, bottom center.
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(0.f, 0.f, 0.f, FarmHudStyle::ScreenMargin))
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle(FarmHudStyle::RegularFont, FarmHudStyle::HintFontSize))
			.ColorAndOpacity(FarmHudStyle::Hint)
			.ShadowOffset(FarmHudStyle::ShadowOffset)
			.Text(this, &SFarmHudWidget::GetControlsText)
		]
	];
	// clang-format on
}

FText SFarmHudWidget::GetStatusText() const
{
	const AFarmHUD* FarmHud = Hud.Get();
	const AFarmDayCycle* DayCycle = FarmHud ? FarmHud->GetDayCycle() : nullptr;
	const UFarmingComponent* Farming = FarmHud ? FarmHud->GetFarming() : nullptr;
	if (!DayCycle || !Farming)
	{
		return FText::GetEmpty();
	}
	const UFarmToolData* Tool = Farming->GetCurrentTool();
	return FText::FromString(FString::Printf(TEXT("Day %d   %s   |   %s"), DayCycle->GetCurrentDay(),
		*FormatClock(DayCycle->GetTimeOfDay()), Tool ? *Tool->GetDisplayName().ToString() : TEXT("No tool")));
}

FText SFarmHudWidget::GetControlsText() const
{
	FString Controls;
	const AFarmHUD* FarmHud = Hud.Get();
	if (const UFarmingComponent* Farming = FarmHud ? FarmHud->GetFarming() : nullptr)
	{
		const TArray<TObjectPtr<const UFarmToolData>>& Tools = Farming->GetToolbar().GetTools();
		for (int32 Slot = 0; Slot < Tools.Num(); ++Slot)
		{
			if (const UFarmToolData* Tool = Tools[Slot])
			{
				Controls += FString::Printf(TEXT("%d %s   "), Slot + 1, *Tool->GetDisplayName().ToString());
			}
		}
		if (!Controls.IsEmpty())
		{
			Controls += TEXT("|   ");
		}
	}
	Controls += TEXT("E Use tool   |   N Sleep   |   F9 New game");
	return FText::FromString(Controls);
}

FText SFarmHudWidget::GetInventoryText() const
{
	const AFarmHUD* FarmHud = Hud.Get();
	const UFarmingComponent* Farming = FarmHud ? FarmHud->GetFarming() : nullptr;
	if (!Farming)
	{
		return FText::GetEmpty();
	}
	return FText::FromString(FString::Printf(TEXT("Seeds: %d   Harvested: %d"), Farming->Seeds, Farming->HarvestedCrops));
}

FText SFarmHudWidget::GetMessageText() const
{
	const AFarmHUD* FarmHud = Hud.Get();
	return FarmHud ? FText::FromString(FarmHud->GetMessage()) : FText::GetEmpty();
}

EVisibility SFarmHudWidget::GetMessageVisibility() const
{
	const AFarmHUD* FarmHud = Hud.Get();
	return FarmHud && FarmHud->HasMessage() ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
}
