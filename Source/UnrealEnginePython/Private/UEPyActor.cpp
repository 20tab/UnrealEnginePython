#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_actor_has_tag(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *tag;
	if (!PyArg_ParseTuple(args, "s:actor_has_tag", &tag)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (actor->ActorHasTag(FName(UTF8_TO_TCHAR(tag)))) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;


}

PyObject *py_ue_get_actor_bounds(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);


	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	FVector origin;
	FVector extent;

	actor->GetActorBounds(false, origin, extent);

	return Py_BuildValue("OO", py_ue_new_fvector(origin), py_ue_new_fvector(extent));

}

PyObject *py_ue_get_actor_component(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	char *name;
	if (!PyArg_ParseTuple(args, "s:get_actor_component", &name)) {
		return NULL;
	}

	for (UActorComponent *component : actor->GetComponents()) {
		if (component->GetName().Equals(UTF8_TO_TCHAR(name))) {
			ue_PyUObject *py_obj = ue_get_python_wrapper(component);
			if (!py_obj)
				return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
			Py_INCREF(py_obj);
			return (PyObject *)py_obj;
		}
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_actor_destroy(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	actor->Destroy();

	Py_INCREF(Py_None);
	return Py_None;


}


PyObject *py_ue_actor_components(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	PyObject *ret = PyList_New(0);

	for (UActorComponent *component : actor->GetComponents()) {
		ue_PyUObject *py_obj = ue_get_python_wrapper(component);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);
	}

	return ret;
}


PyObject *py_ue_get_actor_velocity(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetVelocity());

}


#if WITH_EDITOR
PyObject *py_ue_get_actor_label(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (self->ue_object->IsA<AActor>()) {
		AActor *actor = (AActor *)self->ue_object;
		return PyUnicode_FromString(TCHAR_TO_UTF8(*(actor->GetActorLabel())));
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)self->ue_object;
		return PyUnicode_FromString(TCHAR_TO_UTF8(*(component->GetOwner()->GetActorLabel())));
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
}

PyObject *py_ue_set_actor_label(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	char *label;
	if (!PyArg_ParseTuple(args, "s:set_actor_label", &label)) {
		return NULL;
	}

	actor->SetActorLabel(UTF8_TO_TCHAR(label), true);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_find_actor_by_label(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_actor_by_label", &name)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	UObject *u_object = nullptr;

	for (TActorIterator<AActor> Itr(world); Itr; ++Itr) {
		AActor *u_obj = *Itr;
		if (u_obj->GetActorLabel().Equals(UTF8_TO_TCHAR(name))) {
			u_object = u_obj;
			break;
		}
	}

	if (u_object) {
		ue_PyUObject *ret = ue_get_python_wrapper(u_object);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}

	Py_INCREF(Py_None);
	return Py_None;
}


#endif

PyObject *py_ue_get_owner(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UActorComponent>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a component");
	}

	UActorComponent *component = (UActorComponent *)self->ue_object;

	ue_PyUObject *ret = ue_get_python_wrapper(component->GetOwner());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_register_component(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UActorComponent>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a component");
	}

	UActorComponent *component = (UActorComponent *)self->ue_object;

	component->RegisterComponent();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_component_is_registered(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UActorComponent>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a component");
	}

	UActorComponent *component = (UActorComponent *)self->ue_object;

	if (component->IsRegistered()) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_add_actor_component(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:add_actor_component", &obj, &name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a class");
	}

	UActorComponent *component = NewObject<UActorComponent>(actor, (UClass *)py_obj->ue_object, FName(UTF8_TO_TCHAR(name)));
	if (!component)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	if (actor->GetWorld()) {
		component->RegisterComponent();
	}

	PyObject *ret = (PyObject *)ue_get_python_wrapper(component);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

PyObject *py_ue_actor_create_default_subobject(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:actor_create_default_subobject", &obj, &name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a class");
	}

	UObject *ret_obj = actor->CreateDefaultSubobject(FName(UTF8_TO_TCHAR(name)), UObject::StaticClass(), (UClass *)py_obj->ue_object, false, false, true);
	if (!ret_obj)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	PyObject *ret = (PyObject *)ue_get_python_wrapper(ret_obj);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

PyObject *py_ue_add_actor_root_component(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:add_actor_root_component", &obj, &name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a class");
	}

	USceneComponent *component = NewObject<USceneComponent>(actor, (UClass *)py_obj->ue_object, FName(UTF8_TO_TCHAR(name)));
	if (!component)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	actor->SetRootComponent(component);

	if (actor->GetWorld()) {
		component->RegisterComponent();
	}

	PyObject *ret = (PyObject *)ue_get_python_wrapper(component);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

PyObject *py_ue_actor_has_component_of_type(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:actor_has_component_of_type", &obj)) {
		return NULL;
	}

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (actor->GetComponentByClass((UClass *)py_obj->ue_object)) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;

}

PyObject *py_ue_get_actor_component_by_type(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:get_actor_component_by_type", &obj)) {
		return NULL;
	}

	ue_PyUObject *py_obj = nullptr;

	if (ue_is_pyuobject(obj)) {
		py_obj = (ue_PyUObject *)obj;
	}
	// shortcut for finding class by string
	else if (PyUnicodeOrString_Check(obj)) {
		char *class_name = PyUnicode_AsUTF8(obj);
		UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(class_name));

		if (u_class) {
			py_obj = ue_get_python_wrapper(u_class);
		}
	}

	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	UActorComponent *component = actor->GetComponentByClass((UClass *)py_obj->ue_object);
	if (component) {
		PyObject *ret = (PyObject *)ue_get_python_wrapper(component);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(ret);
		return ret;
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_get_actor_components_by_type(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:get_actor_components_by_type", &obj)) {
		return NULL;
	}

	ue_PyUObject *py_obj = nullptr;

	if (ue_is_pyuobject(obj)) {
		py_obj = (ue_PyUObject *)obj;
	}
	// shortcut for finding class by string
	else if (PyUnicodeOrString_Check(obj)) {
		char *class_name = PyUnicode_AsUTF8(obj);
		UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(class_name));

		if (u_class) {
			py_obj = ue_get_python_wrapper(u_class);
		}
	}

	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	PyObject *components = PyList_New(0);

	for (UActorComponent *component : actor->GetComponentsByClass((UClass *)py_obj->ue_object)) {
		ue_PyUObject *item = ue_get_python_wrapper(component);
		if (item)
			PyList_Append(components, (PyObject *)item);
	}

	return components;

}


PyObject *py_ue_actor_spawn(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_obj_location = nullptr;
	PyObject *py_obj_rotation = nullptr;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O|OO:actor_spawn", &obj, &py_obj_location, &py_obj_rotation)) {
		return NULL;
	}

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	FVector location = FVector(0,0,0);
	FRotator rotation = FRotator(0,0,0);

	if (py_obj_location) {
		ue_PyFVector *py_location = py_ue_is_fvector(py_obj_location);
		if (!py_location)
			return PyErr_Format(PyExc_Exception, "location must be an FVector");
		location = py_location->vec;
	}

	if (py_obj_rotation) {
		ue_PyFRotator *py_rotation = py_ue_is_frotator(py_obj_rotation);
		if (!py_rotation)
			return PyErr_Format(PyExc_Exception, "location must be an FRotator");
		rotation = py_rotation->rot;
	}

	AActor *actor = world->SpawnActor((UClass *)py_obj->ue_object, &location, &rotation);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(actor);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;
}

PyObject *py_ue_get_overlapping_actors(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve actor from UObject");

	PyObject *class_filter = nullptr;
	if (!PyArg_ParseTuple(args, "|O:get_overlapping_actors", &class_filter)) {
		return NULL;
	}

	UClass *filtering = AActor::StaticClass();

	if (class_filter) {

		if (!ue_is_pyuobject(class_filter))
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");

		ue_PyUObject *py_obj = (ue_PyUObject *)class_filter;

		if (!py_obj->ue_object->IsA((UClass *)py_obj->ue_object))
			return PyErr_Format(PyExc_Exception, "argument is not a UClass");

		filtering = (UClass *)py_obj->ue_object;
	}

	PyObject *py_overlapping_actors = PyList_New(0);

	TArray<AActor *> overalpping_actors;
	actor->GetOverlappingActors(overalpping_actors, filtering);

	for (AActor *overlapping_actor : overalpping_actors) {
		ue_PyUObject *item = ue_get_python_wrapper(overlapping_actor);
		if (item) {
			PyList_Append(py_overlapping_actors, (PyObject *)item);
		}
	}
	return py_overlapping_actors;
}