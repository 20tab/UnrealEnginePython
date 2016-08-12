#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_add_controller_yaw_input(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float val;
	if (!PyArg_ParseTuple(args, "f:add_controller_yaw_input", &val)) {
		return NULL;
	}

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>()) {
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor) {
			if (actor->IsA<APawn>()) {
				pawn = (APawn *)actor;
			}
		}
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a pawn");

	pawn->AddControllerYawInput(val);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_add_controller_pitch_input(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float val;
	if (!PyArg_ParseTuple(args, "f:add_controller_pitch_input", &val)) {
		return NULL;
	}

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>()) {
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor) {
			if (actor->IsA<APawn>()) {
				pawn = (APawn *)actor;
			}
		}
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a pawn");

	pawn->AddControllerPitchInput(val);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_add_movement_input(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float x, y, z;
	float scale = 1;
	PyObject *py_force = nullptr;
	bool force = false;
	if (!PyArg_ParseTuple(args, "fff|fO:add_movement_input", &x, &y, &z, &scale, &py_force)) {
		return NULL;
	}

	if (py_force && PyObject_IsTrue(py_force)) {
		force = true;
	}

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>()) {
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor) {
			if (actor->IsA<APawn>()) {
				pawn = (APawn *)actor;
			}
		}
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a pawn");

	pawn->AddMovementInput(FVector(x, y, z), scale, force);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_get_control_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>()) {
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor) {
			if (actor->IsA<APawn>()) {
				pawn = (APawn *)actor;
			}
		}
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a pawn");

	FRotator rot = pawn->GetControlRotation();
	return Py_BuildValue("fff", rot.Roll, rot.Pitch, rot.Yaw);
}


