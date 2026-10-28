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
			"GameplayTags",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// CQTest is a developer module, so it is only available to builds with developer tools (never shipping).
		if (Target.bBuildDeveloperTools)
		{
			PrivateDependencyModuleNames.Add("CQTest");
		}

		PublicIncludePaths.AddRange(new string[] {
			"Vestige"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
