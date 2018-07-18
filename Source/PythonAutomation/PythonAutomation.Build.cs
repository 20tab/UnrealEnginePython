// Copyright 1998-2018 20Tab S.r.l All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PythonAutomation : ModuleRules
{
#if WITH_FORWARDED_MODULE_RULES_CTOR
    public PythonAutomation(ReadOnlyTargetRules Target) : base(Target)
#else
    public PythonAutomation(TargetInfo Target)
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
                "UnrealEd",
                "UnrealEnginePython"
            }
        );

    }
}
