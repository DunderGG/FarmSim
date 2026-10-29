// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "FarmTileState.h"
#include "FarmSaveData.generated.h"

/**
 * The hard-coded tools from before tools were data assets. Only kept so that saves made then still load their tool.
 * The names must stay as they are, because saves store the enum by name.
 */
UENUM()
enum class EFarmTool : uint8
{
	Hoe,
	Seeds,
	WateringCan,
	Hand
};

/** One tile's saved state. */
USTRUCT()
struct FFarmTileSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	EFarmTileState State = EFarmTileState::Grass;

	UPROPERTY()
	bool bWatered = false;

	UPROPERTY()
	int32 GrowthDays = 0;
};
