// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmingComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FarmDayCycle.h"
#include "FarmGrid.h"
#include "FarmHUD.h"
#include "FarmTile.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

UFarmingComponent::UFarmingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFarmingComponent::BeginPlay()
{
	Super::BeginPlay();

	DayCycle = AFarmDayCycle::Get(GetWorld());

	// The prototype has a single field, so just use the first grid in the level.
	TActorIterator<AFarmGrid> It(GetWorld());
	Grid = It ? *It : nullptr;
	if (!Grid)
	{
		// The player's HUD may not exist yet during BeginPlay.
		GetWorld()->GetTimerManager().SetTimerForNextTick([WeakThis = TWeakObjectPtr<UFarmingComponent>(this)]()
		{
			if (const UFarmingComponent* Farming = WeakThis.Get())
			{
				Farming->ShowMessage(TEXT("No FarmGrid in the level. Drag one in from the Place Actors panel."));
			}
		});
	}
}

void UFarmingComponent::SelectTool(EFarmTool Tool)
{
	CurrentTool = Tool;
	OnToolChanged.Broadcast(Tool);
}

void UFarmingComponent::UseTool()
{
	AFarmTile* Tile = GetTargetTile();
	if (!Tile)
	{
		ShowMessage(TEXT("Stand facing a farm tile."));
		return;
	}

	switch (CurrentTool)
	{
	case EFarmTool::Hoe:
		ShowMessage(Tile->Till() ? TEXT("Tilled the soil.") : TEXT("Only grass can be tilled."));
		break;

	case EFarmTool::Seeds:
		if (Seeds <= 0)
		{
			ShowMessage(TEXT("Out of seeds."));
		}
		else if (Tile->Plant())
		{
			--Seeds;
			ShowMessage(TEXT("Planted a seed."));
		}
		else
		{
			ShowMessage(TEXT("Seeds need empty tilled soil."));
		}
		break;

	case EFarmTool::WateringCan:
		ShowMessage(Tile->Water() ? TEXT("Watered.") : TEXT("Nothing to water here."));
		break;

	case EFarmTool::Hand:
		if (Tile->Harvest())
		{
			++HarvestedCrops;
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

void UFarmingComponent::StartNextDay(const TCHAR* Reason)
{
	if (!Grid || !DayCycle)
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

	if (DayCycle && DayCycle->IsPastBedtime())
	{
		StartNextDay(TEXT("You stayed up too late and passed out."));
	}

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
