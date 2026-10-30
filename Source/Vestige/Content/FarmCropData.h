// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "Content/FarmContentData.h"
#include "GameplayTagContainer.h"
#include "FarmCropData.generated.h"

class UStaticMesh;

/**
 * The definition of one crop: how it grows, how it looks and what it is worth, plus the ID and name every content asset has.
 * One asset per crop, found through the Asset Manager by its ContentId. Holds no runtime state; tiles do.
 */
UCLASS()
class UFarmCropData : public UFarmContentData
{
	GENERATED_BODY()

public:
	/** The primary asset type crops are registered under in Config/DefaultGame.ini. */
	static const FPrimaryAssetType AssetType;

	int32 GetDaysToMature() const { return DaysToMature; }
	int32 GetYield() const { return Yield; }

	/** The mesh for a crop that has grown for GrowthDays days, or nullptr if that stage has no mesh or it isn't loaded. */
	UStaticMesh* GetGrowthStageMesh(int32 GrowthDays) const;

protected:
	virtual FPrimaryAssetType GetContentType() const override { return AssetType; }

private:
	/** Seasons the crop grows in. Not used yet: crops grow in any season until seasons exist. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming", meta = (AllowPrivateAccess = "true", Categories = "Farm.Season"))
	FGameplayTagContainer Seasons;

	/** Watered days the crop needs before it can be harvested. It grows one stage per watered day. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Growth", meta = (AllowPrivateAccess = "true", ClampMin = 1))
	int32 DaysToMature = 3;

	/** Tile water level, from 0 (dry) to 1 (soaked), the crop needs to grow that day. Not used yet: any watered tile grows. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Growth", meta = (AllowPrivateAccess = "true", ClampMin = 0, ClampMax = 1))
	float WaterNeed = 0.f;

	/** Highest tile contamination, from 0 (clean) to 1, the crop still grows in. Not used yet: tiles have no contamination. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Growth", meta = (AllowPrivateAccess = "true", ClampMin = 0, ClampMax = 1))
	float ContaminationTolerance = 1.f;

	/**
	 * One mesh per growth stage, from the day planted to ripe (DaysToMature + 1 entries). Origin on the soil surface.
	 * Empty slots fall back to the tile's placeholder shape; with too few entries, the last one is used for later stages.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals", meta = (AllowPrivateAccess = "true", AssetBundles = "Visuals"))
	TArray<TSoftObjectPtr<UStaticMesh>> GrowthStageMeshes;

	/** Crops the player gets from one harvest. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Economy", meta = (AllowPrivateAccess = "true", ClampMin = 1))
	int32 Yield = 1;

	/** Price of one seed. Not used yet: there is no currency. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Economy", meta = (AllowPrivateAccess = "true", ClampMin = 0))
	int32 SeedPrice = 0;

	/** Price one harvested crop sells for. Not used yet: there is no currency. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Economy", meta = (AllowPrivateAccess = "true", ClampMin = 0))
	int32 SellPrice = 0;
};
