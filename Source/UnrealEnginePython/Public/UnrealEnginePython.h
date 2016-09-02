// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPython, Log, All);


class FUnrealEnginePythonModule : public IModuleInterface
{
public:

	void PythonGILAcquire();
	void PythonGILRelease();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void *ue_python_gil;
};

