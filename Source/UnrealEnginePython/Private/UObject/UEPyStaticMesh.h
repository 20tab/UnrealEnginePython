#pragma once



#include "UEPyModule.h"

#if WITH_EDITOR
PyObject *py_ue_static_mesh_build(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_get_num_triangles(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_static_mesh_create_body_setup(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_can_lods_share_static_lighting(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_get_raw_mesh(ue_PyUObject *, PyObject *);

#endif