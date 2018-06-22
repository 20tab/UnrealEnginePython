#pragma once
#include "UnrealEnginePython.h"

#include "Wrappers/UEPyFVector.h"

#if ENGINE_MINOR_VERSION > 12

#include "Runtime/Engine/Classes/Animation/MorphTarget.h"

struct ue_PyFMorphTargetDelta {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FMorphTargetDelta morph_target_delta;
};

void ue_python_init_fmorph_target_delta(PyObject *);

PyObject *py_ue_new_fmorph_target_delta(FMorphTargetDelta);

ue_PyFMorphTargetDelta *py_ue_is_fmorph_target_delta(PyObject *);
#endif
