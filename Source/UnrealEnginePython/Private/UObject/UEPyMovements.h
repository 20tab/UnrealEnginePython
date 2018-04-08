#pragma once



#include "UEPyModule.h"


PyObject *py_ue_add_controller_yaw_input(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_controller_pitch_input(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_controller_roll_input(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_control_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_movement_input(ue_PyUObject *, PyObject *);

PyObject *py_ue_jump(ue_PyUObject *, PyObject *);
PyObject *py_ue_stop_jumping(ue_PyUObject *, PyObject *);
PyObject *py_ue_crouch(ue_PyUObject *, PyObject *);
PyObject *py_ue_uncrouch(ue_PyUObject *, PyObject *);
PyObject *py_ue_launch(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_jumping(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_crouched(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_falling(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_flying(ue_PyUObject *, PyObject *);
PyObject *py_ue_can_jump(ue_PyUObject *, PyObject *);
PyObject *py_ue_can_crouch(ue_PyUObject *, PyObject *);



