#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "Runtime/Engine/Public/SkeletalMeshTypes.h"

#if ENGINE_MINOR_VERSION > 18

#include "Runtime/Engine/Public/Rendering/SkeletalMeshLODModel.h"

#endif

#include "Wrappers/UEPyFColor.h"

struct ue_PyFSoftSkinVertex {
	PyObject_HEAD
	/* Type-specific fields go here. */

	FSoftSkinVertex ss_vertex;

	uint16 material_index;
	uint32 smoothing_group;
};

void ue_python_init_fsoft_skin_vertex(PyObject *);

PyObject *py_ue_new_fsoft_skin_vertex(FSoftSkinVertex);

ue_PyFSoftSkinVertex *py_ue_is_fsoft_skin_vertex(PyObject *);

#endif
