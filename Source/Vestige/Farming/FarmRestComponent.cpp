// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "Farming/FarmRestComponent.h"

#include "Core/FarmFeedbackSubsystem.h"
#include "Engine/World.h"
#include "Time/FarmDayCycle.h"

#define LOCTEXT_NAMESPACE "FarmRestComponent"

UFarmRestComponent::UFarmRestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFarmRestComponent::BeginPlay()
{
	Super::BeginPlay();

	DayCycle = AFarmDayCycle::Get(GetWorld());
	if (DayCycle)
	{
		DayCycle->OnDayEndReached.AddUObject(this, &UFarmRestComponent::PassOut);
	}
}

void UFarmRestComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DayCycle)
	{
		DayCycle->OnDayEndReached.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UFarmRestComponent::Sleep()
{
	StartNextDay(LOCTEXT("Slept", "You slept. Day {Day} begins."));
}

void UFarmRestComponent::PassOut(int32 EndedDay)
{
	StartNextDay(LOCTEXT("PassedOut", "You stayed up too late and passed out. Day {Day} begins."));
}

void UFarmRestComponent::StartNextDay(const FText& MessageFormat)
{
	if (!DayCycle)
	{
		return;
	}

	// Raised first, so messages from anything reacting to the new day, like saving, follow it.
	UFarmFeedbackSubsystem::Notify(GetWorld(), FText::FormatNamed(MessageFormat, TEXT("Day"), DayCycle->GetCurrentDay() + 1));
	DayCycle->StartNextDay();
}

#undef LOCTEXT_NAMESPACE
