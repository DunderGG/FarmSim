// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "Content/FarmCropData.h"
#include "Content/FarmItemData.h"
#include "Content/FarmSeedFragment.h"
#include "Content/FarmToolData.h"
#include "CQTest.h"
#include "Engine/AssetManager.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmItemDataTests
{
	const FPrimaryAssetId TurnipCropId(UFarmCropData::AssetType, TEXT("Base.Turnip"));
	const FPrimaryAssetId TurnipItemId(UFarmItemData::AssetType, TEXT("Base.Turnip"));
	const FPrimaryAssetId TurnipSeedsItemId(UFarmItemData::AssetType, TEXT("Base.TurnipSeeds"));

	TEST_CLASS(Item, "Vestige")
	{
		FActorTestSpawner Spawner;

		UFarmItemData& MakeItem()
		{
			UFarmItemData& Item = Spawner.SpawnObject<UFarmItemData>();
			Item.SetContentIdForTests(TEXT("Test.Item"));
			return Item;
		}

		UFarmSeedFragment& AddSeedFragment(UFarmItemData& Item, const FPrimaryAssetId& CropId)
		{
			UFarmSeedFragment& Seed = Spawner.SpawnObject<UFarmSeedFragment>();
			Seed.SetCropIdForTests(CropId);
			Item.AddFragmentForTests(&Seed);
			return Seed;
		}

		/** Loads a real content asset. Synchronous loading is fine in a test. */
		template <typename T> const T* LoadContent(const FPrimaryAssetId& Id)
		{
			return Cast<T>(UAssetManager::Get().GetPrimaryAssetPath(Id).TryLoad());
		}

		TEST_METHOD(FindFragmentReturnsTheFragmentOfThatClass)
		{
			UFarmItemData& Item = MakeItem();
			const UFarmSeedFragment& Seed = AddSeedFragment(Item, TurnipCropId);

			ASSERT_THAT(IsTrue(Item.FindFragment<UFarmSeedFragment>() == &Seed));
			ASSERT_THAT(IsTrue(Item.FindFragmentByClass(UFarmItemFragment::StaticClass()) == &Seed));
		}

		TEST_METHOD(FindFragmentReturnsNullWhenTheItemHasNone)
		{
			const UFarmItemData& Item = MakeItem();

			ASSERT_THAT(IsNull(Item.FindFragment<UFarmSeedFragment>()));
			ASSERT_THAT(IsNull(Item.FindFragmentByClass(nullptr)));
		}

		TEST_METHOD(TheTurnipSeedsPlantTheTurnip)
		{
			const UFarmItemData* Seeds = LoadContent<UFarmItemData>(TurnipSeedsItemId);
			ASSERT_THAT(IsNotNull(Seeds));

			const UFarmSeedFragment* Seed = Seeds->FindFragment<UFarmSeedFragment>();
			ASSERT_THAT(IsNotNull(Seed));
			ASSERT_THAT(IsTrue(Seed->GetCropId() == TurnipCropId));
		}

		TEST_METHOD(TheTurnipCropYieldsTheTurnipItem)
		{
			const UFarmCropData* Crop = LoadContent<UFarmCropData>(TurnipCropId);
			ASSERT_THAT(IsNotNull(Crop));
			ASSERT_THAT(IsTrue(Crop->GetHarvestItemId() == TurnipItemId));

			const UFarmItemData* Turnip = LoadContent<UFarmItemData>(TurnipItemId);
			ASSERT_THAT(IsNotNull(Turnip));
			ASSERT_THAT(IsNull(Turnip->FindFragment<UFarmSeedFragment>()));
		}

#if WITH_EDITOR
		bool PassesValidation(const UObject& Object)
		{
			FDataValidationContext Context;
			const EDataValidationResult Result = Object.IsDataValid(Context);
			return Result != EDataValidationResult::Invalid && Context.GetNumErrors() == 0;
		}

		TEST_METHOD(AnItemWithAKnownSeedCropIsValid)
		{
			UFarmItemData& Item = MakeItem();
			AddSeedFragment(Item, TurnipCropId);

			ASSERT_THAT(IsTrue(PassesValidation(Item)));
		}

		TEST_METHOD(AStackSizeBelowOneIsInvalid)
		{
			UFarmItemData& Item = MakeItem();
			Item.SetMaxStackSizeForTests(0);

			ASSERT_THAT(IsFalse(PassesValidation(Item)));
		}

		TEST_METHOD(AnEmptyFragmentEntryIsInvalid)
		{
			UFarmItemData& Item = MakeItem();
			Item.AddFragmentForTests(nullptr);

			ASSERT_THAT(IsFalse(PassesValidation(Item)));
		}

		TEST_METHOD(TwoFragmentsOfTheSameClassAreInvalid)
		{
			UFarmItemData& Item = MakeItem();
			AddSeedFragment(Item, TurnipCropId);
			AddSeedFragment(Item, TurnipCropId);

			ASSERT_THAT(IsFalse(PassesValidation(Item)));
		}

		TEST_METHOD(ASeedWithoutACropIsInvalid)
		{
			UFarmItemData& Item = MakeItem();
			AddSeedFragment(Item, FPrimaryAssetId());

			ASSERT_THAT(IsFalse(PassesValidation(Item)));
		}

		TEST_METHOD(ASeedNamingAnotherContentTypeIsInvalid)
		{
			UFarmItemData& Item = MakeItem();
			AddSeedFragment(Item, FPrimaryAssetId(UFarmToolData::AssetType, TEXT("Base.Hoe")));

			ASSERT_THAT(IsFalse(PassesValidation(Item)));
		}

		TEST_METHOD(ASeedNamingAnUnknownCropIsInvalid)
		{
			UFarmItemData& Item = MakeItem();
			AddSeedFragment(Item, FPrimaryAssetId(UFarmCropData::AssetType, TEXT("Test.NoSuchCrop")));

			ASSERT_THAT(IsFalse(PassesValidation(Item)));
		}

		TEST_METHOD(ACropWithoutAHarvestItemIsInvalid)
		{
			UFarmCropData& Crop = Spawner.SpawnObject<UFarmCropData>();
			Crop.SetContentIdForTests(TEXT("Test.Crop"));
			ASSERT_THAT(IsFalse(PassesValidation(Crop)));

			Crop.SetHarvestItemIdForTests(TurnipItemId);
			ASSERT_THAT(IsTrue(PassesValidation(Crop)));
		}

		TEST_METHOD(TheBaseItemsAndTheTurnipCropAreValid)
		{
			for (const FPrimaryAssetId& Id : {TurnipItemId, TurnipSeedsItemId})
			{
				const UFarmItemData* Item = LoadContent<UFarmItemData>(Id);
				ASSERT_THAT(IsNotNull(Item));
				ASSERT_THAT(IsTrue(PassesValidation(*Item)));
			}
			const UFarmCropData* Crop = LoadContent<UFarmCropData>(TurnipCropId);
			ASSERT_THAT(IsNotNull(Crop));
			ASSERT_THAT(IsTrue(PassesValidation(*Crop)));
		}
#endif
	};
}

#endif
