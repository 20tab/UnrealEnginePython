#pragma once
#include "UnrealEnginePython.h"

#if ENGINE_MINOR_VERSION >= 20

#include "Runtime/Core/Public/Misc/FrameNumber.h"

struct ue_PyFFrameNumber {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FFrameNumber frame_number;
};

void ue_python_init_fframe_number(PyObject *);

PyObject *py_ue_new_fframe_number(FFrameNumber);

ue_PyFFrameNumber *py_ue_is_fframe_number(PyObject *);
#endif
