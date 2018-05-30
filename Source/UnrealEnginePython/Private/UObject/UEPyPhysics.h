#pragma once



#include "UEPyModule.h"

PyObject *py_ue_set_simulate_physics(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_impulse(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_angular_impulse(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_force(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_torque(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_physics_linear_velocity(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_physics_linear_velocity(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_physics_angular_velocity(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_physics_angular_velocity(ue_PyUObject *, PyObject *);
PyObject *py_ue_destructible_apply_damage(ue_PyUObject *, PyObject *);
