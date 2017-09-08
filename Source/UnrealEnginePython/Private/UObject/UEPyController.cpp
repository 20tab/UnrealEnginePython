#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_controller_posses(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:posses", &obj)) {
		return NULL;
	}

	AController *controller = ue_py_check_type<AController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

	APawn *pawn = ue_py_check_type<APawn>(obj);
	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not an APawn");

	controller->Possess(pawn);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_controller_get_hud(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	APlayerController *controller = ue_py_check_type<APlayerController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

	ue_PyUObject *ret = ue_get_python_wrapper((UObject *)controller->GetHUD());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_controller_unposses(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AController *controller = ue_py_check_type<AController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

	controller->UnPossess();

	Py_RETURN_NONE;
}

PyObject *py_ue_get_controlled_pawn(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AController *controller = ue_py_check_type<AController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

#if ENGINE_MINOR_VERSION >= 15
	APawn *pawn = controller->GetPawn();
#else
	APawn *pawn = controller->GetControlledPawn();
#endif
	if (!pawn) {
		Py_RETURN_NONE;
	}

	ue_PyUObject *ret = ue_get_python_wrapper(pawn);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}