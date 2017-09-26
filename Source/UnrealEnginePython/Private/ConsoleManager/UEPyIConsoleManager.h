#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Core/Public/HAL/IConsoleManager.h"
#include "UEPyIConsoleManager.generated.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyIConsoleManager;

UCLASS()
class UPythonConsoleDelegate : public UPythonDelegate
{
	GENERATED_BODY()

public:
	void OnConsoleCommand(const TArray < FString > &InArgs);
};

void ue_python_init_iconsole_manager(PyObject *);
