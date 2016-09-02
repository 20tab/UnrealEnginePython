#pragma once



#include "UnrealEnginePython.h"

#if WITH_EDITOR

PyObject *py_unreal_engine_get_editor_world(PyObject *, PyObject * args);
PyObject *py_unreal_engine_editor_get_selected_actors(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_deselect_actors(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_select_actor(PyObject *, PyObject *);
PyObject *py_unreal_engine_import_asset(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_asset(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_assets(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_assets_by_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_set_fbx_import_option(PyObject *, PyObject *);

#endif