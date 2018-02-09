#pragma once

#include "UnrealEnginePython.h"

typedef struct
{
	PyObject_HEAD
	/* Type-specific fields go here. */
	UScriptStruct *u_struct;
	uint8 *data;
	// if 1, data points to un-owned memory
	int is_ptr;
	// points to the original struct memory (do not try this at home !)
	uint8 *original_data;
} ue_PyUScriptStruct;

PyObject *py_ue_new_uscriptstruct(UScriptStruct *, uint8 *);
PyObject *py_ue_wrap_uscriptstruct(UScriptStruct *, uint8 *);
ue_PyUScriptStruct *py_ue_is_uscriptstruct(PyObject *);

UProperty *ue_struct_get_field_from_name(UScriptStruct *, char *);

void ue_python_init_uscriptstruct(PyObject *);