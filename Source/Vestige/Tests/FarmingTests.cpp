// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "Content/FarmCropData.h"
#include "Player/FarmerCharacter.h"
#include "Farming/FarmGrid.h"
#include "Farming/FarmingComponent.h"
#include "Farming/FarmFarmingSaveData.h"
#include "Farming/FarmTile.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmingTests
{
	TEST_CLASS(Farming, "Vestige")
	{
		FActorTestSpawner Spawner;
		AFarmGrid* Grid = nullptr;
		AFarmerCharacter* Farmer = nullptr;
		UFarmingComponent* FarmingComponent = nullptr;

		/**
		 * A default grid and the farmer Blueprint the game uses, standing in front of tile (3, 0). The Blueprint is used
		 * because it holds the real toolbar and seed crop, which load from the base content at BeginPlay.
		 */
		BEFORE_EACH()
		{
			Grid = &Spawner.SpawnActor<AFarmGrid>();
			Grid->DispatchBeginPlay();

			UClass* FarmerClass = LoadClass<AFarmerCharacter>(
				nullptr, TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
			ASSERT_THAT(IsNotNull(FarmerClass));
			Farmer = &Spawner.SpawnActor<AFarmerCharacter>(FActorSpawnParameters(), FarmerClass);
			Farmer->DispatchBeginPlay();
			Farmer->SetActorLocationAndRotation(FVector(250.f, 50.f, 0.f), FRotator::ZeroRotator);
			FarmingComponent = Farmer->FindComponentByClass<UFarmingComponent>();
			ASSERT_THAT(IsNotNull(FarmingComponent));
		}

		/** Puts this tool in hand and sets the seed count, through the same restore loading uses. */
		void Hold(FName ToolId, int32 Seeds)
		{
			FFarmFarmingSaveData Data;
			Data.Seeds = Seeds;
			Data.CurrentToolId = ToolId;
			ASSERT_THAT(IsTrue(FarmingComponent->RestoreFromSaveData(Data)));
		}

		AFarmTile* TargetTile() const { return Grid->GetTileAt(FIntPoint(3, 0)); }

		TEST_METHOD(PlantingUsesASeed)
		{
			TargetTile()->Till();
			Hold(TEXT("Base.Seeds"), 5);

			FarmingComponent->UseTool();

			ASSERT_THAT(IsTrue(TargetTile()->GetState() == EFarmTileState::Planted));
			ASSERT_THAT(IsTrue(FarmingComponent->GetSeeds() == 4));
		}

		TEST_METHOD(WithoutSeedsNothingIsPlanted)
		{
			TargetTile()->Till();
			Hold(TEXT("Base.Seeds"), 0);

			FarmingComponent->UseTool();

			ASSERT_THAT(IsTrue(TargetTile()->GetState() == EFarmTileState::Tilled));
			ASSERT_THAT(IsTrue(FarmingComponent->GetSeeds() == 0));
		}

		TEST_METHOD(TheHoeTillsTheTileInFront)
		{
			Hold(TEXT("Base.Hoe"), 0);

			FarmingComponent->UseTool();

			ASSERT_THAT(IsTrue(TargetTile()->GetState() == EFarmTileState::Tilled));
		}

		TEST_METHOD(HarvestingAddsTheCropsYield)
		{
			TargetTile()->Till();
			Hold(TEXT("Base.Seeds"), 1);
			FarmingComponent->UseTool();
			const UFarmCropData* Crop = TargetTile()->GetCrop();
			ASSERT_THAT(IsNotNull(Crop));
			for (int32 Day = 0; Day < Crop->GetDaysToMature(); ++Day)
			{
				TargetTile()->Water();
				TargetTile()->AdvanceDay();
			}
			Hold(TEXT("Base.Hand"), 0);

			FarmingComponent->UseTool();

			ASSERT_THAT(IsTrue(FarmingComponent->GetHarvestedCrops() == Crop->GetYield()));
			ASSERT_THAT(IsTrue(TargetTile()->GetState() == EFarmTileState::Tilled));
		}

		TEST_METHOD(UsingAToolAwayFromTheFieldChangesNothing)
		{
			Hold(TEXT("Base.Seeds"), 3);
			Farmer->SetActorLocation(FVector(-500.f, -500.f, 0.f));

			FarmingComponent->UseTool();

			ASSERT_THAT(IsTrue(FarmingComponent->GetSeeds() == 3));
		}

		TEST_METHOD(AnUnknownSavedToolPutsTheFirstSlotInHand)
		{
			FFarmFarmingSaveData Data;
			Data.CurrentToolId = TEXT("SomeMod.Scythe");

			ASSERT_THAT(IsFalse(FarmingComponent->RestoreFromSaveData(Data)));
			ASSERT_THAT(IsTrue(FarmingComponent->GetToolbar().GetCurrentSlot() == 0));
		}
	};
}

#endif
