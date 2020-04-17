#pragma once

#include "UEPyModule.h"



PyObject *py_ue_set_material_scalar_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_material_static_switch_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_material_vector_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_material_texture_parameter(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_material_scalar_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_material_vector_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_material_texture_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_material_static_switch_parameter(ue_PyUObject *, PyObject *);

PyObject *py_ue_create_material_instance_dynamic(ue_PyUObject *, PyObject *);

PyObject *py_ue_set_material(ue_PyUObject *, PyObject *);

PyObject *py_ue_set_material_by_name(ue_PyUObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_ue_set_material_parent(ue_PyUObject *, PyObject *);

PyObject *py_ue_static_mesh_set_collision_for_lod(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_set_shadow_for_lod(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_material_graph(ue_PyUObject *, PyObject *);

#endif
