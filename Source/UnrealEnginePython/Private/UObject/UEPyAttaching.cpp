#include "UEPyAttaching.h"

#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"

PyObject *py_ue_get_socket_location(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_location", &socket_name))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FVector vec = component->GetSocketLocation(UTF8_TO_TCHAR(socket_name));
	return py_ue_new_fvector(vec);
}

PyObject *py_ue_get_socket_rotation(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_rotation", &socket_name))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FRotator rot = component->GetSocketRotation(UTF8_TO_TCHAR(socket_name));
	return py_ue_new_frotator(rot);
}

PyObject *py_ue_get_socket_transform(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_transform", &socket_name))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FTransform transform = component->GetSocketTransform(UTF8_TO_TCHAR(socket_name), ERelativeTransformSpace::RTS_Component);
	return py_ue_new_ftransform(transform);
}

PyObject *py_ue_get_socket_world_transform(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_world_transform", &socket_name))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FTransform transform = component->GetSocketTransform(UTF8_TO_TCHAR(socket_name), ERelativeTransformSpace::RTS_World);
	return py_ue_new_ftransform(transform);
}

PyObject *py_ue_get_socket_actor_transform(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *socket_name;
	if (!PyArg_ParseTuple(args, "s:get_socket_actor_transform", &socket_name))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *component = (USceneComponent *)self->ue_object;

	FTransform transform = component->GetSocketTransform(UTF8_TO_TCHAR(socket_name), ERelativeTransformSpace::RTS_Actor);
	return py_ue_new_ftransform(transform);
}

PyObject *py_ue_get_all_child_actors(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);
	PyObject *py_include_descendants = NULL;
	if (!PyArg_ParseTuple(args, "|O:get_all_child_actors", &py_include_descendants))
	{
		return NULL;
	}

	bool include_descendants = true;
	if (py_include_descendants && !PyObject_IsTrue(py_include_descendants))
		include_descendants = false;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve actor from UObject");


	PyObject *py_children = PyList_New(0);

	TArray<AActor *> children;
	actor->GetAllChildActors(children, include_descendants);

	for (AActor *child : children)
	{
		ue_PyUObject *item = ue_get_python_uobject(child);
		if (item)
			PyList_Append(py_children, (PyObject *)item);
	}
	return py_children;
}

PyObject *py_ue_get_attached_actors(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve actor from UObject");

	PyObject *py_children = PyList_New(0);

	TArray<AActor *> children;
	actor->GetAttachedActors(children);

	for (AActor *child : children)
	{
		ue_PyUObject *item = ue_get_python_uobject(child);
		if (item)
			PyList_Append(py_children, (PyObject *)item);
	}
	return py_children;
}

PyObject *py_ue_attach_to_actor(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	char *socket_name = (char *)"";
	int location_rule = (int)EAttachmentRule::SnapToTarget;
	int rotation_rule = (int)EAttachmentRule::KeepWorld;
	int scale_rule = (int)EAttachmentRule::SnapToTarget;
	PyObject *py_weld = nullptr;
	if (!PyArg_ParseTuple(args, "O|siiiO:attach_to_actor", &obj, &socket_name, &location_rule, &rotation_rule, &scale_rule, &py_weld))
	{
		return NULL;
	}


	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "unable to retrieve Actor from uobject");

	ue_PyUObject *py_obj = ue_is_pyuobject(obj);
	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	if (!py_obj->ue_object->IsA<AActor>())
		return PyErr_Format(PyExc_Exception, "argument is not an Actor");

	AActor *other = (AActor *)py_obj->ue_object;

	bool weld = false;
	if (py_weld && PyObject_IsTrue(py_weld))
		weld = true;

	actor->AttachToActor(other, FAttachmentTransformRules((EAttachmentRule)location_rule, (EAttachmentRule)rotation_rule, (EAttachmentRule)scale_rule, weld), UTF8_TO_TCHAR(socket_name));

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_attach_to_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	char *socket_name = (char *)"";
	int location_rule = (int)EAttachmentRule::SnapToTarget;
	int rotation_rule = (int)EAttachmentRule::KeepWorld;
	int scale_rule = (int)EAttachmentRule::SnapToTarget;
	PyObject *py_weld = nullptr;
	if (!PyArg_ParseTuple(args, "O|siiiO:attach_to_component", &obj, &socket_name, &location_rule, &rotation_rule, &scale_rule, &py_weld))
	{
		return NULL;
	}


	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "unable to retrieve Actor from uobject");

	ue_PyUObject *py_obj = ue_is_pyuobject(obj);
	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	if (!py_obj->ue_object->IsA<USceneComponent>())
		return PyErr_Format(PyExc_Exception, "argument is not a SceneComponent");

	USceneComponent *other = (USceneComponent *)py_obj->ue_object;

	bool weld = false;
	if (py_weld && PyObject_IsTrue(py_weld))
		weld = true;

	actor->AttachToComponent(other, FAttachmentTransformRules((EAttachmentRule)location_rule, (EAttachmentRule)rotation_rule, (EAttachmentRule)scale_rule, weld), UTF8_TO_TCHAR(socket_name));

	Py_INCREF(Py_None);
	return Py_None;

}