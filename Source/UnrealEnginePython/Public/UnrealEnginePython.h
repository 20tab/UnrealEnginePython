// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPython, Log, All);


class UNREALENGINEPYTHON_API FUnrealEnginePythonModule : public IModuleInterface
{
public:

	bool PythonGILAcquire();
	void PythonGILRelease();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RunString(char *);
	void RunStringSandboxed(char *);
	void RunFile(char *);
	void RunFileSandboxed(char *);

private:
	void *ue_python_gil;
	// used by console
	void *main_dict;
	void *local_dict;
	void *main_module;
};

struct FScopePythonGIL {
	FScopePythonGIL()
	{
#if defined(UEPY_THREADING)
		UnrealEnginePythonModule = FModuleManager::LoadModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
		safeForRelease = UnrealEnginePythonModule.PythonGILAcquire();
#endif
	}

	~FScopePythonGIL()
	{
#if defined(UEPY_THREADING)
		if (safeForRelease) {
			UnrealEnginePythonModule.PythonGILRelease();
		}
#endif
	}

	FUnrealEnginePythonModule UnrealEnginePythonModule;
	bool safeForRelease;
};




