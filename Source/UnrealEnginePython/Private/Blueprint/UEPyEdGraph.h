#pragma once



#include "UEPyModule.h"
#include "UEPyCallable.h"

#include "UEPyEdGraphPin.h"

#if WITH_EDITOR
PyObject *py_ue_graph_add_node_call_function(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_custom_event(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_variable_get(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_variable_set(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_dynamic_cast(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_event(ue_PyUObject *, PyObject *);

PyObject *py_ue_graph_reconstruct_node(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_remove_node(ue_PyUObject *, PyObject *);

PyObject *py_ue_graph_get_good_place_for_new_node(ue_PyUObject *, PyObject *);

PyObject *py_ue_node_pins(ue_PyUObject *, PyObject *);
PyObject *py_ue_node_find_pin(ue_PyUObject *, PyObject *);
PyObject *py_ue_node_create_pin(ue_PyUObject *, PyObject *);

PyObject *py_ue_node_pin_type_changed(ue_PyUObject *, PyObject *);
PyObject *py_ue_node_pin_default_value_changed(ue_PyUObject *, PyObject *);

PyObject *py_ue_node_function_entry_set_pure(ue_PyUObject *, PyObject *);

PyObject *py_ue_node_get_title(ue_PyUObject *, PyObject *);
PyObject *py_ue_node_allocate_default_pins(ue_PyUObject *, PyObject *);
PyObject *py_ue_node_reconstruct(ue_PyUObject *, PyObject *);
#endif
