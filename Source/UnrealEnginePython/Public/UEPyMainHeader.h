#pragma once

#if defined(UNREAL_ENGINE_PYTHON_ON_MAC)
#include <Headers/Python.h>
#include <Headers/structmember.h>
#elif defined(UNREAL_ENGINE_PYTHON_ON_LINUX)
#include <Python.h>
#include <structmember.h>
#else
#include <include/Python.h>
#include <include/structmember.h>
#endif


#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Styling/SlateStyle.h"
#include "UObject/ScriptMacros.h"
#include "Runtime/Launch/Resources/Version.h"
