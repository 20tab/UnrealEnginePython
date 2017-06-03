#pragma once



#include "UnrealEnginePython.h"

#if WITH_EDITOR
PyObject *py_ue_static_mesh_build(ue_PyUObject *, PyObject *);
PyObject *py_ue_static_mesh_create_body_setup(ue_PyUObject *, PyObject *);
#endif