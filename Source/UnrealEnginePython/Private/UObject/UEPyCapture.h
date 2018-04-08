#pragma once


#include "UEPyModule.h"


PyObject *py_ue_capture_initialize(ue_PyUObject *, PyObject *);
PyObject *py_ue_capture_start(ue_PyUObject *, PyObject *);
PyObject *py_ue_capture_load_from_config(ue_PyUObject *, PyObject *);
PyObject *py_ue_capture_stop(ue_PyUObject *, PyObject *);

PyObject *py_ue_set_level_sequence_asset(ue_PyUObject *, PyObject *);
PyObject *py_unreal_engine_in_editor_capture(PyObject *, PyObject *);