// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "Content/FarmCropData.h"
#include "CQTest.h"
#include "Farming/FarmTileState.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmTileStateTests
{
	TEST_CLASS(TileState, "Vestige")
	{
		FActorTestSpawner Spawner;

		/** Tilled soil with a freshly planted crop. */
		FFarmTileState MakePlanted(const UFarmCropData& Crop)
		{
			FFarmTileState State;
			State.Till();
			State.Plant(&Crop);
			return State;
		}

		TEST_METHOD(OnlyGrassCanBeTilled)
		{
			FFarmTileState State;
			ASSERT_THAT(IsTrue(State.Till()));

			ASSERT_THAT(IsFalse(State.Till()));
			ASSERT_THAT(IsTrue(State.GetState() == EFarmTileState::Tilled));
		}

		TEST_METHOD(GrassCanNotBeWateredOrPlanted)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			FFarmTileState State;

			ASSERT_THAT(IsFalse(State.Water()));
			ASSERT_THAT(IsFalse(State.Plant(&Crop)));
			ASSERT_THAT(IsTrue(State.GetState() == EFarmTileState::Grass));
		}

		TEST_METHOD(APlantedTileCanNotBePlantedAgain)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			FFarmTileState State = MakePlanted(Crop);

			ASSERT_THAT(IsFalse(State.Plant(&Crop)));
		}

		TEST_METHOD(WateredSoilCanNotBeWateredAgainAndDriesOvernight)
		{
			FFarmTileState State;
			State.Till();
			ASSERT_THAT(IsTrue(State.Water()));

			ASSERT_THAT(IsFalse(State.Water()));
			State.AdvanceDay();
			ASSERT_THAT(IsFalse(State.IsWatered()));
		}

		TEST_METHOD(AnUnripeCropCanNotBeHarvested)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			FFarmTileState State = MakePlanted(Crop);

			ASSERT_THAT(IsFalse(State.Harvest()));
			ASSERT_THAT(IsTrue(State.GetCrop() == &Crop));
		}

		TEST_METHOD(ARipeCropStopsGrowing)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			FFarmTileState State = MakePlanted(Crop);
			for (int32 Day = 0; Day <= Crop.GetDaysToMature(); ++Day)
			{
				State.Water();
				State.AdvanceDay();
			}

			ASSERT_THAT(IsTrue(State.IsRipe()));
			ASSERT_THAT(IsTrue(State.GetGrowthDays() == Crop.GetDaysToMature()));
		}

		TEST_METHOD(RestoringClampsGrowthAndKeepsGrassDry)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			FFarmTileState Planted;
			Planted.Restore(EFarmTileState::Planted, false, Crop.GetDaysToMature() + 5, &Crop);
			FFarmTileState Grass;
			Grass.Restore(EFarmTileState::Grass, true, 3, nullptr);

			ASSERT_THAT(IsTrue(Planted.GetGrowthDays() == Crop.GetDaysToMature()));
			ASSERT_THAT(IsFalse(Grass.IsWatered()));
			ASSERT_THAT(IsTrue(Grass.GetGrowthDays() == 0));
		}
	};
}

#endif
