#pragma once


#include "UnrealEnginePython.h"


PyObject *py_ue_capture_initialize(ue_PyUObject *, PyObject *);
PyObject *py_ue_capture_start(ue_PyUObject *, PyObject *);
PyObject *py_ue_capture_stop(ue_PyUObject *, PyObject *);