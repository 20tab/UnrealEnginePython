#pragma once



#include "UEPyModule.h"

PyObject *py_ue_static_mesh_get_bounds(ue_PyUObject *self, PyObject * args);

#if WITH_EDITOR
PyObject *py_ue_static_mesh_build(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_create_body_setup(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_get_raw_mesh(ue_PyUObject *, PyObject *);

PyObject *py_ue_static_mesh_generate_kdop10x(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_generate_kdop10y(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_generate_kdop10z(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_generate_kdop18(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_generate_kdop26(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_import_lod(ue_PyUObject *, PyObject *);
#endif
