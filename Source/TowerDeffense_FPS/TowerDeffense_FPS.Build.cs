// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TowerDeffense_FPS : ModuleRules
{
	public TowerDeffense_FPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore","AIModule","GameplayTasks","NavigationSystem","Niagara"
        });
	}
}
