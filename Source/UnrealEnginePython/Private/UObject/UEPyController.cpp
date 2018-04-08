#include "UEPyController.h"

#include "GameFramework/Controller.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

PyObject *py_ue_controller_posses(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:posses", &obj))
	{
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

PyObject *py_ue_controller_get_hud(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	APlayerController *controller = ue_py_check_type<APlayerController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

	Py_RETURN_UOBJECT((UObject *)controller->GetHUD());
}

PyObject *py_ue_controller_unposses(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AController *controller = ue_py_check_type<AController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

	controller->UnPossess();

	Py_RETURN_NONE;
}

PyObject *py_ue_get_controlled_pawn(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AController *controller = ue_py_check_type<AController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

#if ENGINE_MINOR_VERSION >= 15
	APawn *pawn = controller->GetPawn();
#else
	APawn *pawn = controller->GetControlledPawn();
#endif
	if (!pawn)
	{
		Py_RETURN_NONE;
	}

	Py_RETURN_UOBJECT(pawn);
}

PyObject *py_ue_controller_project_world_location_to_screen(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_point;
	PyObject *py_relative = nullptr;

	if (!PyArg_ParseTuple(args, "O|O:project_world_location_to_screen", &py_obj_point, &py_relative))
		return nullptr;

	APlayerController *controller = ue_py_check_type<APlayerController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "uobject is not an AController");

	ue_PyFVector *point = py_ue_is_fvector(py_obj_point);
	if (!point)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");

	// TODO: Check return value:
	FVector2D screenLocation;
	if (!controller->ProjectWorldLocationToScreen(point->vec, screenLocation, (py_relative && PyObject_IsTrue(py_relative))))
	{
		return PyErr_Format(PyExc_Exception, "unable to project coordinates");
	}

	return Py_BuildValue("(ff)", screenLocation.X, screenLocation.Y);
}