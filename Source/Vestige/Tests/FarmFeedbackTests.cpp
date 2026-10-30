// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/ActorTestSpawner.h"
#include "CQTest.h"
#include "Engine/World.h"
#include "Core/FarmFeedbackSubsystem.h"

#define LOCTEXT_NAMESPACE "FarmFeedbackTests"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmFeedbackTests
{
	TEST_CLASS(Feedback, "Vestige")
	{
		FActorTestSpawner Spawner;

		TEST_METHOD(MessagesReachEveryListener)
		{
			UFarmFeedbackSubsystem* Feedback = Spawner.GetWorld().GetSubsystem<UFarmFeedbackSubsystem>();
			ASSERT_THAT(IsNotNull(Feedback));
			TArray<FText> Received;
			Feedback->OnFeedback.AddLambda(
				[&Received](const FText& Message)
				{
					Received.Add(Message);
				});

			UFarmFeedbackSubsystem::Notify(&Spawner.GetWorld(), LOCTEXT("Tilled", "Tilled the soil."));

			ASSERT_THAT(IsTrue(Received.Num() == 1));
			ASSERT_THAT(IsTrue(Received[0].EqualTo(LOCTEXT("Tilled", "Tilled the soil."))));
		}
	};
}

#undef LOCTEXT_NAMESPACE

#endif
