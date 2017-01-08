#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR

PyObject *py_ue_set_scalar_parameter( ue_PyUObject *, PyObject * );
PyObject *py_ue_set_vector_parameter( ue_PyUObject *, PyObject * );
PyObject *py_ue_set_texture_parameter( ue_PyUObject *, PyObject * );

PyObject *py_ue_set_parent( ue_PyUObject *, PyObject * );

PyObject *py_ue_static_mesh_set_collision_for_lod(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_set_shadow_for_lod(ue_PyUObject *, PyObject *);

#endif
