// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "Content/FarmToolData.h"

#include "Engine/StaticMesh.h"

const FPrimaryAssetType UFarmToolData::AssetType(TEXT("FarmTool"));

UStaticMesh* UFarmToolData::GetHeldMesh() const
{
	return HeldMesh.Get();
}
