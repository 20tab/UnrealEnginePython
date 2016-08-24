#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_quit_game(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	// no need to support multiple controllers
	APlayerController *controller = world->GetFirstPlayerController();
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve the first controller");

	UKismetSystemLibrary::QuitGame(world, controller, EQuitPreference::Quit);

	Py_INCREF(Py_None);
	return Py_None;
}

// mainly used for testing
PyObject *py_ue_world_tick(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float delta_time;
	if (!PyArg_ParseTuple(args, "f:world_tick", &delta_time)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	world->Tick(LEVELTICK_All, delta_time);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_all_objects(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *ret = PyList_New(0);

	for (TObjectIterator<UObject> Itr; Itr; ++Itr) {
		UObject *u_obj = *Itr;
		if (u_obj->GetWorld() != world)
			continue;
		ue_PyUObject *py_obj = ue_get_python_wrapper(u_obj);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);
	}
	return ret;
}

PyObject *py_ue_all_actors(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *ret = PyList_New(0);

	for (TActorIterator<AActor> Itr(world); Itr; ++Itr) {
		UObject *u_obj = *Itr;
		ue_PyUObject *py_obj = ue_get_python_wrapper(u_obj);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);

	}


	return ret;
}

PyObject *py_ue_get_world(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	ue_PyUObject *ret = ue_get_python_wrapper(world);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}