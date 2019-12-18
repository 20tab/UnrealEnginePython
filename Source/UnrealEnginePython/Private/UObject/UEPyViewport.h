#pragma once



#include "UEPyModule.h"
PyObject *py_unreal_engine_get_game_viewport_client(PyObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_get_editor_pie_game_viewport_client(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_set_view_mode(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_set_camera_speed(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_set_view_location(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_set_view_rotation(PyObject *, PyObject *);
#endif

PyObject *py_ue_add_viewport_widget_content(ue_PyUObject *, PyObject *);
PyObject *py_ue_remove_viewport_widget_content(ue_PyUObject *, PyObject *);
PyObject *py_ue_remove_all_viewport_widgets(ue_PyUObject *, PyObject *);
PyObject *py_ue_game_viewport_client_set_rendering_flag(ue_PyUObject *, PyObject *);
PyObject *py_ue_game_viewport_client_get_window(ue_PyUObject *, PyObject *);
