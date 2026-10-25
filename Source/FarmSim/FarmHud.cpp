#include "FarmHud.h"

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
	/** Formats hours since midnight as a 12-hour clock in 10 minute steps, like "6:40 pm". */
	FString FormatClock(float Hours)
	{
		const int32 TotalMinutes = FMath::FloorToInt(Hours * 6.f) * 10;
		const int32 Hour24 = (TotalMinutes / 60) % 24;
		const int32 Hour12 = Hour24 % 12 == 0 ? 12 : Hour24 % 12;
		return FString::Printf(TEXT("%d:%02d %s"), Hour12, TotalMinutes % 60, Hour24 < 12 ? TEXT("am") : TEXT("pm"));
	}
}

void SFarmHud::Construct(const FArguments& InArgs)
{
	Farming = InArgs._Farming;

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
					.Text(this, &SFarmHud::GetStatusText)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 4.f, 0.f, 0.f)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
					.ColorAndOpacity(FarmHudStyle::Text)
					.ShadowOffset(FarmHudStyle::ShadowOffset)
					.Text(this, &SFarmHud::GetInventoryText)
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
			.Visibility(this, &SFarmHud::GetMessageVisibility)
			.BorderImage(Backdrop)
			.BorderBackgroundColor(FarmHudStyle::Backdrop)
			.Padding(FMargin(16.f, 8.f))
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
				.ColorAndOpacity(FarmHudStyle::Message)
				.ShadowOffset(FarmHudStyle::ShadowOffset)
				.Text(this, &SFarmHud::GetMessageText)
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
			.Text(FText::FromString(TEXT("1 Hoe   2 Seeds   3 Watering Can   4 Hand   |   E Use tool   |   N Sleep")))
		]
	];
}

FText SFarmHud::GetStatusText() const
{
	const UFarmingComponent* Component = Farming.Get();
	if (!Component)
	{
		return FText::GetEmpty();
	}
	return FText::FromString(FString::Printf(TEXT("Day %d   %s   |   %s"),
		Component->GetCurrentDay(), *FormatClock(Component->GetTimeOfDay()),
		*UEnum::GetDisplayValueAsText(Component->GetCurrentTool()).ToString()));
}

FText SFarmHud::GetInventoryText() const
{
	const UFarmingComponent* Component = Farming.Get();
	if (!Component)
	{
		return FText::GetEmpty();
	}
	return FText::FromString(FString::Printf(TEXT("Seeds: %d   Harvested: %d"), Component->Seeds, Component->HarvestedCrops));
}

FText SFarmHud::GetMessageText() const
{
	const UFarmingComponent* Component = Farming.Get();
	return Component ? FText::FromString(Component->GetMessage()) : FText::GetEmpty();
}

EVisibility SFarmHud::GetMessageVisibility() const
{
	const UFarmingComponent* Component = Farming.Get();
	return Component && Component->HasMessage() ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
}
