// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PythonConsole : ModuleRules
{
#if WITH_FORWARDED_MODULE_RULES_CTOR
    public PythonConsole(ReadOnlyTargetRules Target) : base(Target)
#else
    public PythonConsole(TargetInfo Target)
#endif
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        string enableUnityBuild = System.Environment.GetEnvironmentVariable("UEP_ENABLE_UNITY_BUILD");
        bFasterWithoutUnity = string.IsNullOrEmpty(enableUnityBuild);

        PrivateIncludePaths.AddRange(
            new string[] {
                "PythonConsole/Private",
				// ... add other private include paths required here ...
			}
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject", // @todo Mac: for some reason it's needed to link in debug on Mac
				"Engine",
                "InputCore",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "TargetPlatform",
                "UnrealEnginePython"
            }
        );

    }
}
