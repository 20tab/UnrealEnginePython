#pragma once

#include "UEPyModule.h"

#include "Runtime/SlateCore/Public/Input/Events.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FInputEvent input;
} ue_PyFInputEvent;

void ue_python_init_finput_event(PyObject *);

PyObject *py_ue_new_finput_event(FInputEvent);
