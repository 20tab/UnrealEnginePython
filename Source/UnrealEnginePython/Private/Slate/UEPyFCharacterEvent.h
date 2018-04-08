#pragma once

#include "UEPyFInputEvent.h"

extern PyTypeObject ue_PyFInputEventType;

typedef struct
{
	/* Type-specific fields go here. */
	ue_PyFInputEvent f_input;
	FCharacterEvent character_event;
} ue_PyFCharacterEvent;

void ue_python_init_fcharacter_event(PyObject *);

PyObject *py_ue_new_fcharacter_event(FCharacterEvent);
ue_PyFCharacterEvent *py_ue_is_fcharacter_event(PyObject *);