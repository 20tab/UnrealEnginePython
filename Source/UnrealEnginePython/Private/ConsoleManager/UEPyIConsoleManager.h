#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Core/Public/HAL/IConsoleManager.h"

typedef struct
{
	PyObject_HEAD
	/* Type-specific fields go here. */
} ue_PyIConsoleManager;

class FPythonSmartConsoleDelegate : public FPythonSmartDelegate
{

public:
	void OnConsoleCommand(const TArray < FString > &InArgs);
};

void ue_python_init_iconsole_manager(PyObject *);
