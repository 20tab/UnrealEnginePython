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
#if UEPY_THREADING
		UnrealEnginePythonModule = FModuleManager::LoadModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
		UnrealEnginePythonModule.PythonGILAcquire();
#endif
	}

	~FScopePythonGIL()
	{
#if UEPY_THREADING
		UnrealEnginePythonModule.PythonGILRelease();
#endif
	}

	FUnrealEnginePythonModule UnrealEnginePythonModule;
};

