#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_get_class(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_a(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_valid(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_child_of(ue_PyUObject *, PyObject *);
PyObject *py_ue_call_function(ue_PyUObject *, PyObject *, PyObject *);
PyObject *py_ue_find_function(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_name(ue_PyUObject *, PyObject * args);
PyObject *py_ue_get_display_name(ue_PyUObject *, PyObject * args);
PyObject *py_ue_set_name(ue_PyUObject *, PyObject * args);
PyObject *py_ue_get_full_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_path_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_property(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_property_flags(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_property_flags(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_property_flags(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_property_struct(ue_PyUObject *, PyObject *);
PyObject *py_ue_properties(ue_PyUObject *, PyObject *);
PyObject *py_ue_call(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_property(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_uproperty(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_property_class(ue_PyUObject *, PyObject *);
PyObject *py_ue_has_property(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_rooted(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_to_root(ue_PyUObject *, PyObject *);
PyObject *py_ue_remove_from_root(ue_PyUObject *, PyObject *);
PyObject *py_ue_auto_root(ue_PyUObject *, PyObject *);

PyObject *py_ue_save_config(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_cdo(ue_PyUObject *, PyObject *);
PyObject *py_ue_enum_values(ue_PyUObject *, PyObject *);
PyObject *py_ue_enum_names(ue_PyUObject *, PyObject *);
#if ENGINE_MINOR_VERSION >= 15
PyObject *py_ue_enum_user_defined_names(ue_PyUObject *, PyObject *);
#endif


PyObject *py_ue_bind_event(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_function(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_property(ue_PyUObject *, PyObject *);

PyObject *py_ue_as_dict(ue_PyUObject *, PyObject *);

PyObject *py_ue_can_modify(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_outer(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_outer(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_outermost(ue_PyUObject *, PyObject *);
PyObject *py_ue_conditional_begin_destroy(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_super_class(ue_PyUObject *, PyObject *);

PyObject *py_ue_functions(ue_PyUObject *, PyObject *);

PyObject *py_ue_broadcast(ue_PyUObject *, PyObject *);

PyObject *py_ue_post_edit_change(ue_PyUObject *, PyObject *);
PyObject *py_ue_post_edit_change_property(ue_PyUObject *, PyObject *);
PyObject *py_ue_pre_edit_change(ue_PyUObject *, PyObject *);
PyObject *py_ue_modify(ue_PyUObject *, PyObject *);

PyObject *py_ue_class_generated_by(ue_PyUObject *, PyObject *);

PyObject *py_ue_class_get_flags(ue_PyUObject *, PyObject *);
PyObject *py_ue_class_set_flags(ue_PyUObject *, PyObject *);


#if WITH_EDITOR
PyObject *py_ue_class_get_config_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_class_set_config_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_save_package(ue_PyUObject *, PyObject *);
PyObject *py_ue_duplicate(ue_PyUObject *, PyObject *);
PyObject *py_ue_asset_can_reimport(ue_PyUObject *, PyObject *);
PyObject *py_ue_asset_reimport(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_metadata(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_metadata(ue_PyUObject *, PyObject *);
PyObject *py_ue_has_metadata(ue_PyUObject *, PyObject *);
#endif

PyObject *py_ue_get_thumbnail(ue_PyUObject *, PyObject *);
PyObject *py_ue_render_thumbnail(ue_PyUObject *, PyObject *);