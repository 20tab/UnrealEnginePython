#pragma once

#include "UEPyModule.h"



PyObject *py_ue_quit_game(ue_PyUObject *, PyObject *);
PyObject *py_ue_play(ue_PyUObject *, PyObject *);
PyObject *py_ue_world_exec(ue_PyUObject *, PyObject *);

// mainly used for unit testing
PyObject *py_ue_world_tick(ue_PyUObject *, PyObject *);


PyObject *py_ue_all_objects(ue_PyUObject *, PyObject *);
PyObject *py_ue_all_actors(ue_PyUObject *, PyObject *);
PyObject *py_ue_find_object(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_world(ue_PyUObject *, PyObject *);
PyObject *py_ue_has_world(ue_PyUObject *, PyObject *);

PyObject *py_ue_set_view_target(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_world_delta_seconds(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_levels(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_game_viewport(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_current_level(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_current_level(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_world_type(ue_PyUObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_ue_world_create_folder(ue_PyUObject *, PyObject *);
PyObject *py_ue_world_delete_folder(ue_PyUObject *, PyObject *);
PyObject *py_ue_world_rename_folder(ue_PyUObject *, PyObject *);
PyObject *py_ue_world_folders(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_level_script_blueprint(ue_PyUObject *, PyObject *);
#endif

