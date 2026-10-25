#include "FarmingComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FarmGrid.h"
#include "FarmHud.h"
#include "FarmTile.h"
#include "GameFramework/Pawn.h"

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

void UFarmingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveHud();
	Super::EndPlay(EndPlayReason);
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

int32 UFarmingComponent::GetCurrentDay() const
{
	return Grid ? Grid->GetCurrentDay() : 0;
}

bool UFarmingComponent::HasMessage() const
{
	const UWorld* World = GetWorld();
	return !Message.IsEmpty() && World && World->GetTimeSeconds() < MessageEndTime;
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

	// Possession can happen before or after BeginPlay, so keep checking until the HUD is up.
	if (!Hud)
	{
		AddHudIfLocalPlayer();
	}
}

void UFarmingComponent::ShowMessage(const FString& NewMessage)
{
	Message = NewMessage;
	MessageEndTime = GetWorld() ? GetWorld()->GetTimeSeconds() + MessageDuration : 0.0;
}

void UFarmingComponent::AddHudIfLocalPlayer()
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	UGameViewportClient* Viewport = GetWorld() ? GetWorld()->GetGameViewport() : nullptr;
	if (!Pawn || !Pawn->IsPlayerControlled() || !Pawn->IsLocallyControlled() || !Viewport)
	{
		return;
	}

	Hud = SNew(SFarmHud).Farming(this);
	Viewport->AddViewportWidgetContent(Hud.ToSharedRef());
}

void UFarmingComponent::RemoveHud()
{
	if (!Hud)
	{
		return;
	}
	if (UGameViewportClient* Viewport = GetWorld() ? GetWorld()->GetGameViewport() : nullptr)
	{
		Viewport->RemoveViewportWidgetContent(Hud.ToSharedRef());
	}
	Hud.Reset();
}
