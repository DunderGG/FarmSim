// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmContentData.h"

#include "Engine/AssetManager.h"
#include "Vestige.h"

const FName UFarmContentData::VisualsBundle(TEXT("Visuals"));

const UFarmContentData* UFarmContentData::LoadWithVisuals(const FPrimaryAssetId& ContentAssetId)
{
	UAssetManager* AssetManager = UAssetManager::GetIfInitialized();
	if (!AssetManager || !ContentAssetId.IsValid())
	{
		return nullptr;
	}

	if (const TSharedPtr<FStreamableHandle> Handle = AssetManager->LoadPrimaryAsset(ContentAssetId, {VisualsBundle}))
	{
		Handle->WaitUntilComplete();
	}
	const UFarmContentData* Content = AssetManager->GetPrimaryAssetObject<UFarmContentData>(ContentAssetId);
	if (!Content)
	{
		UE_LOG(LogVestige, Warning, TEXT("No content asset has the ID %s. Check the asset's Content Id and folder."),
			*ContentAssetId.ToString());
	}
	return Content;
}

FPrimaryAssetId UFarmContentData::GetPrimaryAssetId() const
{
	if (ContentId.IsNone())
	{
		return FPrimaryAssetId();
	}
	return FPrimaryAssetId(GetContentType(), ContentId);
}
