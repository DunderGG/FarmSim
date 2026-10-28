// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmDayCycle.h"

#include "Components/LightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AFarmDayCycle::AFarmDayCycle()
{
	PrimaryActorTick.bCanEverTick = true;

	NightPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("NightPostProcess"));
	RootComponent = NightPostProcess;
	NightPostProcess->bUnbound = true;
	NightPostProcess->Settings.bOverride_AutoExposureBias = true;
	NightPostProcess->Settings.AutoExposureBias = 0.f;
}

AFarmDayCycle* AFarmDayCycle::Get(UWorld* World)
{
	if (AFarmDayCycle* Existing = Find(World))
	{
		return Existing;
	}
	return World ? World->SpawnActor<AFarmDayCycle>() : nullptr;
}

AFarmDayCycle* AFarmDayCycle::Find(const UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}

	// The prototype has a single day cycle, so just use the first one in the level.
	TActorIterator<AFarmDayCycle> It(World);
	return It ? *It : nullptr;
}

void AFarmDayCycle::BeginPlay()
{
	Super::BeginPlay();

	TimeOfDay = DayStartHour;
	FindLights();
	UpdateLighting();
}

void AFarmDayCycle::StartNextDay()
{
	++CurrentDay;
	TimeOfDay = DayStartHour;
	UpdateLighting();
	OnNewDay.Broadcast(CurrentDay);
	OnNewDayFinished.Broadcast(CurrentDay);
}

void AFarmDayCycle::RestoreClock(int32 SavedDay, float SavedTimeOfDay)
{
	CurrentDay = FMath::Max(1, SavedDay);
	TimeOfDay = FMath::Clamp(SavedTimeOfDay, DayStartHour, DayEndHour);
	UpdateLighting();
}

void AFarmDayCycle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// The clock stops at DayEndHour until the player sleeps or passes out.
	TimeOfDay = FMath::Min(TimeOfDay + DeltaSeconds / RealSecondsPerGameHour, DayEndHour);
	UpdateLighting();
}

void AFarmDayCycle::FindLights()
{
	if (!Sun)
	{
		TActorIterator<ADirectionalLight> It(GetWorld());
		Sun = It ? *It : nullptr;
	}
	if (!SkyLight)
	{
		TActorIterator<ASkyLight> It(GetWorld());
		SkyLight = It ? *It : nullptr;
	}

	if (Sun)
	{
		// Static or stationary lights can't be turned at runtime. Setting the light to Movable in the level avoids this.
		ULightComponent* SunLight = Sun->GetLightComponent();
		if (SunLight->Mobility != EComponentMobility::Movable)
		{
			SunLight->SetMobility(EComponentMobility::Movable);
		}
		SunYaw = Sun->GetActorRotation().Yaw;
		DaySunIntensity = SunLight->Intensity;
		DaySunColor = SunLight->GetLightColor();
	}
	if (SkyLight)
	{
		DaySkyIntensity = SkyLight->GetLightComponent()->Intensity;
	}
}

void AFarmDayCycle::UpdateLighting()
{
	if (!Sun)
	{
		return;
	}

	// Progress runs 0 to 1 from sunrise to sunset by day, and from sunset to the next sunrise by night.
	// The sun arcs across the sky by day and the moon takes the same path by night.
	const float Hour = FMath::Fmod(TimeOfDay, 24.f);
	const float DayLength = SunsetHour - SunriseHour;
	const bool bDaytime = Hour >= SunriseHour && Hour < SunsetHour;
	const float Progress = bDaytime
		? (Hour - SunriseHour) / DayLength
		: FMath::Fmod(Hour - SunsetHour + 24.f, 24.f) / (24.f - DayLength);
	const float Arc = FMath::Sin(Progress * PI);

	// A few degrees above the horizon at the ends so mornings and evenings get long shadows, not darkness.
	const float Elevation = FMath::Lerp(5.f, MaxSunElevation, Arc);
	Sun->SetActorRotation(FRotator(-Elevation, SunYaw + Progress * 180.f, 0.f));

	// Light fades in after sunrise and out before sunset, warm near the horizon.
	const float Daylight = bDaytime ? FMath::Clamp(Arc * 3.f, 0.f, 1.f) : 0.f;
	const float Brightness = FMath::Lerp(NightLightFraction, 1.f, Daylight);
	ULightComponent* SunLight = Sun->GetLightComponent();
	SunLight->SetIntensity(DaySunIntensity * Brightness);
	SunLight->SetLightColor(bDaytime ? FMath::Lerp(SunriseColor, DaySunColor, FMath::Clamp(Arc * 2.f, 0.f, 1.f)) : MoonColor);

	if (SkyLight)
	{
		SkyLight->GetLightComponent()->SetIntensity(DaySkyIntensity * Brightness);
	}

	// Darken the exposure over the first part of the night and ease it off again toward sunrise,
	// so daytime, mornings and evenings keep the normal exposure.
	const float Darkness = bDaytime ? 0.f : FMath::Clamp(Arc * 6.f, 0.f, 1.f);
	NightPostProcess->Settings.AutoExposureBias = NightExposureBias * Darkness;
}
