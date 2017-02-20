#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_controller_posses(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:posses", &obj)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AController>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an APawn");
	}

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<APawn>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a APAwn");
	}

	APawn *pawn = (APawn *)py_obj->ue_object;
	AController *controller = (AController *)self->ue_object;

	controller->Possess(pawn);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_controller_unposses(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<AController>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an APawn");
	}

	AController *controller = (AController *)self->ue_object;

	controller->UnPossess();

	Py_INCREF(Py_None);
	return Py_None;
}