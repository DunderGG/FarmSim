// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "FarmCropData.h"
#include "FarmTile.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmTileTests
{
	TEST_CLASS(Tile, "Vestige")
	{
		FActorTestSpawner Spawner;

		/** Waters the tile and starts a new day, Days times over. */
		static void WaterForDays(AFarmTile& FarmTile, int32 Days)
		{
			for (int32 Day = 0; Day < Days; ++Day)
			{
				FarmTile.Water();
				FarmTile.AdvanceDay();
			}
		}

		TEST_METHOD(PlantingNeedsACrop)
		{
			AFarmTile& FarmTile = Spawner.SpawnActor<AFarmTile>();
			ASSERT_THAT(IsTrue(FarmTile.Till()));

			ASSERT_THAT(IsFalse(FarmTile.Plant(nullptr)));
			ASSERT_THAT(IsTrue(FarmTile.GetState() == EFarmTileState::Tilled));
		}

		TEST_METHOD(WateredCropRipensAfterItsDaysToMature)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			AFarmTile& FarmTile = Spawner.SpawnActor<AFarmTile>();
			ASSERT_THAT(IsTrue(FarmTile.Till()));
			ASSERT_THAT(IsTrue(FarmTile.Plant(&Crop)));

			WaterForDays(FarmTile, Crop.GetDaysToMature() - 1);
			ASSERT_THAT(IsFalse(FarmTile.IsRipe()));

			WaterForDays(FarmTile, 1);
			ASSERT_THAT(IsTrue(FarmTile.IsRipe()));
		}

		TEST_METHOD(DryCropDoesNotGrow)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			AFarmTile& FarmTile = Spawner.SpawnActor<AFarmTile>();
			ASSERT_THAT(IsTrue(FarmTile.Till()));
			ASSERT_THAT(IsTrue(FarmTile.Plant(&Crop)));

			FarmTile.AdvanceDay();

			ASSERT_THAT(IsTrue(FarmTile.GetGrowthDays() == 0));
		}

		TEST_METHOD(HarvestingClearsTheCrop)
		{
			const UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			AFarmTile& FarmTile = Spawner.SpawnActor<AFarmTile>();
			ASSERT_THAT(IsTrue(FarmTile.Till()));
			ASSERT_THAT(IsTrue(FarmTile.Plant(&Crop)));
			WaterForDays(FarmTile, Crop.GetDaysToMature());

			ASSERT_THAT(IsTrue(FarmTile.Harvest()));
			ASSERT_THAT(IsTrue(FarmTile.GetState() == EFarmTileState::Tilled));
			ASSERT_THAT(IsTrue(FarmTile.GetCrop() == nullptr));
		}

		TEST_METHOD(RestoredPlantedTileWithoutACropIsTilled)
		{
			AFarmTile& FarmTile = Spawner.SpawnActor<AFarmTile>();

			FarmTile.RestoreState(EFarmTileState::Planted, true, 1, nullptr);

			ASSERT_THAT(IsTrue(FarmTile.GetState() == EFarmTileState::Tilled));
			ASSERT_THAT(IsTrue(FarmTile.GetGrowthDays() == 0));
		}
	};
}

#endif
