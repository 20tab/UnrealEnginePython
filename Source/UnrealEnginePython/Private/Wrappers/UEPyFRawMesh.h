#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR

#if ENGINE_MINOR_VERSION > 13

#include "Developer/RawMesh/Public/RawMesh.h"

struct ue_PyFRawMesh
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FRawMesh raw_mesh;
};


void ue_python_init_fraw_mesh(PyObject *);

PyObject *py_ue_new_fraw_mesh(FRawMesh);

#endif
#endif
