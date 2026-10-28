// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FarmContentData.generated.h"

/**
 * Base class for content definitions (crops, tools) found through the Asset Manager.
 * Owns what all content shares: the stable ContentId that builds the primary asset ID, and the display name.
 */
UCLASS(Abstract)
class UFarmContentData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Asset bundle holding a content asset's meshes. Loading content with it loads them too. */
	static const FName VisualsBundle;

	/**
	 * Loads the content asset with this ID and its Visuals bundle, waiting until both are in memory.
	 * Only for play start or loading screens. Returns nullptr, with a warning, if no such asset exists.
	 */
	static const UFarmContentData* LoadWithVisuals(const FPrimaryAssetId& ContentAssetId);

	/** Built from the content type and ContentId, so renaming or moving the asset keeps the ID. Invalid while ContentId is empty. */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	FName GetContentId() const { return ContentId; }
	const FText& GetDisplayName() const { return DisplayName; }

protected:
	/** The primary asset type of this kind of content, as registered in Config/DefaultGame.ini. */
	virtual FPrimaryAssetType GetContentType() const PURE_VIRTUAL(UFarmContentData::GetContentType, return FPrimaryAssetType(););

private:
	/** Stable ID of the form Namespace.Name (Base.Turnip). Saves and lookups use it; never change it once shipped. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming", meta = (AllowPrivateAccess = "true"))
	FName ContentId;

	/** Name shown to the player. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;
};
