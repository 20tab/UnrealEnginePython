#include "UEPyFoliage.h"

#include "Runtime/Foliage/Public/FoliageType.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include "Wrappers/UEPyFFoliageInstance.h"

PyObject *py_ue_get_instanced_foliage_actor_for_current_level(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	Py_RETURN_UOBJECT(AInstancedFoliageActor::GetInstancedFoliageActorForCurrentLevel(world, true));
}

PyObject *py_ue_get_instanced_foliage_actor_for_level(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	ULevel *level = ue_py_check_type<ULevel>(self);
	if (!level)
		return PyErr_Format(PyExc_Exception, "uobject is not a ULevel");

	Py_RETURN_UOBJECT(AInstancedFoliageActor::GetInstancedFoliageActorForLevel(level, true));
}

PyObject *py_ue_get_foliage_types(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	AInstancedFoliageActor *foliage_actor = ue_py_check_type<AInstancedFoliageActor>(self);
	if (!foliage_actor)
		return PyErr_Format(PyExc_Exception, "uobject is not a AInstancedFoliageActor");

	PyObject *py_list = PyList_New(0);

	TArray<UFoliageType *> FoliageTypes;
#if ENGINE_MINOR_VERSION >=23
	foliage_actor->FoliageInfos.GetKeys(FoliageTypes);
#else
	foliage_actor->FoliageMeshes.GetKeys(FoliageTypes);
#endif

	for (UFoliageType *FoliageType : FoliageTypes)
	{
		PyList_Append(py_list, (PyObject*)ue_get_python_uobject(FoliageType));
	}

	return py_list;
}

#if WITH_EDITOR
PyObject *py_ue_get_foliage_instances(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	PyObject *py_foliage_type;

	if (!PyArg_ParseTuple(args, "O:get_foliage_instances", &py_foliage_type))
	{
		return nullptr;
	}

	AInstancedFoliageActor *foliage_actor = ue_py_check_type<AInstancedFoliageActor>(self);
	if (!foliage_actor)
		return PyErr_Format(PyExc_Exception, "uobject is not a AInstancedFoliageActor");

	UFoliageType *foliage_type = ue_py_check_type<UFoliageType>(py_foliage_type);
	if (!foliage_type)
		return PyErr_Format(PyExc_Exception, "argument is not a UFoliageType");

#if ENGINE_MINOR_VERSION >= 23
	if (!foliage_actor->FoliageInfos.Contains(foliage_type))
#else
	if (!foliage_actor->FoliageMeshes.Contains(foliage_type))
#endif
	{
		return PyErr_Format(PyExc_Exception, "specified UFoliageType not found in AInstancedFoliageActor");
	}

#if ENGINE_MINOR_VERSION >= 23
	FFoliageInfo& info = foliage_actor->FoliageInfos[foliage_type].Get();
#else
	FFoliageMeshInfo& info = foliage_actor->FoliageMeshes[foliage_type].Get();
#endif

	PyObject *py_list = PyList_New(0);

	for (int32 i=0; i<info.Instances.Num(); i++)
	{
		PyList_Append(py_list, py_ue_new_ffoliage_instance(foliage_actor, foliage_type, i));
	}

	return py_list;
}


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
#if ENGINE_MINOR_VERSION >= 23
		foliage_type = ifa->GetLocalFoliageTypeForSource(u_object);
#else
		foliage_type = ifa->GetLocalFoliageTypeForMesh((UStaticMesh *)u_object);
#endif
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

