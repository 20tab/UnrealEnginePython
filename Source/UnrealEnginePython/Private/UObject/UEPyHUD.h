#pragma once



#include "UEPyModule.h"


PyObject *py_ue_hud_draw_2d_line(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_hud_draw_line(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_hud_draw_texture(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_hud_draw_text(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_hud_draw_rect(ue_PyUObject *self, PyObject * args);