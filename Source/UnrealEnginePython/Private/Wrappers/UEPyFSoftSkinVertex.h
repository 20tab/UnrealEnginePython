#pragma once
#include "UnrealEnginePython.h"

#include "Runtime/Engine/Public/SkeletalMeshTypes.h"

struct ue_PyFSoftSkinVertex {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FSoftSkinVertex ss_vertex;
};

void ue_python_init_fsoft_skin_vertex(PyObject *);

PyObject *py_ue_new_fsoft_skin_vertex(FSoftSkinVertex);

ue_PyFSoftSkinVertex *py_ue_is_fsoft_skin_vertex(PyObject *);
