// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FarmSaveSubsystem.generated.h"

class AFarmDayCycle;
class AFarmGrid;
class AFarmHUD;
class UFarmingComponent;
class UFarmSaveGame;

/**
 * Saves the farm every time a new day starts and loads it when play begins.
 * A world subsystem because everything it saves lives in the level, and a fresh one comes with every level load,
 * so starting a new game is just deleting the save and reloading the level.
 */
UCLASS()
class UFarmSaveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** The ContentId of the tool in hand in a save. Saves made before tools were data assets name a base tool through the old enum. */
	static FName GetSavedToolId(const UFarmSaveGame& Save);

	/** Deletes the save and reloads the level, starting the farm from day 1. */
	void StartNewGame();

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:
	/** Runs one tick after play begins, once every actor's BeginPlay has run and the grid has spawned its tiles. */
	void BindAndLoad();

	void SaveFarm(int32 NewDay);
	void LoadFarm();

	UFarmingComponent* GetPlayerFarming() const;
	AFarmHUD* GetPlayerHud() const;

	UPROPERTY()
	TObjectPtr<AFarmDayCycle> DayCycle;

	UPROPERTY()
	TObjectPtr<AFarmGrid> Grid;
};
