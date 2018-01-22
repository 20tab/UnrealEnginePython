#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Foliage/Public/FoliageType.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"

PyObject *py_ue_world_exec(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *command;
	if (!PyArg_ParseTuple(args, "s:world_exec", &command))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	if (world->Exec(world, UTF8_TO_TCHAR(command)))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;

}

PyObject *py_ue_quit_game(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	// no need to support multiple controllers
	APlayerController *controller = world->GetFirstPlayerController();
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve the first controller");

	UKismetSystemLibrary::QuitGame(world, controller, EQuitPreference::Quit);

	Py_RETURN_NONE;
}

PyObject *py_ue_get_world_type(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	return PyLong_FromUnsignedLong(world->WorldType);
}

PyObject *py_ue_play(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	world->BeginPlay();

	Py_RETURN_NONE;
}

// mainly used for testing
PyObject *py_ue_world_tick(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	float delta_time;
	PyObject *py_increase_fc = nullptr;
	if (!PyArg_ParseTuple(args, "f|O:world_tick", &delta_time, &py_increase_fc))
	{
		return nullptr;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	world->Tick(LEVELTICK_All, delta_time);

	if (py_increase_fc && PyObject_IsTrue(py_increase_fc))
		GFrameCounter++;

	Py_RETURN_NONE;
}

PyObject *py_ue_all_objects(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *ret = PyList_New(0);

	for (TObjectIterator<UObject> Itr; Itr; ++Itr)
	{
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

PyObject *py_ue_all_actors(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *ret = PyList_New(0);

	for (TActorIterator<AActor> Itr(world); Itr; ++Itr)
	{
		UObject *u_obj = *Itr;
		ue_PyUObject *py_obj = ue_get_python_wrapper(u_obj);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);

	}


	return ret;
}

PyObject *py_ue_find_object(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_object", &name))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	UObject *u_object = FindObject<UObject>(world->GetOutermost(), UTF8_TO_TCHAR(name));

	if (!u_object)
		return PyErr_Format(PyExc_Exception, "unable to find object %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_get_world(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_get_game_viewport(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	UGameViewportClient *viewport_client = world->GetGameViewport();
	if (!viewport_client)
		return PyErr_Format(PyExc_Exception, "world has no GameViewportClient");

	Py_RETURN_UOBJECT(viewport_client);
}

PyObject *py_ue_has_world(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (ue_get_uworld(self))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyObject *py_ue_set_view_target(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *py_obj;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "O|i:set_view_target", &py_obj, &controller_id))
	{
		return NULL;
	}

	AActor *actor = ue_py_check_type<AActor>(py_obj);
	if (!actor)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an actor");
	}

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	controller->SetViewTarget(actor);

	Py_RETURN_NONE;

}

PyObject *py_ue_get_world_delta_seconds(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	return Py_BuildValue("f", UGameplayStatics::GetWorldDeltaSeconds(world));
}

PyObject *py_ue_get_levels(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *ret = PyList_New(0);

	for (ULevel *level : world->GetLevels())
	{
		ue_PyUObject *py_obj = ue_get_python_wrapper(level);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);

	}
	return ret;
}

PyObject *py_ue_get_current_level(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	ULevel *level = world->GetCurrentLevel();
	if (!level)
		Py_RETURN_NONE;

	ue_PyUObject *ret = ue_get_python_wrapper(level);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_set_current_level(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_level;

	if (!PyArg_ParseTuple(args, "O", &py_level))
		return nullptr;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	ULevel *level = ue_py_check_type<ULevel>(py_level);
	if (!level)
		return PyErr_Format(PyExc_Exception, "argument is not a ULevel");

	if (world->SetCurrentLevel(level))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

PyObject *py_ue_get_instanced_foliage_actor_for_current_level(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	Py_RETURN_UOBJECT(AInstancedFoliageActor::GetInstancedFoliageActorForCurrentLevel(world, true));
}

#if WITH_EDITOR
PyObject *py_ue_add_foliage_asset(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_uobject;

	if (!PyArg_ParseTuple(args, "O:add_foliage_asset", &py_uobject))
	{
		return nullptr;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	UObject *u_object = ue_py_check_type<UObject>(py_uobject);
	if (!u_object)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	UFoliageType *foliage_type = nullptr;

	AInstancedFoliageActor *ifa = AInstancedFoliageActor::GetInstancedFoliageActorForCurrentLevel(world, true);
	if (u_object->IsA<UStaticMesh>())
	{
		foliage_type = ifa->GetLocalFoliageTypeForMesh((UStaticMesh *)u_object);
		if (!foliage_type)
		{
			ifa->AddMesh((UStaticMesh *)u_object, &foliage_type);
		}
	}
	else if (u_object->IsA<UFoliageType>())
	{
		foliage_type = (UFoliageType *)u_object;
		ifa->AddFoliageType(foliage_type);
		
	}

	if (!foliage_type)
		return PyErr_Format(PyExc_Exception, "unable to add foliage asset");

	Py_RETURN_UOBJECT(foliage_type);

}
#endif

