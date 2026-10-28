// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmCropData.h"

#include "Engine/StaticMesh.h"

const FPrimaryAssetType UFarmCropData::AssetType(TEXT("FarmCrop"));

UStaticMesh* UFarmCropData::GetGrowthStageMesh(int32 GrowthDays) const
{
	if (GrowthStageMeshes.IsEmpty())
	{
		return nullptr;
	}
	return GrowthStageMeshes[FMath::Clamp(GrowthDays, 0, GrowthStageMeshes.Num() - 1)].Get();
}
