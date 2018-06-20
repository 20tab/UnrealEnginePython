// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonAutomationModule.h"
#include "UnrealEnginePython.h"
#include "UEPyFAutomationEditorCommonUtils.h"

IMPLEMENT_MODULE(FPythonAutomationModule, PythonAutomation);


void FPythonAutomationModule::StartupModule()
{
	FScopePythonGIL gil;
	PyObject *py_automation_module = ue_py_register_module("unreal_engine.automation");
	ue_python_init_fautomation_editor_common_utils(py_automation_module);
}

void FPythonAutomationModule::ShutdownModule()
{

}
