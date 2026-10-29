// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "FarmTileState.generated.h"

class UFarmCropData;

/** What is on a farm tile. Watering is separate, because tilled and planted soil can both be wet. */
UENUM(BlueprintType)
enum class EFarmTileState : uint8
{
	Grass,
	Tilled,
	Planted
};

/**
 * One tile's farming state and the rules that change it: tilling, planting, watering, growing and harvesting.
 * A plain struct, so the rules can be tested without spawning a tile. AFarmTile owns one and shows it.
 * Every change goes through a function that checks it, so impossible states (a planted tile without a crop) can't happen.
 */
USTRUCT()
struct FFarmTileState
{
	GENERATED_BODY()

public:
	/** Turns grass into tilled soil. Returns false, changing nothing, if the tile isn't grass. */
	bool Till();

	/** Plants InCrop in tilled soil. Returns false, changing nothing, if the soil isn't empty and tilled or InCrop is null. */
	bool Plant(const UFarmCropData* InCrop);

	/** Waters tilled or planted soil until the next day. Returns false if the tile is grass or already watered. */
	bool Water();

	/** Removes a ripe crop, leaving tilled soil. Returns false, changing nothing, if nothing ripe grows here. */
	bool Harvest();

	/** Called once per in-game day. A watered crop grows one stage, then the soil dries out. */
	void AdvanceDay();

	/**
	 * Puts the tile straight into a saved state, without replaying any action. A planted state without a crop restores
	 * as empty tilled soil, watered grass as dry grass, and growth is clamped to what the crop can reach.
	 */
	void Restore(EFarmTileState SavedState, bool bSavedWatered, int32 SavedGrowthDays, const UFarmCropData* SavedCrop);

	EFarmTileState GetState() const { return State; }
	bool IsWatered() const { return bWatered; }
	bool IsRipe() const;
	int32 GetGrowthDays() const { return GrowthDays; }

	/** The crop growing here, or nullptr if nothing is planted. */
	const UFarmCropData* GetCrop() const { return Crop; }

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	EFarmTileState State = EFarmTileState::Grass;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	bool bWatered = false;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	int32 GrowthDays = 0;

	/** Set exactly while the tile is planted. Whoever loads the crop also loads its meshes. */
	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	TObjectPtr<const UFarmCropData> Crop;
};
