#pragma once



#include "UnrealEnginePython.h"

#if WITH_EDITOR
PyObject *py_ue_graph_add_node_call_function(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_custom_event(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_variable_get(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_variable_set(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_event(ue_PyUObject *, PyObject *);

PyObject *py_ue_graph_get_good_place_for_new_node(ue_PyUObject *, PyObject *);

PyObject *py_ue_node_pins(ue_PyUObject *, PyObject *);
PyObject *py_ue_node_find_pin(ue_PyUObject *, PyObject *);
PyObject *py_ue_node_create_pin(ue_PyUObject *, PyObject *);
#endif
