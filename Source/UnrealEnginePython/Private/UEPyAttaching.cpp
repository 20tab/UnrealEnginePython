#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_get_socket_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_location", &socket_name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FVector vec = component->GetSocketLocation(UTF8_TO_TCHAR(socket_name));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}

PyObject *py_ue_get_socket_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_rotation", &socket_name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FRotator rot = component->GetSocketRotation(UTF8_TO_TCHAR(socket_name));
	return Py_BuildValue("fff", rot.Roll, rot.Pitch, rot.Yaw);
}

PyObject *py_ue_get_socket_transform(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_transform", &socket_name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FTransform transform = component->GetSocketTransform(UTF8_TO_TCHAR(socket_name), ERelativeTransformSpace::RTS_Component);
	FVector location = transform.GetLocation();
	FRotator rot = FRotator(transform.GetRotation());
	return Py_BuildValue("ffffff", location.X, location.Y, location.Z, rot.Roll, rot.Pitch, rot.Yaw);
}

PyObject *py_ue_get_socket_world_transform(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_world_transform", &socket_name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FTransform transform = component->GetSocketTransform(UTF8_TO_TCHAR(socket_name), ERelativeTransformSpace::RTS_World);
	FVector location = transform.GetLocation();
	FRotator rot = FRotator(transform.GetRotation());
	return Py_BuildValue("ffffff", location.X, location.Y, location.Z, rot.Roll, rot.Pitch, rot.Yaw);
}

PyObject *py_ue_get_socket_actor_transform(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_actor_transform", &socket_name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FTransform transform = component->GetSocketTransform(UTF8_TO_TCHAR(socket_name), ERelativeTransformSpace::RTS_Actor);
	FVector location = transform.GetLocation();
	FRotator rot = FRotator(transform.GetRotation());
	return Py_BuildValue("ffffff", location.X, location.Y, location.Z, rot.Roll, rot.Pitch, rot.Yaw);
}
