// Copyright 20Tab S.r.l.

#include "PyActor.h"
#include "UEPyModule.h"

APyActor::APyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	PythonTickForceDisabled = false;
	PythonDisableAutoBinding = false;

}

void APyActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (PythonModule.IsEmpty())
		return;

	FScopePythonGIL gil;

	py_uobject = ue_get_python_uobject(this);
	if (!py_uobject)
	{
		unreal_engine_py_log_error();
		return;
	}

	PyObject *py_actor_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_actor_module)
	{
		unreal_engine_py_log_error();
		return;
	}

#if WITH_EDITOR
	// todo implement autoreload with a dictionary of module timestamps
	py_actor_module = PyImport_ReloadModule(py_actor_module);
	if (!py_actor_module)
	{
		unreal_engine_py_log_error();
		return;
	}
#endif

	if (PythonClass.IsEmpty())
		return;

	PyObject *py_actor_module_dict = PyModule_GetDict(py_actor_module);
	PyObject *py_actor_class = PyDict_GetItemString(py_actor_module_dict, TCHAR_TO_UTF8(*PythonClass));

	if (!py_actor_class)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to find class %s in module %s"), *PythonClass, *PythonModule);
		return;
	}

	py_actor_instance = PyObject_CallObject(py_actor_class, NULL);
	if (!py_actor_instance)
	{
		unreal_engine_py_log_error();
		return;
	}

	py_uobject->py_proxy = py_actor_instance;

	PyObject_SetAttrString(py_actor_instance, (char*)"uobject", (PyObject *)py_uobject);


	if (!PyObject_HasAttrString(py_actor_instance, (char *)"tick") || PythonTickForceDisabled)
	{
		SetActorTickEnabled(false);
	}

	if (!PythonDisableAutoBinding)
		ue_autobind_events_for_pyclass(py_uobject, py_actor_instance);

	ue_bind_events_for_py_class_by_attribute(this, py_actor_instance);


	if (!PyObject_HasAttrString(py_actor_instance, (char *)"pre_initialize_components"))
		return;

	PyObject *pic_ret = PyObject_CallMethod(py_actor_instance, (char *)"pre_initialize_components", NULL);
	if (!pic_ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(pic_ret);
}


void APyActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!py_actor_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_actor_instance, (char *)"post_initialize_components"))
		return;

	PyObject *pic_ret = PyObject_CallMethod(py_actor_instance, (char *)"post_initialize_components", NULL);
	if (!pic_ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(pic_ret);
}

// Called when the game starts
void APyActor::BeginPlay()
{
	Super::BeginPlay();

	if (!py_actor_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_actor_instance, (char *)"begin_play"))
		return;

	PyObject *bp_ret = PyObject_CallMethod(py_actor_instance, (char *)"begin_play", NULL);
	if (!bp_ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(bp_ret);

}


// Called every frame
void APyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!py_actor_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_actor_instance, (char *)"tick"))
		return;

	PyObject *ret = PyObject_CallMethod(py_actor_instance, (char *)"tick", (char *)"f", DeltaTime);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}

void APyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!py_actor_instance)
		return;

	FScopePythonGIL gil;

	if (PyObject_HasAttrString(py_actor_instance, (char *)"end_play"))
	{
		PyObject *ep_ret = PyObject_CallMethod(py_actor_instance, (char *)"end_play", (char*)"i", (int)EndPlayReason);

		if (!ep_ret)
		{
			unreal_engine_py_log_error();
		}

		Py_XDECREF(ep_ret);
	}



	Super::EndPlay(EndPlayReason);

	// ...
}

void APyActor::CallPythonActorMethod(FString method_name, FString args)
{
	if (!py_actor_instance)
		return;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}

	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

bool APyActor::CallPythonActorMethodBool(FString method_name, FString args)
{
	if (!py_actor_instance)
		return false;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}
	if (!ret)
	{
		unreal_engine_py_log_error();
		return false;
	}

	if (PyObject_IsTrue(ret))
	{
		Py_DECREF(ret);
		return true;
	}

	Py_DECREF(ret);
	return false;
}

FString APyActor::CallPythonActorMethodString(FString method_name, FString args)
{
	if (!py_actor_instance)
		return FString();

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}
	if (!ret)
	{
		unreal_engine_py_log_error();
		return FString();
	}

	PyObject *py_str = PyObject_Str(ret);
	if (!py_str)
	{
		Py_DECREF(ret);
		return FString();
	}

	const char *str_ret = UEPyUnicode_AsUTF8(py_str);

	FString ret_fstring = FString(UTF8_TO_TCHAR(str_ret));

	Py_DECREF(py_str);

	return ret_fstring;
}


APyActor::~APyActor()
{
	FScopePythonGIL gil;

	Py_XDECREF(py_actor_instance);

#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Python AActor %p (mapped to %p) wrapper XDECREF'ed"), this, py_uobject ? py_uobject->py_proxy : nullptr);
#endif

	Py_XDECREF(py_uobject);
	FUnrealEnginePythonHouseKeeper::Get()->UnregisterPyUObject(this);


}
