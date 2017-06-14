#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Core/Public/HAL/IConsoleManager.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
} ue_PyIConsoleManager;


void ue_python_init_iconsole_manager(PyObject *);
