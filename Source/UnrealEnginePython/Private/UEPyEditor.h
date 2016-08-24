#pragma once



#include "UnrealEnginePython.h"
PyObject *py_unreal_engine_get_editor_world(PyObject *, PyObject * args);
PyObject *py_unreal_engine_editor_get_selected_actors(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_deselect_actors(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_select_actor(PyObject *, PyObject *);