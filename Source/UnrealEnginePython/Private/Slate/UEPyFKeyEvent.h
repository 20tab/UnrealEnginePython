#pragma once

#include "UnrealEnginePython.h"

#include "UEPyFInputEvent.h"

extern PyTypeObject ue_PyFInputEventType;

typedef struct {
	/* Type-specific fields go here. */
	ue_PyFInputEvent f_input;
	FKeyEvent key_event;
} ue_PyFKeyEvent;

void ue_python_init_fkey_event(PyObject *);

PyObject *py_ue_new_fkey_event(FKeyEvent);

ue_PyFKeyEvent *py_ue_is_fkey_event(PyObject *);
