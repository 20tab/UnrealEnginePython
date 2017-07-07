#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_anim_sequence_get_skeleton(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_get_raw_animation_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_get_raw_animation_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_add_new_raw_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_set_skeleton(ue_PyUObject *, PyObject *);