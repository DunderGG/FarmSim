// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FarmToolbar.h"
#include "FarmingComponent.generated.h"

class AFarmGrid;
class AFarmTile;
class UFarmCropData;
class UFarmToolData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFarmToolChanged, const UFarmToolData* /* Tool */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFarmTargetTileChanged, const AFarmTile* /* Tile */);

/**
 * Gives its owner farming tools and a tiny inventory.
 * Uses the tile directly in front of the owner, and tells the player what happened through UFarmFeedbackSubsystem.
 */
UCLASS(ClassGroup = (Farming), meta = (BlueprintSpawnableComponent))
class UFarmingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFarmingComponent();

	/** Puts the tool in a toolbar slot (0 is the first) in hand. Does nothing if the slot doesn't exist. */
	void SelectToolbarSlot(int32 Slot);

	/** Puts the saved tool in hand, by its ContentId. Falls back to the first slot, with a warning, if no slot holds it. */
	void RestoreCurrentTool(FName ToolContentId);

	/** The tool in hand, or nullptr if its slot is empty. */
	const UFarmToolData* GetCurrentTool() const { return Toolbar.GetCurrentTool(); }
	const FFarmToolbar& GetToolbar() const { return Toolbar; }

	/** Uses the tool in hand on the tile in front of the owner. Does nothing to the farm without a tile or a tool. */
	void UseTool();

	/** The crop the Seeds tool plants, or nullptr if SeedCropId doesn't name a crop asset. Loaded at BeginPlay. */
	const UFarmCropData* GetSeedCrop() const { return SeedCrop; }

	/** The tile in front of the owner, or nullptr if it isn't facing one. Updated whenever the owner moves or turns. */
	const AFarmTile* GetTargetTile() const { return TargetTile.Get(); }

	/** Broadcast whenever the current tool changes, including when a save is loaded. */
	FOnFarmToolChanged OnToolChanged;

	/** Broadcast when the tile in front of the owner changes, with nullptr when it stops facing one. */
	FOnFarmTargetTileChanged OnTargetTileChanged;

	/** How far in front of the owner the targeted tile is, in cm. */
	UPROPERTY(EditAnywhere, Category = "Farming")
	float ReachDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Farming")
	int32 Seeds = 10;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	int32 HarvestedCrops = 0;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void LoadSeedCrop();
	void LoadToolbar();
	/** The tile ReachDistance in front of the owner right now, or nullptr. */
	AFarmTile* FindTargetTile() const;

	/** Finds the target tile again and broadcasts OnTargetTileChanged if it changed. */
	void UpdateTargetTile();

	/** Bound to the owner's root component, so the target follows every move and turn without ticking. */
	void FollowOwnerTransform(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

	/** Tells the player what happened, through UFarmFeedbackSubsystem. */
	void Notify(const FText& Message) const;

	UPROPERTY()
	TObjectPtr<AFarmGrid> Grid;

	TWeakObjectPtr<AFarmTile> TargetTile;

	/** Crop asset the Seeds tool plants, picked by its ID (FarmCrop:Base.Turnip). */
	UPROPERTY(EditDefaultsOnly, Category = "Farming", meta = (AllowPrivateAccess = "true", AllowedTypes = "FarmCrop"))
	FPrimaryAssetId SeedCropId;

	UPROPERTY()
	TObjectPtr<const UFarmCropData> SeedCrop;

	/**
	 * Tool assets in the toolbar slots, in order (FarmTool:Base.Hoe).
	 * Keys 1 to 9 select the first nine; the first is in hand at the start.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Farming", meta = (AllowPrivateAccess = "true", AllowedTypes = "FarmTool"))
	TArray<FPrimaryAssetId> ToolbarIds;

	/** The tools loaded from ToolbarIds at BeginPlay, and the one in hand. */
	UPROPERTY()
	FFarmToolbar Toolbar;
};
