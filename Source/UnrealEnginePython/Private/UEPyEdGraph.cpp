#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "Runtime/Engine/Classes/EdGraph/EdGraph.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CallFunction.h"
#include "Editor/BlueprintGraph/Classes/EdGraphSchema_K2.h"
#include "Editor/BlueprintGraph/Classes/K2Node_CustomEvent.h"
#include "Editor/BlueprintGraph/Classes/K2Node_VariableGet.h"
#include "Editor/BlueprintGraph/Classes/K2Node_VariableSet.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Editor/BlueprintGraph/Classes/EdGraphSchema_K2_Actions.h"

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

	UBlueprint *bp = (UBlueprint *)node->GetGraph()->GetOuter();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

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

	UBlueprint *bp = (UBlueprint *)node->GetGraph()->GetOuter();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(node);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

PyObject *py_ue_graph_get_good_place_for_new_node(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UEdGraph>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraph");
	}

	UEdGraph *graph = (UEdGraph *)self->ue_object;
	FVector2D pos = graph->GetGoodPlaceForNewNode();

	PyObject *ret = PyTuple_New(2);
	PyTuple_SetItem(ret, 0, PyLong_FromDouble(pos.X));
	PyTuple_SetItem(ret, 1, PyLong_FromDouble(pos.Y));
	return ret;
}

PyObject *py_ue_graph_add_node_event(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_class = nullptr;
	char *name = nullptr;
	int x = 0;
	int y = 0;

	if (!PyArg_ParseTuple(args, "Os|ii:graph_add_node_event", &py_class, &name, &x, &y)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UEdGraph>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraph");
	}

	UEdGraph *graph = (UEdGraph *)self->ue_object;

	if (!ue_is_pyuobject(py_class)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_class;
	if (!py_obj->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");
	}

	UClass *u_class = (UClass *)py_obj->ue_object;
	UBlueprint *bp = (UBlueprint *)graph->GetOuter();

	UK2Node_Event *node = FBlueprintEditorUtils::FindOverrideForFunction(bp, u_class, UTF8_TO_TCHAR(name));
	if (!node) {
		node = NewObject<UK2Node_Event>(graph);

		UEdGraphSchema_K2::SetNodeMetaData(node, FNodeMetadata::DefaultGraphNode);
		node->EventReference.SetExternalMember(UTF8_TO_TCHAR(name), u_class);
		FEdGraphSchemaAction_K2NewNode::SpawnNodeFromTemplate<UK2Node_Event>(graph, node, FVector2D(x, y));
		//graph->AddNode(node);
	}

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

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

	UEdGraphSchema_K2::ConfigureVarNode(node, FName(UTF8_TO_TCHAR(name)), u_struct, FBlueprintEditorUtils::FindBlueprintForGraph(graph));
	UEdGraphSchema_K2::SetNodeMetaData(node, FNodeMetadata::DefaultGraphNode);
	FEdGraphSchemaAction_K2NewNode::SpawnNodeFromTemplate<UK2Node_VariableGet>(graph, node, FVector2D(x, y));
	//graph->AddNode(node);

	UBlueprint *bp = (UBlueprint *)node->GetGraph()->GetOuter();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

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

	UEdGraphSchema_K2::ConfigureVarNode(node, FName(UTF8_TO_TCHAR(name)), u_struct, FBlueprintEditorUtils::FindBlueprintForGraph(graph));
	UEdGraphSchema_K2::SetNodeMetaData(node, FNodeMetadata::DefaultGraphNode);
	FEdGraphSchemaAction_K2NewNode::SpawnNodeFromTemplate<UK2Node_VariableSet>(graph, node, FVector2D(x, y));
	//graph->AddNode(node);

	UBlueprint *bp = (UBlueprint *)node->GetGraph()->GetOuter();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(node);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

PyObject *py_ue_graph_add_node(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_node_class;
	int x = 0;
	int y = 0;
	if (!PyArg_ParseTuple(args, "O|ii:graph_add_node", &py_node_class, &x, &y)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UEdGraph>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraph");
	}

	UEdGraph *graph = (UEdGraph *)self->ue_object;

	if (!ue_is_pyuobject(py_node_class)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	UEdGraphNode *node = nullptr;

	ue_PyUObject *py_obj = (ue_PyUObject *)py_node_class;
	if (py_obj->ue_object->IsA<UClass>()) {
		UClass *u_class = (UClass *)py_obj->ue_object;
		if (!u_class->IsChildOf<UEdGraphNode>()) {
			return PyErr_Format(PyExc_Exception, "argument is not a child of UEdGraphNode");
		}
		node = (UEdGraphNode *)NewObject<UObject>(graph, u_class);
		node->PostLoad();
	}
	else if (py_obj->ue_object->IsA<UEdGraphNode>()) {
		node = (UEdGraphNode *)py_obj->ue_object;
		if (node->GetOuter() != graph) {
			node->Rename(*node->GetName(), graph);
		}
	}

	if (!node) {
		return PyErr_Format(PyExc_Exception, "argument is not a supported type");
	}

	node->CreateNewGuid();
	node->PostPlacedNewNode();
	node->SetFlags(RF_Transactional);
	node->AllocateDefaultPins();
	node->NodePosX = x;
	node->NodePosY = y;
	UEdGraphSchema_K2::SetNodeMetaData(node, FNodeMetadata::DefaultGraphNode);
	graph->AddNode(node);

	if (UBlueprint *bp = Cast<UBlueprint>(node->GetGraph()->GetOuter())) {
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);
	}

	PyObject *ret = (PyObject *)ue_get_python_wrapper(node);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;
}

PyObject *py_ue_node_pins(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UEdGraphNode>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraphNode");
	}

	UEdGraphNode *node = (UEdGraphNode *)self->ue_object;

	PyObject *pins_list = PyList_New(0);
	for (UEdGraphPin *pin : node->Pins) {
		PyList_Append(pins_list, (PyObject *)py_ue_new_edgraphpin(pin));
	}
	return pins_list;
}

PyObject *py_ue_node_find_pin(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name = nullptr;
	if (!PyArg_ParseTuple(args, "s:node_find_pin", &name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UEdGraphNode>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UEdGraphNode");
	}

	UEdGraphNode *node = (UEdGraphNode *)self->ue_object;

	UEdGraphPin *pin = node->FindPin(UTF8_TO_TCHAR(name));
	if (!pin) {
		return PyErr_Format(PyExc_Exception, "unable to find pin \"%s\"", name);
	}

	PyObject *ret = py_ue_new_edgraphpin(pin);
	Py_INCREF(ret);
	return ret;
}
#endif
