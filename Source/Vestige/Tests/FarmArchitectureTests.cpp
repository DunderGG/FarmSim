// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmArchitectureTests
{
	/**
	 * The source folders as architecture layers, lowest first, matching Architecture.md section 14. A file may include
	 * headers from its own folder and from folders earlier in this list, never from a later one. A new folder must be
	 * added here, in its layer, before files go in it.
	 */
	const TCHAR* const LayerOrder[] = {
		TEXT("Core"), TEXT("Content"), TEXT("Time"), TEXT("Farming"), TEXT("Save"), TEXT("Player"), TEXT("UI")};

	/** Folders outside the layers. Their files aren't checked, and any layer may include from them. */
	const TCHAR* const ExemptFolders[] = {
		TEXT("Tests"), // Tests may include anything.
		TEXT("Template"), // Epic's template code, which isn't maintained to the guide (CodingGuidelines.md, section 2.2).
	};

	/** The layer index of Folder, or INDEX_NONE if it isn't a layer. */
	int32 GetLayer(const FString& Folder)
	{
		for (int32 Layer = 0; Layer < UE_ARRAY_COUNT(LayerOrder); ++Layer)
		{
			if (Folder == LayerOrder[Layer])
			{
				return Layer;
			}
		}
		return INDEX_NONE;
	}

	bool IsExempt(const FString& Folder)
	{
		for (const TCHAR* Exempt : ExemptFolders)
		{
			if (Folder == Exempt)
			{
				return true;
			}
		}
		return false;
	}

	/** The path inside the quotes of an #include "..." line, or an empty string for any other line. */
	FString GetQuotedInclude(const FString& Line)
	{
		const FString Trimmed = Line.TrimStartAndEnd();
		if (!Trimmed.StartsWith(TEXT("#include \"")))
		{
			return FString();
		}
		const int32 Start = Trimmed.Find(TEXT("\"")) + 1;
		const int32 End = Trimmed.Find(TEXT("\""), ESearchCase::CaseSensitive, ESearchDir::FromStart, Start);
		return End > Start ? Trimmed.Mid(Start, End - Start) : FString();
	}

	TEST_CLASS(Architecture, "Vestige")
	{
		TEST_METHOD(IncludeDirection)
		{
			const FString SourceDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::GameSourceDir(), TEXT("Vestige")));
			if (!FPaths::DirectoryExists(SourceDir))
			{
				AddInfo(FString::Printf(TEXT("Skipped: there is no source folder at %s."), *SourceDir));
				return;
			}

			TArray<FString> Files;
			IFileManager::Get().FindFilesRecursive(Files, *SourceDir, TEXT("*.*"), true, false);
			TSet<FString> OurHeaderNames;
			for (const FString& File : Files)
			{
				if (FPaths::GetExtension(File) == TEXT("h"))
				{
					OurHeaderNames.Add(FPaths::GetCleanFilename(File));
				}
			}

			int32 FilesChecked = 0;
			for (const FString& File : Files)
			{
				const FString Extension = FPaths::GetExtension(File);
				if (Extension != TEXT("h") && Extension != TEXT("cpp"))
				{
					continue;
				}

				FString RelativePath = File;
				FPaths::MakePathRelativeTo(RelativePath, *(SourceDir + TEXT("/")));
				FString Folder;
				FString Rest;
				if (!RelativePath.Split(TEXT("/"), &Folder, &Rest) || IsExempt(Folder))
				{
					continue;
				}

				const int32 FileLayer = GetLayer(Folder);
				if (FileLayer == INDEX_NONE)
				{
					AddError(FString::Printf(TEXT("%s is in %s/, which isn't a layer. Add the folder to LayerOrder in this test and "
												  "to Architecture.md section 14."),
						*RelativePath, *Folder));
					continue;
				}

				++FilesChecked;
				TArray<FString> Lines;
				FFileHelper::LoadFileToStringArray(Lines, *File);
				for (int32 LineIndex = 0; LineIndex < Lines.Num(); ++LineIndex)
				{
					const FString Include = GetQuotedInclude(Lines[LineIndex]);
					FString IncludedFolder;
					FString IncludedFile;
					if (Include.IsEmpty() || Include.EndsWith(TEXT(".generated.h")))
					{
						continue;
					}
					if (!Include.Split(TEXT("/"), &IncludedFolder, &IncludedFile))
					{
						// A bare name is fine for engine headers ("CoreMinimal.h"), not for ours.
						if (OurHeaderNames.Contains(Include))
						{
							AddError(FString::Printf(TEXT("%s(%d) includes \"%s\" without its folder. Include our headers by their "
														  "path from the module root."),
								*RelativePath, LineIndex + 1, *Include));
						}
						continue;
					}

					// Engine headers have folders too ("Engine/World.h"); only paths that exist in our source are ours.
					const bool bOurs = FPaths::FileExists(FPaths::Combine(SourceDir, Include));
					const int32 IncludedLayer = GetLayer(IncludedFolder);
					if (bOurs && IncludedLayer > FileLayer)
					{
						AddError(FString::Printf(TEXT("%s(%d) includes %s from a higher layer. %s/ may only include %s/ and the "
													  "layers below it (Architecture.md section 14)."),
							*RelativePath, LineIndex + 1, *Include, *Folder, *Folder));
					}
				}
			}

			ASSERT_THAT(IsTrue(FilesChecked > 0));
		}
	};
}

#endif
