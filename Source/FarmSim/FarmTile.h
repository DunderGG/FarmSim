// Copyright (C) 2026 David Bennehag and the FarmSim contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-FarmSim-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmTile.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;

UENUM(BlueprintType)
enum class EFarmTileState : uint8
{
	Grass,
	Tilled,
	Planted
};

/** One square of farmland: can be tilled, planted, watered and harvested. */
UCLASS()
class AFarmTile : public AActor
{
	GENERATED_BODY()

public:
	AFarmTile();

	// Each action returns true if it changed the tile.
	bool Till();
	bool Plant();
	bool Water();
	bool Harvest();

	/** Called once per in-game day. Watered crops grow one stage, then the soil dries out. */
	void AdvanceDay();

	EFarmTileState GetState() const { return State; }
	bool IsWatered() const { return bWatered; }
	bool IsRipe() const { return State == EFarmTileState::Planted && GrowthDays >= DaysToMature; }
	int32 GetGrowthDays() const { return GrowthDays; }

	/** Puts the tile straight into a saved state. */
	void RestoreState(EFarmTileState SavedState, bool bSavedWatered, int32 SavedGrowthDays);

	/** Days of watering a crop needs before it can be harvested. */
	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 1))
	int32 DaysToMature = 3;

	// Optional art. Anything left empty falls back to the colored placeholder shapes.

	/** Mesh for tilled, planted and watered soil. Origin at the bottom center, 1 m wide. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals")
	TObjectPtr<UStaticMesh> TilledSoilMesh;

	/** Material swapped onto TilledSoilMesh while the soil is watered. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals")
	TObjectPtr<UMaterialInterface> WateredSoilMaterial;

	/** One mesh per growth stage, from the day planted to ripe (DaysToMature + 1 entries). Origin on the soil surface. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals")
	TArray<TObjectPtr<UStaticMesh>> CropStageMeshes;

	/** Height of the soil surface that custom crop meshes stand on, in cm. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals")
	float SoilSurfaceHeight = 6.f;

protected:
	virtual void BeginPlay() override;

private:
	void RefreshVisuals();
	void RefreshSoil();
	void RefreshCrop();

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UStaticMeshComponent> SoilMesh;

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UStaticMeshComponent> CropMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> PlaceholderSoilMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> PlaceholderCropMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SoilMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CropMaterial;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	EFarmTileState State = EFarmTileState::Grass;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	bool bWatered = false;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	int32 GrowthDays = 0;
};
