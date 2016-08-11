#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_get_actor_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorLocation();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorLocation();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

PyObject *py_ue_get_actor_scale(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorScale3D();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorScale3D();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

PyObject *py_ue_get_actor_forward(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorForwardVector();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorForwardVector();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

PyObject *py_ue_get_actor_right(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorRightVector();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorRightVector();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

PyObject *py_ue_get_actor_up(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorUpVector();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorUpVector();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

PyObject *py_ue_get_actor_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FRotator vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorRotation();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorRotation();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.Pitch, vec3.Yaw, vec3.Roll);

}

PyObject *py_ue_set_actor_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:set_actor_location", &x, &y, &z)) {
		return NULL;
	}

	if (self->ue_object->IsA<AActor>()) {
		((AActor *)self->ue_object)->SetActorLocation(FVector(x, y, z));
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		((UActorComponent *)self->ue_object)->GetOwner()->SetActorLocation(FVector(x, y, z));
		goto ret;
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_set_actor_scale(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:set_actor_scale", &x, &y, &z)) {
		return NULL;
	}

	if (self->ue_object->IsA<AActor>()) {
		((AActor *)self->ue_object)->SetActorScale3D(FVector(x, y, z));
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		((UActorComponent *)self->ue_object)->GetOwner()->SetActorScale3D(FVector(x, y, z));
		goto ret;
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_set_actor_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float pitch, yaw, roll;
	if (!PyArg_ParseTuple(args, "fff:set_actor_rotation", &pitch, &yaw, &roll)) {
		return NULL;
	}

	if (self->ue_object->IsA<AActor>()) {
		((AActor *)self->ue_object)->SetActorRotation(FRotator(pitch, yaw, roll));
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		((UActorComponent *)self->ue_object)->GetOwner()->SetActorRotation(FRotator(pitch, yaw, roll));
		goto ret;
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	Py_INCREF(Py_None);
	return Py_None;

}


PyObject *py_ue_get_world_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetComponentLocation();
		return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_world_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FRotator vec3 = ((USceneComponent *)self->ue_object)->GetComponentRotation();
		return Py_BuildValue("fff", vec3.Pitch, vec3.Yaw, vec3.Roll);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_world_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetComponentScale();
		return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->RelativeLocation;
		return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FRotator vec3 = ((USceneComponent *)self->ue_object)->RelativeRotation;
		return Py_BuildValue("fff", vec3.Pitch, vec3.Yaw, vec3.Roll);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->RelativeScale3D;
		return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_forward_vector(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetForwardVector();
		return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_up_vector(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetUpVector();
		return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_right_vector(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetRightVector();
		return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:set_world_location", &x, &y, &z)) {
		return NULL;
	}
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetWorldLocation(FVector(x, y, z));
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	float pitch, yaw, roll;
	if (!PyArg_ParseTuple(args, "fff:set_world_rotation", &pitch, &yaw, &roll)) {
		return NULL;
	}
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetWorldRotation(FRotator(pitch, yaw, roll));
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:set_world_scale", &x, &y, &z)) {
		return NULL;
	}
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetWorldScale3D(FVector(x, y, z));
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:set_relative_location", &x, &y, &z)) {
		return NULL;
	}
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetRelativeLocation(FVector(x, y, z));
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	float pitch, yaw, roll;
	if (!PyArg_ParseTuple(args, "fff:set_relative_rotation", &pitch, &yaw, &roll)) {
		return NULL;
	}
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetRelativeRotation(FRotator(pitch, yaw, roll));
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:set_relative_scale", &x, &y, &z)) {
		return NULL;
	}
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetRelativeScale3D(FVector(x, y, z));
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}