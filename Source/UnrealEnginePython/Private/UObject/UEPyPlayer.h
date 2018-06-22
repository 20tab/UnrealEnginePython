#pragma once



#include "UEPyModule.h"

PyObject *py_ue_create_player(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_num_players(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_num_spectators(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_player_controller(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_player_hud(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_player_camera_manager(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_player_pawn(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_player_hud(ue_PyUObject *, PyObject *);
PyObject *py_ue_restart_level(ue_PyUObject *, PyObject *);