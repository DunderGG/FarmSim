// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FarmSim : ModuleRules
{
	public FarmSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"FarmSim",
			"FarmSim/Variant_Platforming",
			"FarmSim/Variant_Platforming/Animation",
			"FarmSim/Variant_Combat",
			"FarmSim/Variant_Combat/AI",
			"FarmSim/Variant_Combat/Animation",
			"FarmSim/Variant_Combat/Gameplay",
			"FarmSim/Variant_Combat/Interfaces",
			"FarmSim/Variant_Combat/UI",
			"FarmSim/Variant_SideScrolling",
			"FarmSim/Variant_SideScrolling/AI",
			"FarmSim/Variant_SideScrolling/Gameplay",
			"FarmSim/Variant_SideScrolling/Interfaces",
			"FarmSim/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
