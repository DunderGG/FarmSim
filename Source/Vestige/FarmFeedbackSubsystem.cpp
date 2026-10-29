// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmFeedbackSubsystem.h"

#include "Engine/World.h"

void UFarmFeedbackSubsystem::Notify(const UWorld* World, const FText& Message)
{
	if (UFarmFeedbackSubsystem* Feedback = UWorld::GetSubsystem<UFarmFeedbackSubsystem>(World))
	{
		Feedback->OnFeedback.Broadcast(Message);
	}
}
