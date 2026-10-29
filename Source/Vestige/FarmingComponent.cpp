// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmingComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "FarmContentData.h"
#include "FarmCropData.h"
#include "FarmDayCycle.h"
#include "FarmGrid.h"
#include "FarmHUD.h"
#include "FarmTile.h"
#include "FarmToolData.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Vestige.h"

UFarmingComponent::UFarmingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFarmingComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadSeedCrop();
	LoadToolbar();
	DayCycle = AFarmDayCycle::Get(GetWorld());
	if (DayCycle)
	{
		DayCycle->OnDayEndReached.AddUObject(this, &UFarmingComponent::PassOut);
	}
	Grid = AFarmGrid::Find(GetWorld());
	if (!Grid)
	{
		// The player's HUD may not exist yet during BeginPlay.
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			[WeakThis = TWeakObjectPtr<UFarmingComponent>(this)]()
			{
				if (const UFarmingComponent* Farming = WeakThis.Get())
				{
					Farming->ShowMessage(TEXT("No FarmGrid in the level. Drag one in from the Place Actors panel."));
				}
			});
	}
}

void UFarmingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DayCycle)
	{
		DayCycle->OnDayEndReached.RemoveAll(this);
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

void UFarmingComponent::UseTool()
{
	AFarmTile* Tile = GetTargetTile();
	if (!Tile)
	{
		ShowMessage(TEXT("Stand facing a farm tile."));
		return;
	}

	const UFarmToolData* Tool = GetCurrentTool();
	if (!Tool)
	{
		ShowMessage(TEXT("No tool in this slot."));
		return;
	}

	switch (Tool->GetAction())
	{
	case EFarmToolAction::Till:
		ShowMessage(Tile->Till() ? TEXT("Tilled the soil.") : TEXT("Only grass can be tilled."));
		break;

	case EFarmToolAction::Plant:
		if (Seeds <= 0 || !SeedCrop)
		{
			ShowMessage(TEXT("Out of seeds."));
		}
		else if (Tile->Plant(SeedCrop))
		{
			--Seeds;
			ShowMessage(TEXT("Planted a seed."));
		}
		else
		{
			ShowMessage(TEXT("Seeds need empty tilled soil."));
		}
		break;

	case EFarmToolAction::Water:
		ShowMessage(Tile->Water() ? TEXT("Watered.") : TEXT("Nothing to water here."));
		break;

	case EFarmToolAction::Harvest:
		if (const UFarmCropData* RipeCrop = Tile->GetCrop(); RipeCrop && Tile->Harvest())
		{
			HarvestedCrops += RipeCrop->GetYield();
			ShowMessage(TEXT("Harvested a crop!"));
		}
		else
		{
			ShowMessage(TEXT("Nothing ripe here."));
		}
		break;
	}
}

void UFarmingComponent::Sleep()
{
	StartNextDay(TEXT("You slept."));
}

void UFarmingComponent::PassOut(int32 EndedDay)
{
	StartNextDay(TEXT("You stayed up too late and passed out."));
}

void UFarmingComponent::StartNextDay(const TCHAR* Reason)
{
	// Starting a day doesn't need the grid: the grid listens to OnNewDay, and without one there is nothing to grow.
	if (!DayCycle)
	{
		return;
	}

	// Shown first so that anything reacting to the new day, like saving, can add to it.
	ShowMessage(FString::Printf(TEXT("%s Day %d begins."), Reason, DayCycle->GetCurrentDay() + 1));
	DayCycle->StartNextDay();
}

AFarmTile* UFarmingComponent::GetTargetTile() const
{
	const AActor* Owner = GetOwner();
	if (!Grid || !Owner)
	{
		return nullptr;
	}
	return Grid->GetTileAtLocation(Owner->GetActorLocation() + Owner->GetActorForwardVector() * ReachDistance);
}

void UFarmingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (const AFarmTile* Tile = GetTargetTile())
	{
		FVector Origin, Extent;
		Tile->GetActorBounds(false, Origin, Extent);
		DrawDebugBox(GetWorld(), Origin, Extent + FVector(2.f), FColor::Yellow, false, -1.f, 0, 2.f);
	}
}

void UFarmingComponent::ShowMessage(const FString& Message) const
{
	if (AFarmHUD* Hud = AFarmHUD::GetFor(Cast<APawn>(GetOwner())))
	{
		Hud->ShowMessage(Message);
	}
}
