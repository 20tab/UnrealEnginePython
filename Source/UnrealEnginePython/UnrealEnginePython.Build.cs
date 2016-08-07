// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealEnginePython : ModuleRules
{

    private const string pythonHome = "C:/Users/Roberto/Documents/Unreal Projects/PyTest30/Plugins/UnrealEnginePython/python-3.5.2-embed-amd64";

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
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );




        PublicIncludePaths.Add(pythonHome);
        PublicLibraryPaths.Add(pythonHome + "/libs");
        PublicAdditionalLibraries.Add("python35.lib");

        RuntimeDependencies.Add(new RuntimeDependency(pythonHome + "/python35.dll"));
        RuntimeDependencies.Add(new RuntimeDependency(pythonHome + "/python35.zip"));

    }
}
