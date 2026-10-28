// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "FarmingComponent.h"
#include "FarmTile.h"
#include "GameFramework/SaveGame.h"
#include "FarmSaveGame.generated.h"

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

/** Everything needed to pick the farm back up: the clock, the player's inventory and every tile, in grid order. */
UCLASS()
class UFarmSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static constexpr const TCHAR* SlotName = TEXT("Farm");
	static constexpr int32 UserIndex = 0;

	UPROPERTY()
	int32 Day = 1;

	UPROPERTY()
	float TimeOfDay = 6.f;

	UPROPERTY()
	int32 Seeds = 0;

	UPROPERTY()
	int32 HarvestedCrops = 0;

	UPROPERTY()
	EFarmTool CurrentTool = EFarmTool::Hoe;

	UPROPERTY()
	TArray<FFarmTileSaveData> Tiles;
};
