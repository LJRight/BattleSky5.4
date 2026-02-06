// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleSky : ModuleRules
{
	public BattleSky(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"UMG", 
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
		});
	}
}
