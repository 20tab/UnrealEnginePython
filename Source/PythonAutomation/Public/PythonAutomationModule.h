// Copyright 1998-2018 20Tab S.r.l. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleInterface.h"

class FPythonAutomationModule : public IModuleInterface
{
public:
	virtual void StartupModule();
	virtual void ShutdownModule();

};
