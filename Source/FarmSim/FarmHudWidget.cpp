#include "FarmHudWidget.h"

#include "FarmDayCycle.h"
#include "FarmHUD.h"

#include "FarmingComponent.h"
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
}

namespace
{
	/** Formats hours since midnight as a 24-hour clock in 10 minute steps, like "18:40". */
	FString FormatClock(float Hours)
	{
		const int32 TotalMinutes = FMath::FloorToInt(Hours * 6.f) * 10;
		return FString::Printf(TEXT("%02d:%02d"), (TotalMinutes / 60) % 24, TotalMinutes % 60);
	}
}

void SFarmHudWidget::Construct(const FArguments& InArgs)
{
	Hud = InArgs._Hud;

	const FSlateBrush* Backdrop = FCoreStyle::Get().GetBrush("WhiteBrush");

	ChildSlot
	[
		SNew(SOverlay)
		.Visibility(EVisibility::HitTestInvisible)

		// Status panel, top left.
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(24.f)
		[
			SNew(SBorder)
			.BorderImage(Backdrop)
			.BorderBackgroundColor(FarmHudStyle::Backdrop)
			.Padding(FMargin(16.f, 10.f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
					.ColorAndOpacity(FarmHudStyle::Text)
					.ShadowOffset(FarmHudStyle::ShadowOffset)
					.Text(this, &SFarmHudWidget::GetStatusText)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 4.f, 0.f, 0.f)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
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
		.Padding(FMargin(0.f, 0.f, 0.f, 80.f))
		[
			SNew(SBorder)
			.Visibility(this, &SFarmHudWidget::GetMessageVisibility)
			.BorderImage(Backdrop)
			.BorderBackgroundColor(FarmHudStyle::Backdrop)
			.Padding(FMargin(16.f, 8.f))
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
				.ColorAndOpacity(FarmHudStyle::Message)
				.ShadowOffset(FarmHudStyle::ShadowOffset)
				.Text(this, &SFarmHudWidget::GetMessageText)
			]
		]

		// Controls hint, bottom center.
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(0.f, 0.f, 0.f, 24.f))
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 13))
			.ColorAndOpacity(FarmHudStyle::Hint)
			.ShadowOffset(FarmHudStyle::ShadowOffset)
			.Text(FText::FromString(TEXT("1 Hoe   2 Seeds   3 Watering Can   4 Hand   |   E Use tool   |   N Sleep   |   F9 New game")))
		]
	];
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
	return FText::FromString(FString::Printf(TEXT("Day %d   %s   |   %s"),
		DayCycle->GetCurrentDay(), *FormatClock(DayCycle->GetTimeOfDay()),
		*UEnum::GetDisplayValueAsText(Farming->GetCurrentTool()).ToString()));
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