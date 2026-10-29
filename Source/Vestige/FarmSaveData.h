// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "FarmTileState.h"
#include "FarmSaveData.generated.h"

// The parts of a save, one per owner. Each owner fills its part and restores from it; UFarmSaveGame only holds them.
// Plain values and content IDs only: never object pointers, asset paths or positions in a list.

/**
 * The hard-coded tools from before tools were data assets. Only read when upgrading a version 0 save.
 * The names must stay as they are, because those saves store the enum by name.
 */
UENUM()
enum class EFarmTool : uint8
{
	Hoe,
	Seeds,
	WateringCan,
	Hand
};

/** AFarmDayCycle's part of a save. */
USTRUCT()
struct FFarmClockSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Day = 1;

	/** Hours since midnight at the start of the day. */
	UPROPERTY()
	float TimeOfDay = 0.f;
};

/** UFarmingComponent's part of a save. */
USTRUCT()
struct FFarmFarmingSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Seeds = 0;

	UPROPERTY()
	int32 HarvestedCrops = 0;

	/** ContentId of the tool in hand (Base.Hoe). */
	UPROPERTY()
	FName CurrentToolId;
};

/** One tile's part of a save. AFarmTile fills the state and crop; AFarmGrid adds where the tile is. */
USTRUCT()
struct FFarmTileSaveData
{
	GENERATED_BODY()

	/** The tile's column (X) and row (Y) in the grid. Version 0 saves have none; the upgrade adds them. */
	UPROPERTY()
	FIntPoint Coordinates = FIntPoint::ZeroValue;

	UPROPERTY()
	EFarmTileState State = EFarmTileState::Grass;

	UPROPERTY()
	bool bWatered = false;

	UPROPERTY()
	int32 GrowthDays = 0;

	/** ContentId of the crop growing here (Base.Turnip), or None. */
	UPROPERTY()
	FName CropId;
};
