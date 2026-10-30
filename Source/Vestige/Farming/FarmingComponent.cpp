// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "Farming/FarmingComponent.h"

#include "Components/SceneComponent.h"
#include "Content/FarmContentData.h"
#include "Content/FarmCropData.h"
#include "Content/FarmToolData.h"
#include "Core/FarmFeedbackSubsystem.h"
#include "Core/Vestige.h"
#include "Engine/World.h"
#include "Farming/FarmFarmingSaveData.h"
#include "Farming/FarmGrid.h"
#include "Farming/FarmTile.h"
#include "Farming/FarmTileTargeting.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
#include "TimerManager.h"

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

		// Until the first movement input, aim where the owner faces.
		const FIntPoint FacingStep = FarmTileTargeting::SnapToStep(Grid->ToGridDirection(GetOwner()->GetActorForwardVector()));
		if (FacingStep != FIntPoint::ZeroValue)
		{
			AimStep = FacingStep;
		}
		OwnerCoordinates = Grid->GetCoordinatesAt(GetOwner()->GetActorLocation());
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
	TRACE_CPUPROFILER_EVENT_SCOPE(UFarmingComponent::LoadSeedCrop);

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
	TRACE_CPUPROFILER_EVENT_SCOPE(UFarmingComponent::LoadToolbar);

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

bool UFarmingComponent::RestoreCurrentTool(FName ToolContentId)
{
	const bool bFound = Toolbar.SelectTool(ToolContentId);
	if (!bFound)
	{
		UE_LOG(
			LogVestige, Warning, TEXT("The saved tool %s isn't on the toolbar, so the first slot is in hand."), *ToolContentId.ToString());
		Toolbar.SelectSlot(0);
	}
	OnToolChanged.Broadcast(GetCurrentTool());
	return bFound;
}

FFarmFarmingSaveData UFarmingComponent::GetSaveData() const
{
	FFarmFarmingSaveData Data;
	Data.Seeds = Seeds;
	Data.HarvestedCrops = HarvestedCrops;
	const UFarmToolData* Tool = GetCurrentTool();
	Data.CurrentToolId = Tool ? Tool->GetContentId() : NAME_None;
	return Data;
}

bool UFarmingComponent::RestoreFromSaveData(const FFarmFarmingSaveData& Data)
{
	Seeds = FMath::Max(0, Data.Seeds);
	HarvestedCrops = FMath::Max(0, Data.HarvestedCrops);
	OnInventoryChanged.Broadcast();
	return RestoreCurrentTool(Data.CurrentToolId);
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

void UFarmingComponent::AimAt(const FVector& WorldDirection)
{
	if (!Grid)
	{
		return;
	}

	// Called every frame while a movement input is held, so the usual case stops here: no input, or the same direction.
	const FIntPoint Step = FarmTileTargeting::SnapToStep(Grid->ToGridDirection(WorldDirection));
	if (Step == FIntPoint::ZeroValue || Step == AimStep)
	{
		return;
	}
	AimStep = Step;
	UpdateTargetTile();
}

FVector UFarmingComponent::GetAimDirection() const
{
	return Grid ? Grid->ToWorldDirection(AimStep) : FVector::ZeroVector;
}

AFarmTile* UFarmingComponent::FindTargetTile() const
{
	const AActor* Owner = GetOwner();
	if (!Grid || !Owner)
	{
		return nullptr;
	}
	return Grid->GetTileAt(FarmTileTargeting::GetNeighbour(Grid->GetCoordinatesAt(Owner->GetActorLocation()), AimStep));
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
	// Fires on every move and turn; the target only changes when the owner reaches another tile.
	const AActor* Owner = GetOwner();
	const FIntPoint Coordinates = Grid && Owner ? Grid->GetCoordinatesAt(Owner->GetActorLocation()) : OwnerCoordinates;
	if (Coordinates == OwnerCoordinates)
	{
		return;
	}
	OwnerCoordinates = Coordinates;
	UpdateTargetTile();
}

void UFarmingComponent::Notify(const FText& Message) const
{
	UFarmFeedbackSubsystem::Notify(GetWorld(), Message);
}

#undef LOCTEXT_NAMESPACE
