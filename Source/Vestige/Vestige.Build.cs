// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Vestige : ModuleRules
{
	public Vestige(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags"
		});

		// UI modules are private: no other module uses our widgets, so our public headers don't need to expose them.
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate",
			"SlateCore",
			"UMG"
		});

		// CQTest is a developer module, so it is only available to builds with developer tools (never shipping).
		if (Target.bBuildDeveloperTools)
		{
			PrivateDependencyModuleNames.Add("CQTest");
		}

		PublicIncludePaths.AddRange(new string[] {
			"Vestige"
		});
	}
}
