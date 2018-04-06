// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class PythonEditor : ModuleRules
    {
#if WITH_FORWARDED_MODULE_RULES_CTOR
        public PythonEditor(ReadOnlyTargetRules Target) : base(Target)
#else
        public PythonEditor(TargetInfo Target)
#endif
        {
            PCHUsage = PCHUsageMode.NoSharedPCHs;
            
            PrivateIncludePaths.AddRange(
                new string[] {
                    "PythonEditor/Private",
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "SlateCore",
                    "Slate",
                    "AssetTools",
                    "UnrealEd",
                    "EditorStyle",
                    "PropertyEditor",
                    "Kismet",  // for FWorkflowCentricApplication
					"InputCore",
                    "DirectoryWatcher",
                    "LevelEditor",
                    "Projects",
                    "UnrealEnginePython"
                }
                );
        }
    }
}