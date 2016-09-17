// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PythonConsole : ModuleRules
{

    private const string pythonHome = "python27";

    protected string PythonHome
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", pythonHome));
        }
    }

    public PythonConsole(TargetInfo Target)
	{
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

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            PublicIncludePaths.Add(PythonHome);
            PublicAdditionalLibraries.Add(Path.Combine(PythonHome, "libs", pythonHome + ".lib"));
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
