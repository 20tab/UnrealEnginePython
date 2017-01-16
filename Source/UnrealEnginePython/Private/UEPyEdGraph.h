#pragma once



#include "UnrealEnginePython.h"

#if WITH_EDITOR
PyObject *py_ue_graph_add_node_call_function(ue_PyUObject *, PyObject *);
PyObject *py_ue_graph_add_node_custom_event(ue_PyUObject *, PyObject *);
#endif
