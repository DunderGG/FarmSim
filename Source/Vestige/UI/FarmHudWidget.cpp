// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "UI/FarmHudWidget.h"

#include "Styling/CoreStyle.h"
#include "UI/FarmHUD.h"
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
	constexpr float MessageSpacing = 6.f;
	const FMargin PanelPadding(16.f, 10.f);
	const FMargin MessagePadding(16.f, 8.f);
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

		// Feedback messages, each in its own box, above the controls hint. AddMessageBoxes fills the list.
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(0.f, 0.f, 0.f, FarmHudStyle::MessageBottomMargin))
		[
			SAssignNew(MessageList, SVerticalBox)
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

	if (const AFarmHUD* FarmHud = Hud.Get())
	{
		AddMessageBoxes(FarmHud->GetMaxMessages());
	}
}

FText SFarmHudWidget::GetStatusText() const
{
	const AFarmHUD* FarmHud = Hud.Get();
	return FarmHud ? FarmHud->GetStatusText() : FText::GetEmpty();
}

FText SFarmHudWidget::GetInventoryText() const
{
	const AFarmHUD* FarmHud = Hud.Get();
	return FarmHud ? FarmHud->GetInventoryText() : FText::GetEmpty();
}

FText SFarmHudWidget::GetControlsText() const
{
	const AFarmHUD* FarmHud = Hud.Get();
	return FarmHud ? FarmHud->GetControlsText() : FText::GetEmpty();
}

void SFarmHudWidget::AddMessageBoxes(int32 Count)
{
	const FSlateBrush* Backdrop = FCoreStyle::Get().GetBrush(FarmHudStyle::BackdropBrush);
	for (int32 Index = 0; Index < Count; ++Index)
	{
		TSharedPtr<SBorder> MessageBox;
		TSharedPtr<STextBlock> MessageText;

		// clang-format off
		MessageList->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(0.f, Index == 0 ? 0.f : FarmHudStyle::MessageSpacing, 0.f, 0.f)
		[
			SAssignNew(MessageBox, SBorder)
			.Visibility(EVisibility::Collapsed)
			.BorderImage(Backdrop)
			.BorderBackgroundColor(this, &SFarmHudWidget::GetMessageBackdropColor, Index)
			.ColorAndOpacity(this, &SFarmHudWidget::GetMessageContentTint, Index)
			.Padding(FarmHudStyle::MessagePadding)
			[
				SAssignNew(MessageText, STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle(FarmHudStyle::BoldFont, FarmHudStyle::MessageFontSize))
				.ColorAndOpacity(FarmHudStyle::Message)
				.ShadowOffset(FarmHudStyle::ShadowOffset)
			]
		];
		// clang-format on

		MessageBoxes.Add(MessageBox);
		MessageTexts.Add(MessageText);
	}
}

void SFarmHudWidget::RefreshMessages()
{
	const AFarmHUD* FarmHud = Hud.Get();
	const TArray<FFarmHudMessage> NoMessages;
	const TArray<FFarmHudMessage>& Shown = FarmHud ? FarmHud->GetMessages().GetMessages() : NoMessages;
	for (int32 Index = 0; Index < MessageBoxes.Num(); ++Index)
	{
		const bool bShown = Shown.IsValidIndex(Index);
		MessageTexts[Index]->SetText(bShown ? Shown[Index].Text : FText::GetEmpty());
		MessageBoxes[Index]->SetVisibility(bShown ? EVisibility::HitTestInvisible : EVisibility::Collapsed);
	}
}

FSlateColor SFarmHudWidget::GetMessageBackdropColor(int32 Index) const
{
	const AFarmHUD* FarmHud = Hud.Get();
	FLinearColor Color = FarmHudStyle::Backdrop;
	Color.A *= FarmHud ? FarmHud->GetMessageOpacity(Index) : 0.f;
	return Color;
}

FLinearColor SFarmHudWidget::GetMessageContentTint(int32 Index) const
{
	const AFarmHUD* FarmHud = Hud.Get();
	return FLinearColor(1.f, 1.f, 1.f, FarmHud ? FarmHud->GetMessageOpacity(Index) : 0.f);
}
