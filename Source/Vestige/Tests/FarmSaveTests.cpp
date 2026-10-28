// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "FarmSaveGame.h"
#include "FarmSaveSubsystem.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmSaveTests
{
	TEST_CLASS(Save, "Vestige")
	{
		FActorTestSpawner Spawner;

		TEST_METHOD(SavedToolIdIsUsedAsItIs)
		{
			UFarmSaveGame& SaveGame = Spawner.SpawnObject<UFarmSaveGame>();
			SaveGame.CurrentToolId = TEXT("Base.WateringCan");
			SaveGame.CurrentTool = EFarmTool::Hoe;

			ASSERT_THAT(IsTrue(UFarmSaveSubsystem::GetSavedToolId(SaveGame) == TEXT("Base.WateringCan")));
		}

		TEST_METHOD(OldSaveToolMapsToTheBaseToolId)
		{
			UFarmSaveGame& SaveGame = Spawner.SpawnObject<UFarmSaveGame>();

			SaveGame.CurrentTool = EFarmTool::Hoe;
			ASSERT_THAT(IsTrue(UFarmSaveSubsystem::GetSavedToolId(SaveGame) == TEXT("Base.Hoe")));
			SaveGame.CurrentTool = EFarmTool::Seeds;
			ASSERT_THAT(IsTrue(UFarmSaveSubsystem::GetSavedToolId(SaveGame) == TEXT("Base.Seeds")));
			SaveGame.CurrentTool = EFarmTool::WateringCan;
			ASSERT_THAT(IsTrue(UFarmSaveSubsystem::GetSavedToolId(SaveGame) == TEXT("Base.WateringCan")));
			SaveGame.CurrentTool = EFarmTool::Hand;
			ASSERT_THAT(IsTrue(UFarmSaveSubsystem::GetSavedToolId(SaveGame) == TEXT("Base.Hand")));
		}
	};
}

#endif
