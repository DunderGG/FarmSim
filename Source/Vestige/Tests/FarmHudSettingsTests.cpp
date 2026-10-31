// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "UI/FarmHUD.h"
#include "UI/FarmHudSettings.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmHudSettingsTests
{
	/** The settings are tuning, so these check the limits and where the HUD reads them, never their values. */
	TEST_CLASS(HudSettings, "Vestige")
	{
		FActorTestSpawner Spawner;

		TEST_METHOD(TheMessageCapStaysInItsRange)
		{
			ASSERT_THAT(IsTrue(UFarmHudSettings::ClampMaxMessages(0) == FarmHudLimits::FewestMessages));
			ASSERT_THAT(IsTrue(UFarmHudSettings::ClampMaxMessages(5) == 5));
			ASSERT_THAT(IsTrue(UFarmHudSettings::ClampMaxMessages(FarmHudLimits::MostMessages + 1) == FarmHudLimits::MostMessages));
		}

		TEST_METHOD(TheProjectSettingsAreInRange)
		{
			const UFarmHudSettings& Settings = UFarmHudSettings::Get();

			ASSERT_THAT(IsTrue(Settings.GetMaxMessages() >= FarmHudLimits::FewestMessages));
			ASSERT_THAT(IsTrue(Settings.GetMaxMessages() <= FarmHudLimits::MostMessages));
			ASSERT_THAT(IsTrue(Settings.GetMessageSeconds() >= FarmHudLimits::ShortestMessageSeconds));
		}

		TEST_METHOD(TheHudTakesItsMessageCapFromTheSettings)
		{
			AFarmHUD& Hud = Spawner.SpawnActor<AFarmHUD>();
			Hud.DispatchBeginPlay();

			ASSERT_THAT(IsTrue(Hud.GetMaxMessages() == UFarmHudSettings::Get().GetMaxMessages()));
		}
	};
}

#endif
