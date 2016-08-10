#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_is_input_key_down(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *key;
	if (!PyArg_ParseTuple(args, "s:is_input_key_down", &key)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();

	if (controller->IsInputKeyDown(key)) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_enable_input(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	/*
	TODO manage controller index
	}*/

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();
	if (!controller)
		return PyErr_Format(PyExc_Exception, "no player controller available");

	if (self->ue_object->IsA<AActor>()) {
		((AActor *)self->ue_object)->EnableInput(controller);
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		((UActorComponent *)self->ue_object)->GetOwner()->EnableInput(controller);
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_get_input_axis(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *axis_name;
	if (!PyArg_ParseTuple(args, "s:get_input_axis", &axis_name)) {
		return NULL;
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>()) {
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input) {
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	return Py_BuildValue("f", input->GetAxisValue(FName(UTF8_TO_TCHAR(axis_name))));

}

PyObject *py_ue_bind_input_axis(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *axis_name;
	if (!PyArg_ParseTuple(args, "s:bind_input_axis", &axis_name)) {
		return NULL;
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>()) {
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input) {
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	input->BindAxis(FName(UTF8_TO_TCHAR(axis_name)));

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_show_mouse_cursor(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:show_mouse_cursor", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();
	if (controller)
		controller->bShowMouseCursor = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_enable_click_events(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:enable_click_events", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();
	if (controller)
		controller->bEnableClickEvents = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_enable_mouse_over_events(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:enable_mouse_over_events", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	APlayerController *controller = world->GetFirstPlayerController();
	if (controller)
		controller->bEnableMouseOverEvents = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}





