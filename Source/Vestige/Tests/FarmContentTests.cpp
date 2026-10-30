// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "Content/FarmAssetManager.h"
#include "Content/FarmCropData.h"
#include "Content/FarmToolData.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmContentTests
{
	TEST_CLASS(Content, "Vestige")
	{
		FActorTestSpawner Spawner;

		TEST_METHOD(TheProjectAssetManagerIsInUse)
		{
			// Set by AssetManagerClassName in Config/DefaultEngine.ini. Without it, duplicate content IDs go unreported.
			ASSERT_THAT(IsTrue(UAssetManager::Get().IsA<UFarmAssetManager>()));
		}

		TEST_METHOD(WellFormedContentIdsAreAccepted)
		{
			ASSERT_THAT(IsTrue(UFarmContentData::IsWellFormedContentId(TEXT("Base.Turnip"))));
			ASSERT_THAT(IsTrue(UFarmContentData::IsWellFormedContentId(TEXT("Base.WateringCan"))));
			ASSERT_THAT(IsTrue(UFarmContentData::IsWellFormedContentId(TEXT("MyMod2.BlueCorn3"))));
		}

		TEST_METHOD(BadlyFormedContentIdsAreRejected)
		{
			const TCHAR* const BadIds[] = {TEXT(""), TEXT("Turnip"), TEXT("Base."), TEXT(".Turnip"), TEXT("Base.Turnip.Big"),
				TEXT("base.Turnip"), TEXT("Base.turnip"), TEXT("Base.Watering Can"), TEXT("Base.Watering_Can"), TEXT("Base.1Turnip")};
			for (const TCHAR* BadId : BadIds)
			{
				ASSERT_THAT(IsFalse(UFarmContentData::IsWellFormedContentId(BadId)));
			}
		}

		TEST_METHOD(PrimaryAssetIdIsBuiltFromTheTypeAndContentId)
		{
			UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			ASSERT_THAT(IsFalse(Crop.GetPrimaryAssetId().IsValid()));

			Crop.SetContentIdForTests(TEXT("Base.Turnip"));

			ASSERT_THAT(IsTrue(Crop.GetPrimaryAssetId() == FPrimaryAssetId(UFarmCropData::AssetType, TEXT("Base.Turnip"))));
		}

		TEST_METHOD(DuplicateContentIdsAreFoundWithEveryPackage)
		{
			const FPrimaryAssetId Turnip(UFarmCropData::AssetType, TEXT("Base.Turnip"));
			const FPrimaryAssetId Hoe(UFarmToolData::AssetType, TEXT("Base.Hoe"));
			const TPair<FPrimaryAssetId, FName> Assets[] = {{Turnip, TEXT("/Game/Farm/Crops/DA_Crop_Turnip")},
				{Hoe, TEXT("/Game/Farm/Tools/DA_Tool_Hoe")}, {Turnip, TEXT("/Game/Farm/Crops/DA_Crop_TurnipCopy")}};

			const TArray<FFarmDuplicateContentId> Duplicates = UFarmAssetManager::FindDuplicateContentIds(Assets);

			ASSERT_THAT(IsTrue(Duplicates.Num() == 1));
			ASSERT_THAT(IsTrue(Duplicates[0].Id == Turnip));
			ASSERT_THAT(IsTrue(Duplicates[0].Packages.Num() == 2));
		}

		TEST_METHOD(TheSameNameInTwoContentTypesIsNotADuplicate)
		{
			const TPair<FPrimaryAssetId, FName> Assets[] = {
				{FPrimaryAssetId(UFarmCropData::AssetType, TEXT("Base.Seeds")), TEXT("/Game/A")},
				{FPrimaryAssetId(UFarmToolData::AssetType, TEXT("Base.Seeds")), TEXT("/Game/B")}};

			ASSERT_THAT(IsTrue(UFarmAssetManager::FindDuplicateContentIds(Assets).IsEmpty()));
		}
	};
}

#endif
