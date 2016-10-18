#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_set_simulate_physics(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_torque(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_physics_angular_velocity(ue_PyUObject *, PyObject *);
PyObject *py_ue_destructible_apply_damage(ue_PyUObject *, PyObject *);
