#pragma once

#include "UEPyFInputEvent.h"

extern PyTypeObject ue_PyFInputEventType;

typedef struct
{
	/* Type-specific fields go here. */
	ue_PyFInputEvent f_input;
	FPointerEvent pointer;
} ue_PyFPointerEvent;

void ue_python_init_fpointer_event(PyObject *);

PyObject *py_ue_new_fpointer_event(FPointerEvent);
ue_PyFPointerEvent *py_ue_is_fpointer_event(PyObject *);