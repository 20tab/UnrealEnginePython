#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_anim_get_skeleton(ue_PyUObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_ue_anim_sequence_get_raw_animation_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_get_raw_animation_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_add_new_raw_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_anim_composite_section(ue_PyUObject *, PyObject *);
#endif
PyObject *py_ue_anim_set_skeleton(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_blend_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_blend_parameter(ue_PyUObject *, PyObject *);