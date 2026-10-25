#include "FarmingComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "FarmGrid.h"
#include "FarmTile.h"

namespace
{
	// Fixed keys so the HUD lines update in place instead of stacking up.
	constexpr uint64 HudStatusKey = 0xFA4D0001;
	constexpr uint64 HudControlsKey = 0xFA4D0002;
}

UFarmingComponent::UFarmingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFarmingComponent::BeginPlay()
{
	Super::BeginPlay();

	// The prototype has a single field, so just use the first grid in the level.
	TActorIterator<AFarmGrid> It(GetWorld());
	Grid = It ? *It : nullptr;
	if (!Grid)
	{
		ShowMessage(TEXT("No FarmGrid in the level. Drag one in from the Place Actors panel."));
	}
}

void UFarmingComponent::SelectTool(EFarmTool Tool)
{
	CurrentTool = Tool;
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
	if (!Grid)
	{
		return;
	}
	Grid->AdvanceDay();
	ShowMessage(FString::Printf(TEXT("You slept. Day %d begins."), Grid->GetCurrentDay()));
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

	if (GEngine)
	{
		const int32 Day = Grid ? Grid->GetCurrentDay() : 0;
		const FString ToolName = UEnum::GetDisplayValueAsText(CurrentTool).ToString();
		GEngine->AddOnScreenDebugMessage(HudControlsKey, 0.f, FColor::Silver,
			TEXT("1 Hoe   2 Seeds   3 Watering Can   4 Hand   |   E Use tool   |   N Sleep"));
		GEngine->AddOnScreenDebugMessage(HudStatusKey, 0.f, FColor::White,
			FString::Printf(TEXT("Day %d   Tool: %s   Seeds: %d   Harvested: %d"), Day, *ToolName, Seeds, HarvestedCrops));
	}
}

void UFarmingComponent::ShowMessage(const FString& Message) const
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, Message);
	}
}
