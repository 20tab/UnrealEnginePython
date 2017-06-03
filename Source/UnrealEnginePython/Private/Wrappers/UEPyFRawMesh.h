#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "Developer/RawMesh/Public/RawMesh.h"

struct ue_PyFRawMesh {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FRawMesh *raw_mesh;
};


void ue_python_init_fraw_mesh(PyObject *);

#endif
