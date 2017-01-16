#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "Runtime/Engine/Classes/EdGraph/EdGraph.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CallFunction.h"
#include "Editor/BlueprintGraph/Classes/EdGraphSchema_K2.h"

PyObject *py_ue_graph_add_node_call_function(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_function = NULL;
	char *name = nullptr;
	int x = 0;
	int y = 0;
	if (!PyArg_ParseTuple(args, "O|ii:graph_add_node_call_function", &py_function, &x, &y)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UEdGraph>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraph");
	}

	UEdGraph *graph = (UEdGraph *)self->ue_object;

	UFunction *function = nullptr;

	if (ue_is_pyuobject(py_function)) {
		ue_PyUObject *py_function_obj = (ue_PyUObject *)py_function;
		if (py_function_obj->ue_object->IsA<UFunction>()) {
			function = (UFunction *)py_function_obj->ue_object;
		}
		else {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
	}
	else if (py_ue_is_callable(py_function)) {
		ue_PyCallable *py_callable = (ue_PyCallable *)py_function;
		function = py_callable->u_function;
	}

	if (!function)
		return PyErr_Format(PyExc_Exception, "uobject is not a UFunction");

	UK2Node_CallFunction *node = NewObject<UK2Node_CallFunction>(graph);

	node->CreateNewGuid();
	node->PostPlacedNewNode();
	node->SetFromFunction(function);
	node->SetFlags(RF_Transactional);
	node->AllocateDefaultPins();
	node->NodePosX = x;
	node->NodePosY = y;
	UEdGraphSchema_K2::SetNodeMetaData(node, FNodeMetadata::DefaultGraphNode);
	graph->AddNode(node);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(node);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}
#endif
