#pragma once

#if WITH_EDITOR

#include "UnrealEnginePython.h"

#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	IPlugin *plugin;
} ue_PyIPlugin;

PyObject *py_ue_new_iplugin(IPlugin *);

void ue_python_init_iplugin(PyObject *);
#endif