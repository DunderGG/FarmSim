// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FarmingComponent.generated.h"

class AFarmDayCycle;
class AFarmGrid;
class AFarmTile;
class UFarmCropData;

UENUM(BlueprintType)
enum class EFarmTool : uint8
{
	Hoe,
	Seeds,
	WateringCan UMETA(DisplayName = "Watering Can"),
	Hand
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFarmToolChanged, EFarmTool);

/**
 * Gives its owner farming tools, a tiny inventory and the ability to sleep.
 * Uses the tile directly in front of the owner and reports what happened on the player's AFarmHUD.
 */
UCLASS(ClassGroup = (Farming), meta = (BlueprintSpawnableComponent))
class UFarmingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFarmingComponent();

	void SelectTool(EFarmTool Tool);
	EFarmTool GetCurrentTool() const { return CurrentTool; }
	void UseTool();
	void Sleep();

	/** The crop the Seeds tool plants, or nullptr if SeedCropId doesn't name a crop asset. Loaded at BeginPlay. */
	const UFarmCropData* GetSeedCrop() const { return SeedCrop; }

	/** Broadcast whenever the current tool changes, including when a save is loaded. */
	FOnFarmToolChanged OnToolChanged;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** How far in front of the owner the targeted tile is, in cm. */
	UPROPERTY(EditAnywhere, Category = "Farming")
	float ReachDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Farming")
	int32 Seeds = 10;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	int32 HarvestedCrops = 0;

protected:
	virtual void BeginPlay() override;

private:
	void LoadSeedCrop();
	AFarmTile* GetTargetTile() const;
	void StartNextDay(const TCHAR* Reason);
	void ShowMessage(const FString& Message) const;

	UPROPERTY()
	TObjectPtr<AFarmGrid> Grid;

	UPROPERTY()
	TObjectPtr<AFarmDayCycle> DayCycle;

	/** Crop asset the Seeds tool plants, picked by its ID (FarmCrop:Base.Turnip). */
	UPROPERTY(EditDefaultsOnly, Category = "Farming", meta = (AllowPrivateAccess = "true", AllowedTypes = "FarmCrop"))
	FPrimaryAssetId SeedCropId;

	UPROPERTY()
	TObjectPtr<const UFarmCropData> SeedCrop;

	EFarmTool CurrentTool = EFarmTool::Hoe;
};
