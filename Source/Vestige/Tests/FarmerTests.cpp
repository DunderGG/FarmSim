// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "Player/FarmerCharacter.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmerTests
{
	TEST_CLASS(Farmer, "Vestige")
	{
		FActorTestSpawner Spawner;

		TEST_METHOD(ActionKeyHintsNameTheBoundKeys)
		{
			const AFarmerCharacter& Farmer = Spawner.SpawnActor<AFarmerCharacter>();

			const TArray<FFarmKeyHint> Hints = Farmer.GetActionKeyHints();

			ASSERT_THAT(IsTrue(Hints.Num() == 3));
			ASSERT_THAT(IsTrue(Hints[0].Key.ToString() == TEXT("E")));
			ASSERT_THAT(IsTrue(Hints[1].Key.ToString() == TEXT("N")));
			ASSERT_THAT(IsTrue(Hints[2].Key.ToString() == TEXT("F9")));
			for (const FFarmKeyHint& Hint : Hints)
			{
				ASSERT_THAT(IsFalse(Hint.Action.IsEmpty()));
			}
		}

		TEST_METHOD(OnlyAShortMoveInputIsATapToTurn)
		{
			constexpr float TapSeconds = 0.15f;

			ASSERT_THAT(IsTrue(AFarmerCharacter::IsTapToTurn(0.f, TapSeconds)));
			ASSERT_THAT(IsTrue(AFarmerCharacter::IsTapToTurn(0.14f, TapSeconds)));
			ASSERT_THAT(IsFalse(AFarmerCharacter::IsTapToTurn(0.15f, TapSeconds)));
			ASSERT_THAT(IsFalse(AFarmerCharacter::IsTapToTurn(2.f, TapSeconds)));
		}

		TEST_METHOD(ToolbarKeyHintsLeaveOutEmptySlots)
		{
			const AFarmerCharacter& Farmer = Spawner.SpawnActor<AFarmerCharacter>();

			// The C++ farmer has no toolbar; the Blueprint fills it.
			ASSERT_THAT(IsTrue(Farmer.GetToolbarKeyHints().IsEmpty()));
		}
	};
}

#endif
