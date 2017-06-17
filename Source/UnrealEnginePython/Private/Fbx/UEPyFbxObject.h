#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include <fbxsdk.h>

struct ue_PyFbxObject {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FbxObject *fbx_object;
};


void ue_python_init_fbx_object(PyObject *);

PyObject *py_ue_new_fbx_object(FbxObject *);

ue_PyFbxObject *py_ue_is_fbx_object(PyObject *);

#endif
