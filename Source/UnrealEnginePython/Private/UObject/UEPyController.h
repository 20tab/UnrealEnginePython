#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_controller_posses(ue_PyUObject *, PyObject *);
PyObject *py_ue_controller_unposses(ue_PyUObject *, PyObject *);
PyObject *py_ue_controller_get_hud(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_controlled_pawn(ue_PyUObject *, PyObject *);