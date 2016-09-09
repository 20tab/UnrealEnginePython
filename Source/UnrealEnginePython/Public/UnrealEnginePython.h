// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPython, Log, All);


class UNREALENGINEPYTHON_API FUnrealEnginePythonModule : public IModuleInterface
{
public:

	void PythonGILAcquire();
	void PythonGILRelease();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void *ue_python_gil;
};

struct FScopePythonGIL {
	FScopePythonGIL()
	{
		UnrealEnginePythonModule = FModuleManager::LoadModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
		UnrealEnginePythonModule.PythonGILAcquire();
	}

	~FScopePythonGIL()
	{
		UnrealEnginePythonModule.PythonGILRelease();
	}

	FUnrealEnginePythonModule UnrealEnginePythonModule;
};

