// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TowerDeffense_FPSEditorTarget : TargetRules
{
	public TowerDeffense_FPSEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("TowerDeffense_FPS");
        ExtraModuleNames.Add("GitEditor");//’Ç‰Á
    }
}
