#include "FarmingComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FarmDayCycle.h"
#include "FarmGrid.h"
#include "FarmHud.h"
#include "FarmSaveGame.h"
#include "FarmTile.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
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
		ShowMessage(TEXT("No FarmGrid in the level. Drag one in from the Place Actors panel."));
		return;
	}

	// The grid spawns its tiles in its own BeginPlay, which may not have run yet.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UFarmingComponent::LoadFarm);
}

void UFarmingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveHud();
	Super::EndPlay(EndPlayReason);
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
	DayCycle->StartNextDay();
	SaveFarm();
	ShowMessage(FString::Printf(TEXT("%s Day %d begins. Game saved."), Reason, DayCycle->GetCurrentDay()));
}

void UFarmingComponent::StartNewGame()
{
	UGameplayStatics::DeleteGameInSlot(UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex);
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void UFarmingComponent::SaveFarm()
{
	UFarmSaveGame* Save = Cast<UFarmSaveGame>(UGameplayStatics::CreateSaveGameObject(UFarmSaveGame::StaticClass()));
	Save->Day = DayCycle->GetCurrentDay();
	Save->TimeOfDay = DayCycle->GetTimeOfDay();
	Save->Seeds = Seeds;
	Save->HarvestedCrops = HarvestedCrops;
	Save->CurrentTool = CurrentTool;
	for (const AFarmTile* Tile : Grid->GetTiles())
	{
		FFarmTileSaveData& TileData = Save->Tiles.AddDefaulted_GetRef();
		if (Tile)
		{
			TileData.State = Tile->GetState();
			TileData.bWatered = Tile->IsWatered();
			TileData.GrowthDays = Tile->GetGrowthDays();
		}
	}

	if (!UGameplayStatics::SaveGameToSlot(Save, UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex))
	{
		ShowMessage(TEXT("Couldn't save the game."));
	}
}

void UFarmingComponent::LoadFarm()
{
	if (!Grid || !DayCycle || !UGameplayStatics::DoesSaveGameExist(UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex))
	{
		return;
	}

	const UFarmSaveGame* Save = Cast<UFarmSaveGame>(UGameplayStatics::LoadGameFromSlot(UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex));
	if (!Save)
	{
		ShowMessage(TEXT("Couldn't load the save. Press F9 to start a new game."));
		return;
	}

	DayCycle->RestoreClock(Save->Day, Save->TimeOfDay);
	Seeds = Save->Seeds;
	HarvestedCrops = Save->HarvestedCrops;
	SelectTool(Save->CurrentTool);

	// If the grid's size changed since saving, tiles beyond the saved ones stay as grass.
	const TArray<TObjectPtr<AFarmTile>>& Tiles = Grid->GetTiles();
	for (int32 Index = 0; Index < FMath::Min(Tiles.Num(), Save->Tiles.Num()); ++Index)
	{
		if (AFarmTile* Tile = Tiles[Index])
		{
			const FFarmTileSaveData& TileData = Save->Tiles[Index];
			Tile->RestoreState(TileData.State, TileData.bWatered, TileData.GrowthDays);
		}
	}

	ShowMessage(FString::Printf(TEXT("Welcome back. Day %d."), Save->Day));
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
	return DayCycle ? DayCycle->GetCurrentDay() : 0;
}

float UFarmingComponent::GetTimeOfDay() const
{
	return DayCycle ? DayCycle->GetTimeOfDay() : 0.f;
}

bool UFarmingComponent::HasMessage() const
{
	const UWorld* World = GetWorld();
	return !Message.IsEmpty() && World && World->GetTimeSeconds() < MessageEndTime;
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
