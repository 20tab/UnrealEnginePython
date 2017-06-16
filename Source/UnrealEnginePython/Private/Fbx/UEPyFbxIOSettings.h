#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include <fbxsdk.h>

struct ue_PyFbxIOSettings {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FbxIOSettings *fbx_io_settings;
};


void ue_python_init_fbx_io_settings(PyObject *);

ue_PyFbxIOSettings *py_ue_is_fbx_io_settings(PyObject *);
#endif
