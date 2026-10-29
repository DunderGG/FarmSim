// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmingComponent.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "FarmContentData.h"
#include "FarmCropData.h"
#include "FarmFeedbackSubsystem.h"
#include "FarmGrid.h"
#include "FarmTile.h"
#include "FarmToolData.h"
#include "TimerManager.h"
#include "Vestige.h"

#define LOCTEXT_NAMESPACE "FarmingComponent"

UFarmingComponent::UFarmingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFarmingComponent::BeginPlay()
{
	Super::BeginPlay();

	Seeds = StartingSeeds;
	LoadSeedCrop();
	LoadToolbar();
	Grid = AFarmGrid::Find(GetWorld());
	if (Grid)
	{
		// The grid spawns its tiles in its own BeginPlay, which may run after this one.
		Grid->OnTilesReady.AddUObject(this, &UFarmingComponent::UpdateTargetTile);
	}
	else
	{
		UE_LOG(LogVestige, Warning, TEXT("No FarmGrid in %s, so there is nothing to farm."), *GetWorld()->GetMapName());

		// The HUD may not be listening for feedback yet during BeginPlay.
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			[WeakThis = TWeakObjectPtr<UFarmingComponent>(this)]()
			{
				if (const UFarmingComponent* Farming = WeakThis.Get())
				{
					Farming->Notify(LOCTEXT("NoGrid", "No FarmGrid in the level. Drag one in from the Place Actors panel."));
				}
			});
	}

	if (USceneComponent* Root = GetOwner() ? GetOwner()->GetRootComponent() : nullptr)
	{
		Root->TransformUpdated.AddUObject(this, &UFarmingComponent::FollowOwnerTransform);
	}
	UpdateTargetTile();
}

void UFarmingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Grid)
	{
		Grid->OnTilesReady.RemoveAll(this);
	}
	if (USceneComponent* Root = GetOwner() ? GetOwner()->GetRootComponent() : nullptr)
	{
		Root->TransformUpdated.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UFarmingComponent::LoadSeedCrop()
{
	// Waiting is fine at play start, and with the meshes loaded now, planting never stalls on a load.
	SeedCrop = Cast<UFarmCropData>(UFarmContentData::LoadWithVisuals(SeedCropId));
	if (!SeedCrop)
	{
		UE_LOG(LogVestige, Warning, TEXT("%s's Seed Crop Id (%s) names no crop asset, so nothing can be planted."), *GetPathName(),
			*SeedCropId.ToString());
	}
}

void UFarmingComponent::LoadToolbar()
{
	if (ToolbarIds.IsEmpty())
	{
		UE_LOG(LogVestige, Warning, TEXT("%s has no Toolbar Ids, so the farmer has no tools."), *GetPathName());
	}

	// A missing tool leaves its slot empty, so the other tools keep their keys.
	TArray<TObjectPtr<const UFarmToolData>> Tools;
	for (const FPrimaryAssetId& ToolId : ToolbarIds)
	{
		const UFarmToolData* Tool = Cast<UFarmToolData>(UFarmContentData::LoadWithVisuals(ToolId));
		if (!Tool)
		{
			UE_LOG(LogVestige, Warning, TEXT("Toolbar slot %d of %s (%s) names no tool asset, so the slot is empty."), Tools.Num() + 1,
				*GetPathName(), *ToolId.ToString());
		}
		Tools.Add(Tool);
	}
	Toolbar.SetTools(MoveTemp(Tools));
	OnToolChanged.Broadcast(GetCurrentTool());
}

void UFarmingComponent::SelectToolbarSlot(int32 Slot)
{
	if (Toolbar.SelectSlot(Slot))
	{
		OnToolChanged.Broadcast(GetCurrentTool());
	}
}

void UFarmingComponent::RestoreCurrentTool(FName ToolContentId)
{
	if (!Toolbar.SelectTool(ToolContentId))
	{
		UE_LOG(
			LogVestige, Warning, TEXT("The saved tool %s isn't on the toolbar, so the first slot is in hand."), *ToolContentId.ToString());
		Toolbar.SelectSlot(0);
	}
	OnToolChanged.Broadcast(GetCurrentTool());
}

void UFarmingComponent::RestoreInventory(int32 SavedSeeds, int32 SavedHarvestedCrops)
{
	Seeds = FMath::Max(0, SavedSeeds);
	HarvestedCrops = FMath::Max(0, SavedHarvestedCrops);
	OnInventoryChanged.Broadcast();
}

void UFarmingComponent::UseTool()
{
	AFarmTile* Tile = FindTargetTile();
	if (!Tile)
	{
		Notify(LOCTEXT("NoTile", "Stand facing a farm tile."));
		return;
	}

	const UFarmToolData* Tool = GetCurrentTool();
	if (!Tool)
	{
		Notify(LOCTEXT("NoTool", "No tool in this slot."));
		return;
	}

	switch (Tool->GetAction())
	{
	case EFarmToolAction::Till:
		Notify(Tile->Till() ? LOCTEXT("Tilled", "Tilled the soil.") : LOCTEXT("CantTill", "Only grass can be tilled."));
		break;

	case EFarmToolAction::Plant:
		if (Seeds <= 0 || !SeedCrop)
		{
			Notify(LOCTEXT("OutOfSeeds", "Out of seeds."));
		}
		else if (Tile->Plant(SeedCrop))
		{
			--Seeds;
			OnInventoryChanged.Broadcast();
			Notify(LOCTEXT("Planted", "Planted a seed."));
		}
		else
		{
			Notify(LOCTEXT("CantPlant", "Seeds need empty tilled soil."));
		}
		break;

	case EFarmToolAction::Water:
		Notify(Tile->Water() ? LOCTEXT("Watered", "Watered.") : LOCTEXT("CantWater", "Nothing to water here."));
		break;

	case EFarmToolAction::Harvest:
		if (const UFarmCropData* RipeCrop = Tile->GetCrop(); RipeCrop && Tile->Harvest())
		{
			HarvestedCrops += RipeCrop->GetYield();
			OnInventoryChanged.Broadcast();
			Notify(LOCTEXT("Harvested", "Harvested a crop!"));
		}
		else
		{
			Notify(LOCTEXT("NothingRipe", "Nothing ripe here."));
		}
		break;
	}
}

AFarmTile* UFarmingComponent::FindTargetTile() const
{
	const AActor* Owner = GetOwner();
	if (!Grid || !Owner)
	{
		return nullptr;
	}
	return Grid->GetTileAtLocation(Owner->GetActorLocation() + Owner->GetActorForwardVector() * ReachDistance);
}

void UFarmingComponent::UpdateTargetTile()
{
	AFarmTile* NewTarget = FindTargetTile();
	if (NewTarget == TargetTile.Get())
	{
		return;
	}
	TargetTile = NewTarget;
	OnTargetTileChanged.Broadcast(NewTarget);
}

void UFarmingComponent::FollowOwnerTransform(
	USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	UpdateTargetTile();
}

void UFarmingComponent::Notify(const FText& Message) const
{
	UFarmFeedbackSubsystem::Notify(GetWorld(), Message);
}

#undef LOCTEXT_NAMESPACE
