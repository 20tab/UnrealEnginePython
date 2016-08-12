#pragma once



#include "UnrealEnginePython.h"


PyObject *py_ue_add_controller_yaw_input(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_controller_pitch_input(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_control_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_movement_input(ue_PyUObject *, PyObject *);

