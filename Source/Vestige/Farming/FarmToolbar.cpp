// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "Farming/FarmToolbar.h"

#include "Content/FarmToolData.h"

void FFarmToolbar::SetTools(TArray<TObjectPtr<const UFarmToolData>> InTools)
{
	Tools = MoveTemp(InTools);
	CurrentSlot = 0;
}

bool FFarmToolbar::SelectSlot(int32 Slot)
{
	if (!Tools.IsValidIndex(Slot))
	{
		return false;
	}
	CurrentSlot = Slot;
	return true;
}

bool FFarmToolbar::SelectTool(FName ToolContentId)
{
	const int32 Slot = Tools.IndexOfByPredicate(
		[ToolContentId](const UFarmToolData* Tool)
		{
			return Tool && Tool->GetContentId() == ToolContentId;
		});
	return !ToolContentId.IsNone() && SelectSlot(Slot);
}

const UFarmToolData* FFarmToolbar::GetCurrentTool() const
{
	return Tools.IsValidIndex(CurrentSlot) ? Tools[CurrentSlot].Get() : nullptr;
}
