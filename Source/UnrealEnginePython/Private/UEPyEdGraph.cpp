#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "Runtime/Engine/Classes/EdGraph/EdGraph.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CallFunction.h"
#include "Editor/BlueprintGraph/Classes/EdGraphSchema_K2.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CustomEvent.h"
#include "Editor/BlueprintGraph/Classes/K2Node_VariableGet.h"
#include "Editor/BlueprintGraph/Classes/K2Node_VariableSet.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"

PyObject *py_ue_graph_add_node_call_function(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_function = NULL;
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

PyObject *py_ue_graph_add_node_custom_event(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name = nullptr;
	int x = 0;
	int y = 0;
	if (!PyArg_ParseTuple(args, "s|ii:graph_add_node_custom_event", &name, &x, &y)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UEdGraph>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraph");
	}

	UEdGraph *graph = (UEdGraph *)self->ue_object;

	UK2Node_CustomEvent *node = NewObject<UK2Node_CustomEvent>(graph);

	node->CreateNewGuid();
	node->PostPlacedNewNode();
	node->CustomFunctionName = name;
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

PyObject *py_ue_graph_add_node_variable_get(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name = nullptr;
	PyObject *py_struct = nullptr;
	int x = 0;
	int y = 0;
	if (!PyArg_ParseTuple(args, "s|Oii:graph_add_node_variable_get", &name, &py_struct, &x, &y)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UEdGraph>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraph");
	}

	UEdGraph *graph = (UEdGraph *)self->ue_object;
	UStruct *u_struct = nullptr;

	if (py_struct && py_struct != Py_None) {
		if (!ue_is_pyuobject(py_struct)) {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		ue_PyUObject *ue_py_struct = (ue_PyUObject *)py_struct;
		if (!ue_py_struct->ue_object->IsA<UStruct>()) {
			return PyErr_Format(PyExc_Exception, "argument is not a UStruct");
		}
		u_struct = (UStruct *)ue_py_struct->ue_object;
	}

	UK2Node_VariableGet *node = NewObject<UK2Node_VariableGet>(graph);

	node->CreateNewGuid();
	node->PostPlacedNewNode();
	node->SetFlags(RF_Transactional);
	node->AllocateDefaultPins();
	node->NodePosX = x;
	node->NodePosY = y;
	UEdGraphSchema_K2::ConfigureVarNode(node, FName(UTF8_TO_TCHAR(name)), u_struct, FBlueprintEditorUtils::FindBlueprintForGraph(graph));
	UEdGraphSchema_K2::SetNodeMetaData(node, FNodeMetadata::DefaultGraphNode);
	graph->AddNode(node);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(node);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

PyObject *py_ue_graph_add_node_variable_set(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name = nullptr;
	PyObject *py_struct = nullptr;
	int x = 0;
	int y = 0;
	if (!PyArg_ParseTuple(args, "s|Oii:graph_add_node_variable_set", &name, &py_struct, &x, &y)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UEdGraph>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraph");
	}

	UEdGraph *graph = (UEdGraph *)self->ue_object;
	UStruct *u_struct = nullptr;

	if (py_struct && py_struct != Py_None) {
		if (!ue_is_pyuobject(py_struct)) {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		ue_PyUObject *ue_py_struct = (ue_PyUObject *)py_struct;
		if (!ue_py_struct->ue_object->IsA<UStruct>()) {
			return PyErr_Format(PyExc_Exception, "argument is not a UStruct");
		}
		u_struct = (UStruct *)ue_py_struct->ue_object;
	}

	UK2Node_VariableSet *node = NewObject<UK2Node_VariableSet>(graph);

	node->CreateNewGuid();
	node->PostPlacedNewNode();
	node->SetFlags(RF_Transactional);
	node->AllocateDefaultPins();
	node->NodePosX = x;
	node->NodePosY = y;
	UEdGraphSchema_K2::ConfigureVarNode(node, FName(UTF8_TO_TCHAR(name)), u_struct, FBlueprintEditorUtils::FindBlueprintForGraph(graph));
	UEdGraphSchema_K2::SetNodeMetaData(node, FNodeMetadata::DefaultGraphNode);
	graph->AddNode(node);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(node);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}
#endif
