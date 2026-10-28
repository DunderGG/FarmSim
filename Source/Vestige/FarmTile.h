// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmTile.generated.h"

class UFarmCropData;
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
	/** Also returns false without a crop to plant. */
	bool Plant(const UFarmCropData* InCrop);
	bool Water();
	bool Harvest();

	/** Called once per in-game day. Watered crops grow one stage, then the soil dries out. */
	void AdvanceDay();

	EFarmTileState GetState() const { return State; }
	bool IsWatered() const { return bWatered; }
	bool IsRipe() const;
	int32 GetGrowthDays() const { return GrowthDays; }

	/** The crop growing here, or nullptr if nothing is planted. */
	const UFarmCropData* GetCrop() const { return Crop; }

	/** Puts the tile straight into a saved state. A planted state without a crop restores as empty tilled soil. */
	void RestoreState(EFarmTileState SavedState, bool bSavedWatered, int32 SavedGrowthDays, const UFarmCropData* SavedCrop);

	// Optional art. Anything left empty falls back to the colored placeholder shapes.

	/** Mesh for tilled, planted and watered soil. Origin at the bottom center, 1 m wide. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals")
	TObjectPtr<UStaticMesh> TilledSoilMesh;

	/** Material swapped onto TilledSoilMesh while the soil is watered. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals")
	TObjectPtr<UMaterialInterface> WateredSoilMaterial;

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

	/** Set exactly while the tile is planted. Whoever loads the crop also loads its meshes. */
	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	TObjectPtr<const UFarmCropData> Crop;
};
