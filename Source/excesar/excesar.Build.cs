// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class excesar : ModuleRules
{
	public excesar(ReadOnlyTargetRules Target) : base(Target)
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
			"excesar",
			"excesar/Variant_Platforming",
			"excesar/Variant_Platforming/Animation",
			"excesar/Variant_Combat",
			"excesar/Variant_Combat/AI",
			"excesar/Variant_Combat/Animation",
			"excesar/Variant_Combat/Gameplay",
			"excesar/Variant_Combat/Interfaces",
			"excesar/Variant_Combat/UI",
			"excesar/Variant_SideScrolling",
			"excesar/Variant_SideScrolling/AI",
			"excesar/Variant_SideScrolling/Gameplay",
			"excesar/Variant_SideScrolling/Interfaces",
			"excesar/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
