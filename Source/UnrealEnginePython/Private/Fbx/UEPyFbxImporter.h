#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include <fbxsdk.h>

struct ue_PyFbxImporter {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FbxImporter *fbx_importer;
};


void ue_python_init_fbx_importer(PyObject *);

#endif
