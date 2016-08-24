#include "UnrealEnginePythonPrivatePCH.h"



#if WITH_EDITOR
PyObject *py_unreal_engine_get_editor_world(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	UWorld *world = GEditor->GetEditorWorldContext().World();
	ue_PyUObject *ret = ue_get_python_wrapper(world);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_editor_get_selected_actors(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *actors = PyList_New(0);

	USelection *selection = GEditor->GetSelectedActors();
	int32 nums = selection->CountSelections<UObject>();

	for (int32 i = 0; i < nums; i++) {
		UObject *obj = selection->GetSelectedObject(i);
		if (!obj->IsA<AActor>())
			continue;
		AActor *actor = (AActor *)obj;
		ue_PyUObject *item = ue_get_python_wrapper(actor);
		if (item)
			PyList_Append(actors, (PyObject *)item);
	}

	return actors;
}

PyObject *py_unreal_engine_editor_deselect_actors(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	GEditor->SelectNone(true, true, false);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_select_actor(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:editor_select_actor", &obj)) {
		return NULL;
	}

	if (!ue_is_pyuobject(obj))
		return PyErr_Format(PyExc_Exception, "invalid uobject");

	ue_PyUObject *py_actor = (ue_PyUObject *)obj;

	if (!py_actor->ue_object->IsA<AActor>())
		return PyErr_Format(PyExc_Exception, "uobject is not an Actor");

	AActor *actor = (AActor *)py_actor->ue_object;

	GEditor->SelectActor(actor, true, true);

	Py_INCREF(Py_None);
	return Py_None;
}
#endif

