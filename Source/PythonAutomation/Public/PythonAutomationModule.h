// Copyright 1998-2018 20Tab S.r.l. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION==4 && ENGINE_MINOR_VERSION>=22
#include "Modules/ModuleInterface.h"
#else
#include "ModuleInterface.h"
#endif

class FPythonAutomationModule : public IModuleInterface
{
public:
	virtual void StartupModule();
	virtual void ShutdownModule();

};
