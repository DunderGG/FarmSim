// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FarmTargetHighlightComponent.generated.h"

class AFarmTile;
class UFarmingComponent;
class UMaterialInterface;

/**
 * Draws a frame around the tile the farmer is facing. Presentation only: it follows its owner's
 * UFarmingComponent::OnTargetTileChanged and never ticks. Unlike debug drawing, it shows in shipping builds.
 * The frame is four thin bars of one instanced mesh, placed in the tile's own space so it scales with the grid.
 */
UCLASS(ClassGroup = (Farming), meta = (BlueprintSpawnableComponent))
class UFarmTargetHighlightComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	UFarmTargetHighlightComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Adds the four bars of the frame, around a tile of AFarmTile::UnscaledSize. */
	void BuildFrame();

	/** Moves the frame onto Tile, or hides it when Tile is null. Bound to UFarmingComponent::OnTargetTileChanged. */
	void ShowOn(const AFarmTile* Tile);

	/**
	 * Material for the frame. It needs a vector parameter called Color. The default is the engine's unlit mesh material,
	 * so the frame stays visible at night; a project material can replace it.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> HighlightMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals", meta = (AllowPrivateAccess = "true"))
	FLinearColor HighlightColor = FLinearColor(1.f, 0.85f, 0.1f);

	/** Width of each bar of the frame, in cm on a tile of unscaled size. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals", meta = (AllowPrivateAccess = "true", ClampMin = 0.5))
	float BarWidth = 3.f;

	/** Height of the frame above the tile's origin, in cm. Above the soil and the bottom of the crops. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|Visuals", meta = (AllowPrivateAccess = "true", ClampMin = 0))
	float FrameHeight = 8.f;

	UPROPERTY()
	TWeakObjectPtr<UFarmingComponent> Farming;
};
