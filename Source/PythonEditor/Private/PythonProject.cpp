// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorPrivatePCH.h"


UPythonProject::UPythonProject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	Path = PythonModule.ScriptsPath;
}
