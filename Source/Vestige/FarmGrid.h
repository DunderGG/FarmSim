// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmGrid.generated.h"

class AFarmTile;
class UBoxComponent;

/**
 * A rectangle of farm tiles, spawned at BeginPlay with the actor's location as one corner.
 * Advances every tile when the level's AFarmDayCycle starts a new day.
 */
UCLASS()
class AFarmGrid : public AActor
{
	GENERATED_BODY()

public:
	AFarmGrid();

	/** Returns the tile under a world location, ignoring height, or nullptr if it's outside the grid. */
	AFarmTile* GetTileAtLocation(const FVector& WorldLocation) const;

	/** Every tile, row by row. Empty until BeginPlay has spawned them. */
	const TArray<TObjectPtr<AFarmTile>>& GetTiles() const { return Tiles; }

	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 1))
	int32 Width = 8;

	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 1))
	int32 Height = 8;

	/** Size of one tile in Unreal units (cm). */
	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 10))
	float TileSize = 100.f;

	/** Tile actor to spawn. Swap in a Blueprint subclass later to change how tiles look. */
	UPROPERTY(EditAnywhere, Category = "Farming")
	TSubclassOf<AFarmTile> TileClass;

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

private:
	/** Grows watered crops and dries the soil on every tile. */
	void AdvanceTiles(int32 NewDay);

	/** Outlines the grid in the editor so it's easy to place. Hidden in game. */
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UBoxComponent> Bounds;

	UPROPERTY()
	TArray<TObjectPtr<AFarmTile>> Tiles;
};
