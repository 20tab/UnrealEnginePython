#pragma once



#include "UEPyModule.h"

PyObject *py_ue_texture_get_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_render_target_get_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_render_target_get_data_to_buffer(ue_PyUObject *, PyObject *);

PyObject *py_ue_texture_set_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_texture_get_width(ue_PyUObject *, PyObject *);
PyObject *py_ue_texture_get_height(ue_PyUObject *, PyObject *);

PyObject *py_ue_texture_has_alpha_channel(ue_PyUObject *, PyObject *);

PyObject *py_unreal_engine_compress_image_array(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_checkerboard_texture(PyObject *, PyObject *);

PyObject *py_unreal_engine_create_transient_texture(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_transient_texture_render_target2d(PyObject *, PyObject *);

PyObject *py_ue_texture_update_resource(ue_PyUObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_create_texture(PyObject * self, PyObject *);
PyObject *py_ue_texture_get_source_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_texture_set_source_data(ue_PyUObject *, PyObject *);
#endif
