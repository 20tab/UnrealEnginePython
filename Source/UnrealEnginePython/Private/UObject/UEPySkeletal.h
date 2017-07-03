#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_get_anim_instance(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_skeletal_mesh(ue_PyUObject *, PyObject *);

PyObject *py_ue_skeleton_get_parent_index(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_bones_get_num(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_get_bone_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_find_bone_index(ue_PyUObject *, PyObject *);