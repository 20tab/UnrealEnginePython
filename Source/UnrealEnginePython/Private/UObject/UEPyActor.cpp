#include "UEPyActor.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/UnrealEd/Public/ComponentTypeRegistry.h"
#endif

PyObject *py_ue_actor_has_tag(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *tag;
	if (!PyArg_ParseTuple(args, "s:actor_has_tag", &tag))
	{
		return nullptr;
	}

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	if (actor->ActorHasTag(FName(UTF8_TO_TCHAR(tag))))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_component_has_tag(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *tag;
	if (!PyArg_ParseTuple(args, "s:component_has_tag", &tag))
	{
		return nullptr;
	}

	UActorComponent *component = ue_py_check_type<UActorComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not an UActorComponent");

	if (component->ComponentHasTag(FName(UTF8_TO_TCHAR(tag))))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_actor_begin_play(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	Py_BEGIN_ALLOW_THREADS;

#if ENGINE_MINOR_VERSION > 14
	actor->DispatchBeginPlay();
#else
	actor->BeginPlay();
#endif
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}

PyObject *py_ue_get_actor_bounds(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	FVector origin;
	FVector extent;

	actor->GetActorBounds(false, origin, extent);

	return Py_BuildValue("OO", py_ue_new_fvector(origin), py_ue_new_fvector(extent));
}

PyObject *py_ue_get_actor_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	char *name;
	if (!PyArg_ParseTuple(args, "s:get_actor_component", &name))
	{
		return NULL;
	}

	for (UActorComponent *component : actor->GetComponents())
	{
		if (component->GetName().Equals(UTF8_TO_TCHAR(name)))
		{
			Py_RETURN_UOBJECT(component);
		}
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_actor_destroy_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	PyObject *py_component;
	if (!PyArg_ParseTuple(args, "O:actor_destroy_component", &py_component))
	{
		return NULL;
	}

	UActorComponent *component = ue_py_check_type<UActorComponent>(py_component);
	if (!component)
		return PyErr_Format(PyExc_Exception, "argument is not a UActorComponent");

	Py_BEGIN_ALLOW_THREADS
#if ENGINE_MINOR_VERSION >= 17
		component->DestroyComponent();
#else
		actor->K2_DestroyComponent(component);
#endif
	Py_END_ALLOW_THREADS

		Py_RETURN_NONE;
}

PyObject *py_ue_actor_destroy(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	Py_BEGIN_ALLOW_THREADS;
	actor->Destroy();
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;

}


PyObject *py_ue_actor_components(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	PyObject *ret = PyList_New(0);

	for (UActorComponent *component : actor->GetComponents())
	{
		ue_PyUObject *py_obj = ue_get_python_uobject(component);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);
	}

	return ret;
}


PyObject *py_ue_get_actor_velocity(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetVelocity());

}


#if WITH_EDITOR
PyObject *py_ue_component_type_registry_invalidate_class(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	UClass *Class = ue_py_check_type<UClass>(self);
	if (!Class)
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");

	if (!Class->IsChildOf<UActorComponent>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a subclass of UActorComponent");
	}

	FComponentTypeRegistry::Get().InvalidateClass(Class);

	Py_RETURN_NONE;
}

PyObject *py_ue_get_folder_path(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor");

	const FName DirPath = actor->GetFolderPath();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*DirPath.ToString()));
}

PyObject *py_ue_set_folder_path(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *path;
	PyObject *py_bool = nullptr;

	if (!PyArg_ParseTuple(args, "s|O:set_folder_path", &path, &py_bool))
		return nullptr;

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor");

	FName DirPath = FName(UTF8_TO_TCHAR(path));

	if (py_bool && PyObject_IsTrue(py_bool))
	{
		actor->SetFolderPath_Recursively(DirPath);
	}
	else
	{
		actor->SetFolderPath(DirPath);
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_get_actor_label(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (self->ue_object->IsA<AActor>())
	{
		AActor *actor = (AActor *)self->ue_object;
		return PyUnicode_FromString(TCHAR_TO_UTF8(*(actor->GetActorLabel())));
	}

	if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		return PyUnicode_FromString(TCHAR_TO_UTF8(*(component->GetOwner()->GetActorLabel())));
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
}

PyObject *py_ue_set_actor_label(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	char *label;
	if (!PyArg_ParseTuple(args, "s:set_actor_label", &label))
	{
		return NULL;
	}

	actor->SetActorLabel(UTF8_TO_TCHAR(label), true);

	Py_RETURN_NONE;
}

PyObject *py_ue_set_actor_hidden_in_game(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "O:set_actor_hidden_in_game", &py_bool))
	{
		return nullptr;
	}

	if (PyObject_IsTrue(py_bool))
	{
		actor->SetActorHiddenInGame(true);
	}
	else
	{
		actor->SetActorHiddenInGame(false);
	}



	Py_RETURN_NONE;
}

PyObject *py_ue_find_actor_by_label(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_actor_by_label", &name))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	UObject *u_object = nullptr;

	for (TActorIterator<AActor> Itr(world); Itr; ++Itr)
	{
		AActor *u_obj = *Itr;
		if (u_obj->GetActorLabel().Equals(UTF8_TO_TCHAR(name)))
		{
			u_object = u_obj;
			break;
		}
	}

	if (u_object)
	{
		Py_RETURN_UOBJECT(u_object);
	}

	Py_RETURN_NONE;
}


#endif

PyObject *py_ue_get_owner(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UActorComponent *component = ue_py_check_type<UActorComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a component");

	Py_RETURN_UOBJECT(component->GetOwner());
}

PyObject *py_ue_register_component(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<UActorComponent>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a component");
	}

	UActorComponent *component = (UActorComponent *)self->ue_object;

	Py_BEGIN_ALLOW_THREADS;
	if (!component->IsRegistered())
		component->RegisterComponent();
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}

PyObject *py_ue_component_is_registered(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UActorComponent *component = ue_py_check_type<UActorComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a component");

	if (component->IsRegistered())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_setup_attachment(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_component;
	char *socket_name = nullptr;

	if (!PyArg_ParseTuple(args, "O|s:setup_attachment", &py_component))
		return nullptr;

	USceneComponent *child = ue_py_check_type<USceneComponent>(self);
	if (!child)
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	USceneComponent *parent = ue_py_check_type<USceneComponent>(py_component);
	if (!parent)
		return PyErr_Format(PyExc_Exception, "argument is not a USceneComponent");

	FName SocketName = NAME_None;
	if (socket_name)
		SocketName = FName(UTF8_TO_TCHAR(socket_name));

	Py_BEGIN_ALLOW_THREADS;
	child->SetupAttachment(parent, SocketName);
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}

PyObject *py_ue_unregister_component(ue_PyUObject * self, PyObject * args)
{
	ue_py_check(self);

	UActorComponent *component = ue_py_check_type<UActorComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not an UActorComponent");

	Py_BEGIN_ALLOW_THREADS;
	if (component->IsRegistered())
		component->UnregisterComponent();
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}

PyObject *py_ue_destroy_component(ue_PyUObject * self, PyObject * args)
{
	ue_py_check(self);

	UActorComponent *component = ue_py_check_type<UActorComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not an UActorComponent");

	Py_BEGIN_ALLOW_THREADS;
	component->DestroyComponent();
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}

PyObject *py_ue_add_instance_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_component;
	if (!PyArg_ParseTuple(args, "O:add_instance_component", &py_component))
	{
		return nullptr;
	}


	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	UActorComponent *component = ue_py_check_type<UActorComponent>(py_component);
	if (!component)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UActorComponent");
	}

	Py_BEGIN_ALLOW_THREADS;
	actor->AddInstanceComponent(component);
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;

}


PyObject *py_ue_add_actor_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	char *name;
	PyObject *py_parent = nullptr;
	if (!PyArg_ParseTuple(args, "Os|O:add_actor_component", &obj, &name, &py_parent))
	{
		return nullptr;
	}

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	UClass *u_class = ue_py_check_type<UClass>(obj);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	if (!u_class->IsChildOf<UActorComponent>())
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UClass derived from UActorComponent");
	}

	USceneComponent *parent_component = nullptr;

	if (py_parent)
	{
		parent_component = ue_py_check_type<USceneComponent>(py_parent);
		if (!parent_component)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a USceneComponent");
		}
	}
	UActorComponent *component = nullptr;
	Py_BEGIN_ALLOW_THREADS;
	component = NewObject<UActorComponent>(actor, u_class, FName(UTF8_TO_TCHAR(name)), RF_Public);
	if (component)
	{

		if (py_parent && component->IsA<USceneComponent>())
		{
			USceneComponent *scene_component = (USceneComponent *)component;
			scene_component->SetupAttachment(parent_component);
		}

		if (actor->GetWorld() && !component->IsRegistered())
		{
			component->RegisterComponent();
		}

		if (component->bWantsInitializeComponent && !component->HasBeenInitialized() && component->IsRegistered())
			component->InitializeComponent();
	}

	Py_END_ALLOW_THREADS;

	if (!component)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	Py_RETURN_UOBJECT(component);
}

PyObject *py_ue_add_python_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	char *module_name;
	char *class_name;
	if (!PyArg_ParseTuple(args, "sss:add_python_component", &name, &module_name, &class_name))
	{
		return nullptr;
	}

	AActor *actor = ue_py_check_type<AActor >(self);
	if (!actor)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	UPythonComponent *component = nullptr;
	Py_BEGIN_ALLOW_THREADS;
	component = NewObject<UPythonComponent>(actor, FName(UTF8_TO_TCHAR(name)), RF_Public);

	if (component)
	{
		component->PythonModule = FString(UTF8_TO_TCHAR(module_name));
		component->PythonClass = FString(UTF8_TO_TCHAR(class_name));

		if (actor->GetWorld() && !component->IsRegistered())
		{
			component->RegisterComponent();
		}

		if (component->bWantsInitializeComponent && !component->HasBeenInitialized() && component->IsRegistered())
			component->InitializeComponent();
	}
	Py_END_ALLOW_THREADS;

	if (!component)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	Py_RETURN_UOBJECT(component);
}

PyObject *py_ue_actor_create_default_subobject(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:actor_create_default_subobject", &obj, &name))
	{
		return nullptr;
	}

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	UClass *u_class = ue_py_check_type<UClass>(obj);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	if (!FUObjectThreadContext::Get().TopInitializer())
		return PyErr_Format(PyExc_Exception, "CreateDefaultSubobject() can be called only in a constructor");

	UObject *ret_obj = nullptr;

	Py_BEGIN_ALLOW_THREADS;
	ret_obj = actor->CreateDefaultSubobject(FName(UTF8_TO_TCHAR(name)), UObject::StaticClass(), u_class, false, false, true);
	Py_END_ALLOW_THREADS;

	if (!ret_obj)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	Py_RETURN_UOBJECT(ret_obj);
}

PyObject *py_ue_get_actor_root_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	UActorComponent *component = actor->GetRootComponent();
	if (component)
	{
		Py_RETURN_UOBJECT(component);
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_add_actor_root_component(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:add_actor_root_component", &obj, &name))
	{
		return nullptr;
	}

	AActor *actor = ue_py_check_type<AActor>(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");


	UClass *u_class = ue_py_check_type<UClass>(obj);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	USceneComponent *component = NewObject<USceneComponent>(actor, u_class, FName(UTF8_TO_TCHAR(name)), RF_Public);
	if (!component)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	Py_BEGIN_ALLOW_THREADS;
	actor->SetRootComponent(component);

	if (actor->GetWorld() && !component->IsRegistered())
	{
		component->RegisterComponent();
	}

	if (component->bWantsInitializeComponent && !component->HasBeenInitialized() && component->IsRegistered())
		component->InitializeComponent();

	Py_END_ALLOW_THREADS;

	Py_RETURN_UOBJECT(component);
}

PyObject *py_ue_actor_has_component_of_type(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:actor_has_component_of_type", &obj))
	{
		return NULL;
	}

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	UClass *u_class = ue_py_check_type<UClass>(obj);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	if (actor->GetComponentByClass(u_class))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_TRUE;
}

PyObject *py_ue_get_actor_component_by_type(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:get_actor_component_by_type", &obj))
	{
		return nullptr;
	}

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	UClass *u_class = ue_py_check_type<UClass>(obj);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");


	UActorComponent *component = actor->GetComponentByClass(u_class);
	if (component)
	{
		Py_RETURN_UOBJECT(component);
	}

	Py_RETURN_NONE;

}

PyObject *py_ue_get_actor_components_by_type(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:get_actor_components_by_type", &obj))
	{
		return nullptr;
	}

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	UClass *u_class = ue_py_check_type<UClass>(obj);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	PyObject *components = PyList_New(0);

	for (UActorComponent *component : actor->GetComponentsByClass(u_class))
	{
		ue_PyUObject *item = ue_get_python_uobject(component);
		if (item)
			PyList_Append(components, (PyObject *)item);
	}

	return components;

}

PyObject *py_ue_get_actor_components_by_tag(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *tag;
	PyObject *py_uclass = nullptr;
	if (!PyArg_ParseTuple(args, "s|O:get_actor_components_by_tag", &tag, &py_uclass))
	{
		return nullptr;
	}

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	PyObject *components = PyList_New(0);

	UClass *u_class = UActorComponent::StaticClass();

	if (py_uclass)
	{
		u_class = ue_py_check_type<UClass>(py_uclass);
		if (!u_class)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UClass");
		}

		if (!u_class->IsChildOf<UActorComponent>())
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UClass inheriting from UActorComponent");
		}
	}

	for (UActorComponent *component : actor->GetComponentsByTag(u_class, FName(UTF8_TO_TCHAR(tag))))
	{
		ue_PyUObject *item = ue_get_python_uobject(component);
		if (item)
			PyList_Append(components, (PyObject *)item);
	}

	return components;

}


PyObject *py_ue_actor_spawn(ue_PyUObject * self, PyObject * args, PyObject *kwargs)
{

	ue_py_check(self);

	PyObject *py_class;

	PyObject *py_obj_location = nullptr;
	PyObject *py_obj_rotation = nullptr;

	if (!PyArg_ParseTuple(args, "O|OO:actor_spawn", &py_class, &py_obj_location, &py_obj_rotation))
	{
		return nullptr;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	UClass *u_class = ue_py_check_type<UClass>(py_class);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");


	if (!u_class->IsChildOf<AActor>())
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UClass derived from AActor");
	}

	FVector location = FVector(0, 0, 0);
	FRotator rotation = FRotator(0, 0, 0);

	if (py_obj_location)
	{
		ue_PyFVector *py_location = py_ue_is_fvector(py_obj_location);
		if (!py_location)
			return PyErr_Format(PyExc_Exception, "location must be an FVector");
		location = py_location->vec;
	}

	if (py_obj_rotation)
	{
		ue_PyFRotator *py_rotation = py_ue_is_frotator(py_obj_rotation);
		if (!py_rotation)
			return PyErr_Format(PyExc_Exception, "location must be an FRotator");
		rotation = py_rotation->rot;
	}

	AActor *actor = nullptr;

	if (kwargs && PyDict_Size(kwargs) > 0)
	{
		FTransform transform;
		transform.SetTranslation(location);
		transform.SetRotation(rotation.Quaternion());
		Py_BEGIN_ALLOW_THREADS;
		actor = world->SpawnActorDeferred<AActor>(u_class, transform);
		Py_END_ALLOW_THREADS;
		if (!actor)
			return PyErr_Format(PyExc_Exception, "unable to spawn a new Actor");
		ue_PyUObject *py_actor = ue_get_python_uobject_inc(actor);
		if (!py_actor)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");

		PyObject *py_iter = PyObject_GetIter(kwargs);

		while (PyObject *py_key = PyIter_Next(py_iter))
		{
			PyObject *void_ret = py_ue_set_property(py_actor, Py_BuildValue("OO", py_key, PyDict_GetItem(kwargs, py_key)));
			if (!void_ret)
			{
				Py_DECREF(py_iter);
				return PyErr_Format(PyExc_Exception, "unable to set property for new Actor");
			}
		}
		Py_DECREF(py_iter);
		Py_BEGIN_ALLOW_THREADS;
		UGameplayStatics::FinishSpawningActor(actor, transform);
		Py_END_ALLOW_THREADS;
		return (PyObject *)py_actor;
	}

	Py_BEGIN_ALLOW_THREADS;
	actor = world->SpawnActor(u_class, &location, &rotation);
	Py_END_ALLOW_THREADS;
	if (!actor)
		return PyErr_Format(PyExc_Exception, "unable to spawn a new Actor");
	Py_RETURN_UOBJECT(actor);

}

PyObject *py_ue_get_overlapping_actors(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);


	PyObject *class_filter = nullptr;
	if (!PyArg_ParseTuple(args, "|O:get_overlapping_actors", &class_filter))
	{
		return nullptr;
	}

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve actor from UObject");


	UClass *filtering = AActor::StaticClass();

	if (class_filter)
	{

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

	for (AActor *overlapping_actor : overalpping_actors)
	{
		ue_PyUObject *item = ue_get_python_uobject(overlapping_actor);
		if (item)
		{
			PyList_Append(py_overlapping_actors, (PyObject *)item);
		}
	}
	return py_overlapping_actors;
}

PyObject *py_ue_actor_set_level_sequence(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_sequence;
	if (!PyArg_ParseTuple(args, "O:actor_set_level_sequence", &py_sequence))
	{
		return NULL;
	}

	ALevelSequenceActor *actor = ue_py_check_type<ALevelSequenceActor>(self);
	if (!actor)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a LevelSequenceActor");
	}

	ULevelSequence *sequence = ue_py_check_type<ULevelSequence>(py_sequence);
	if (!sequence)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a LevelSequence");
	}

	Py_BEGIN_ALLOW_THREADS;
	actor->SetSequence(sequence);
	Py_END_ALLOW_THREADS;

	Py_RETURN_NONE;
}


#if WITH_EDITOR
PyObject *py_ue_get_editor_world_counterpart_actor(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve actor from UObject");

	AActor *editor_actor = EditorUtilities::GetEditorWorldCounterpartActor(actor);
	if (!editor_actor)
		return PyErr_Format(PyExc_Exception, "unable to retrieve editor counterpart actor");

	Py_RETURN_UOBJECT(editor_actor);
}
#endif