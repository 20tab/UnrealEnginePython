// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class UnrealEnginePython : ModuleRules
{

    private string pythonHome = "python35";
    //private string pythonHome = "python27";

    protected string PythonHome
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", pythonHome));
        }
    }

    public UnrealEnginePython(TargetInfo Target)
    {


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
                "UnrealEd",
            });
        }

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            System.Console.WriteLine("Using Python at: " + PythonHome);
            PublicIncludePaths.Add(PythonHome);
            PublicAdditionalLibraries.Add(Path.Combine(PythonHome, "libs", string.Format("{0}.lib", pythonHome)));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            if (pythonHome == "python35")
            {
                string mac_python = "/Library/Frameworks/Python.framework/Versions/3.5/";
                PublicIncludePaths.Add(Path.Combine(mac_python, "include"));
                PublicAdditionalLibraries.Add(Path.Combine(mac_python, "lib", "libpython3.5m.dylib"));
                Definitions.Add(string.Format("UNREAL_ENGINE_PYTHON_ON_MAC=3"));
            }
            else if (pythonHome == "python27") {
                string mac_python = "/Library/Frameworks/Python.framework/Versions/2.7/";
                PublicIncludePaths.Add(Path.Combine(mac_python, "include"));
                PublicAdditionalLibraries.Add(Path.Combine(mac_python, "lib", "libpython2.7.dylib"));
                Definitions.Add(string.Format("UNREAL_ENGINE_PYTHON_ON_MAC=2"));
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            if (pythonHome == "python35")
            {
                PublicIncludePaths.Add("/usr/include/python3.5m");
                PublicAdditionalLibraries.Add("/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/libpython3.5.so");
            }
            else if (pythonHome == "python27") {
                PublicIncludePaths.Add("/usr/include/python2.7");
                PublicAdditionalLibraries.Add("/usr/lib/python2.7/config-x86_64-linux-gnu/libpython2.7.so");
            }
            Definitions.Add(string.Format("UNREAL_ENGINE_PYTHON_ON_LINUX"));
        }

    }
}
