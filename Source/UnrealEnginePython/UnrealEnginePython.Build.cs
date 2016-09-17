// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class UnrealEnginePython : ModuleRules
{

    private const string PythonVersion = "python27";

    protected string PythonHome
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", PythonVersion));
        }
    }

    public UnrealEnginePython(TargetInfo Target)
    {

        System.Console.WriteLine("Using Python at: " + PythonHome);

        PublicIncludePaths.AddRange(
            new string[] {
                "UnrealEnginePython/Public",
				// ... add public include paths required here ...
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "UnrealEnginePython/Private",
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "MovieScene",
                "LevelSequence",
				// ... add private dependencies that you statically link with here ...
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );


        if (UEBuildConfiguration.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]{
                "UnrealEd"
            });
        }

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            PublicIncludePaths.Add(PythonHome);
            PublicAdditionalLibraries.Add(Path.Combine(PythonHome, "libs", PythonVersion + ".lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            string mac_python = "/Library/Frameworks/Python.framework/Versions/3.5/";
            PublicIncludePaths.Add(Path.Combine(mac_python, "include"));
            PublicAdditionalLibraries.Add(Path.Combine(mac_python, "lib", "libpython3.5m.dylib"));
            Definitions.Add(string.Format("UNREAL_ENGINE_PYTHON_ON_MAC=1"));
        }

    }
}
