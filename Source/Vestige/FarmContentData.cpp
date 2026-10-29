// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmContentData.h"

#include "Engine/AssetManager.h"
#include "Vestige.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "FarmContentData"

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

bool UFarmContentData::IsWellFormedContentId(FName Id)
{
	FString Namespace;
	FString Name;
	if (Id.IsNone() || !Id.ToString().Split(TEXT("."), &Namespace, &Name))
	{
		return false;
	}

	auto IsPascalCaseWord = [](const FString& Word)
	{
		if (Word.IsEmpty() || !FChar::IsUpper(Word[0]))
		{
			return false;
		}
		for (const TCHAR Character : Word)
		{
			const bool bAsciiLetterOrDigit = FChar::IsAlnum(Character) && Character < 128;
			if (!bAsciiLetterOrDigit)
			{
				return false;
			}
		}
		return true;
	};
	return IsPascalCaseWord(Namespace) && IsPascalCaseWord(Name);
}

#if WITH_EDITOR
EDataValidationResult UFarmContentData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	if (!IsWellFormedContentId(ContentId))
	{
		Context.AddError(FText::Format(
			LOCTEXT("BadContentId",
				"Content Id \"{0}\" isn't a content ID. Use a namespace and a name joined by one dot, each starting with a capital letter, "
				"letters and digits only: Base.Turnip."),
			FText::FromName(ContentId)));
		Result = EDataValidationResult::Invalid;
	}
	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
