// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "Player/FarmCharacterMovementComponent.h"
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

		TEST_METHOD(MovementInputRampsInFromThePress)
		{
			constexpr float RampSeconds = 0.15f;

			ASSERT_THAT(IsTrue(AFarmerCharacter::GetMoveInputScale(0.f, RampSeconds) == 0.f));
			ASSERT_THAT(IsTrue(FMath::IsNearlyEqual(AFarmerCharacter::GetMoveInputScale(RampSeconds * 0.5f, RampSeconds), 0.5f)));
			ASSERT_THAT(IsTrue(AFarmerCharacter::GetMoveInputScale(RampSeconds, RampSeconds) == 1.f));
			ASSERT_THAT(IsTrue(AFarmerCharacter::GetMoveInputScale(2.f, RampSeconds) == 1.f));
			ASSERT_THAT(IsTrue(AFarmerCharacter::GetMoveInputScale(TNumericLimits<float>::Max(), RampSeconds) == 1.f));
		}

		TEST_METHOD(WithoutARampTheInputIsFullAtOnce) { ASSERT_THAT(IsTrue(AFarmerCharacter::GetMoveInputScale(0.f, 0.f) == 1.f)); }

		TEST_METHOD(TheFarmerTurnsWithTheFarmMovementComponent)
		{
			const AFarmerCharacter& Farmer = Spawner.SpawnActor<AFarmerCharacter>();

			// The template's constructor sets its own rate, so this catches the farmer's rate being overwritten.
			const UFarmCharacterMovementComponent* Movement = Farmer.GetCharacterMovement<UFarmCharacterMovementComponent>();
			ASSERT_THAT(IsNotNull(Movement));
			ASSERT_THAT(IsTrue(Movement->RotationRate.Yaw == FarmMovement::TurnDegreesPerSecond));
		}

		TEST_METHOD(TheFarmerBlueprintTurnsWithTheFarmMovementComponent)
		{
			const UClass* FarmerClass = LoadClass<AFarmerCharacter>(
				nullptr, TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
			ASSERT_THAT(IsNotNull(FarmerClass));

			// The Blueprint's rate is tuning, so any rate that turns at all passes.
			const UFarmCharacterMovementComponent* Movement =
				FarmerClass->GetDefaultObject<AFarmerCharacter>()->GetCharacterMovement<UFarmCharacterMovementComponent>();
			ASSERT_THAT(IsNotNull(Movement));
			ASSERT_THAT(IsTrue(Movement->RotationRate.Yaw > 0.f));
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
