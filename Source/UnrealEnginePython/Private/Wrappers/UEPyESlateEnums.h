#pragma once



#include "UnrealEnginePython.h"

typedef struct {
    PyObject_HEAD
        /* Type-specific fields go here. */
        PyObject *py_dict;
} ue_PyESlateEnums;

void ue_python_init_eslate_enums(PyObject *);

ue_PyESlateEnums *py_ue_is_eslate_enums(PyObject *);