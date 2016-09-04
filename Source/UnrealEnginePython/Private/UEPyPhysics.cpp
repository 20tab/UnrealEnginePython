#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_set_simulate_physics(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:set_simulate_physics", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>()) {
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	if (!primitive) {
		return PyErr_Format(PyExc_Exception, "unable to set physics for the object");
	}

	primitive->SetSimulatePhysics(enabled);

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_ue_destructible_apply_damage(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float damage_amount = 0;
	float impulse_strength = 0;
	PyObject *py_obj_location = nullptr;
	PyObject *py_obj_impulse = nullptr;
	if (!PyArg_ParseTuple(args, "ffOO:destructible_apply_damage", &damage_amount, &impulse_strength, &py_obj_location, &py_obj_impulse)) {
		return NULL;
	}

	UDestructibleComponent *destructible = nullptr;
	AActor *actor = nullptr;

	if (self->ue_object->IsA<UDestructibleComponent>()) {
		destructible = (UDestructibleComponent *)self->ue_object;
	}
	else if (self->ue_object->IsA<AActor>()) {
		actor = (AActor *)self->ue_object;
		destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		actor = (AActor *)self->ue_object->GetOuter();
		if (actor) {
			destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
		}
	}

	FVector location = FVector(0, 0, 0);
	FVector impulse = FVector(0, 0, 0);

	if (py_obj_location) {
		ue_PyFVector *py_location = py_ue_is_fvector(py_obj_location);
		if (!py_location)
			return PyErr_Format(PyExc_Exception, "location must be a FVector");
		location = py_location->vec;
	}

	if (py_obj_impulse) {
		ue_PyFVector *py_impulse = py_ue_is_fvector(py_obj_impulse);
		if (!py_impulse)
			return PyErr_Format(PyExc_Exception, "impulse must be a FVector");
		impulse = py_impulse->vec;
	}

	if (destructible) {
		destructible->ApplyDamage(damage_amount, location, impulse, impulse_strength);
	}
	else {
		return PyErr_Format(PyExc_Exception, "UObject is not a destructible");
	}

	Py_INCREF(Py_False);
	return Py_False;
}