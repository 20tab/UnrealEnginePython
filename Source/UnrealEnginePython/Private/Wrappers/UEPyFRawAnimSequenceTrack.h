#pragma once

#include "UEPyModule.h"

#include "Runtime/Engine/Classes/Animation/AnimSequence.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FRawAnimSequenceTrack raw_anim_sequence_track;
} ue_PyFRawAnimSequenceTrack;

PyObject *py_ue_new_fraw_anim_sequence_track(FRawAnimSequenceTrack);

void ue_python_init_fraw_anim_sequence_track(PyObject *);

ue_PyFRawAnimSequenceTrack *py_ue_is_fraw_anim_sequence_track(PyObject *);