// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonProject.h"
#include "UnrealEnginePython.h"


UPythonProject::UPythonProject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	Path = PythonModule.ScriptsPaths[0];
}
