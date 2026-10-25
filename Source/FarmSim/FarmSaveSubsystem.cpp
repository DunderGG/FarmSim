#include "FarmSaveSubsystem.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "FarmDayCycle.h"
#include "FarmGrid.h"
#include "FarmHUD.h"
#include "FarmingComponent.h"
#include "FarmSaveGame.h"
#include "FarmTile.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

bool UFarmSaveSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UFarmSaveSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// This runs before the actors' BeginPlay, so wait a tick for the grid's tiles and the player.
	InWorld.GetTimerManager().SetTimerForNextTick(this, &UFarmSaveSubsystem::BindAndLoad);
}

void UFarmSaveSubsystem::StartNewGame()
{
	UGameplayStatics::DeleteGameInSlot(UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
}

void UFarmSaveSubsystem::BindAndLoad()
{
	// The prototype has a single field, so just use the first grid in the level.
	TActorIterator<AFarmGrid> It(GetWorld());
	Grid = It ? *It : nullptr;
	DayCycle = AFarmDayCycle::Find(GetWorld());
	if (!Grid || !DayCycle)
	{
		return;
	}

	DayCycle->OnNewDayFinished.AddUObject(this, &UFarmSaveSubsystem::SaveFarm);
	LoadFarm();
}

void UFarmSaveSubsystem::SaveFarm(int32 NewDay)
{
	const UFarmingComponent* Farming = GetPlayerFarming();
	if (!Farming)
	{
		return;
	}

	UFarmSaveGame* Save = Cast<UFarmSaveGame>(UGameplayStatics::CreateSaveGameObject(UFarmSaveGame::StaticClass()));
	Save->Day = DayCycle->GetCurrentDay();
	Save->TimeOfDay = DayCycle->GetTimeOfDay();
	Save->Seeds = Farming->Seeds;
	Save->HarvestedCrops = Farming->HarvestedCrops;
	Save->CurrentTool = Farming->GetCurrentTool();
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

	const bool bSaved = UGameplayStatics::SaveGameToSlot(Save, UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex);
	if (AFarmHUD* Hud = GetPlayerHud())
	{
		Hud->AppendMessage(bSaved ? TEXT("Game saved.") : TEXT("Couldn't save the game."));
	}
}

void UFarmSaveSubsystem::LoadFarm()
{
	UFarmingComponent* Farming = GetPlayerFarming();
	if (!Farming || !UGameplayStatics::DoesSaveGameExist(UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex))
	{
		return;
	}

	const UFarmSaveGame* Save = Cast<UFarmSaveGame>(UGameplayStatics::LoadGameFromSlot(UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex));
	if (!Save)
	{
		if (AFarmHUD* Hud = GetPlayerHud())
		{
			Hud->ShowMessage(TEXT("Couldn't load the save. Press F9 to start a new game."));
		}
		return;
	}

	DayCycle->RestoreClock(Save->Day, Save->TimeOfDay);
	Farming->Seeds = Save->Seeds;
	Farming->HarvestedCrops = Save->HarvestedCrops;
	Farming->SelectTool(Save->CurrentTool);

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

	if (AFarmHUD* Hud = GetPlayerHud())
	{
		Hud->ShowMessage(FString::Printf(TEXT("Welcome back. Day %d."), Save->Day));
	}
}

UFarmingComponent* UFarmSaveSubsystem::GetPlayerFarming() const
{
	const APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return Pawn ? Pawn->FindComponentByClass<UFarmingComponent>() : nullptr;
}

AFarmHUD* UFarmSaveSubsystem::GetPlayerHud() const
{
	return AFarmHUD::GetFor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}
