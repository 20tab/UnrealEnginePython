#pragma once



#include "UnrealEnginePython.h"
PyObject *py_unreal_engine_get_game_viewport_client(PyObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_get_editor_game_viewport_client(PyObject *, PyObject *);
#endif

PyObject *py_ue_add_viewport_widget_content(ue_PyUObject *, PyObject *);
PyObject *py_ue_remove_viewport_widget_content(ue_PyUObject *, PyObject *);
PyObject *py_ue_remove_all_viewport_widgets(ue_PyUObject *, PyObject *);
