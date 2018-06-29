
#include "PyHUD.h"
#include "UEPyModule.h"
#include "PythonDelegate.h"

APyHUD::APyHUD()
{
	PrimaryActorTick.bCanEverTick = true;

	PythonTickForceDisabled = false;
	PythonDisableAutoBinding = false;

}


// Called when the game starts
void APyHUD::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (PythonModule.IsEmpty())
		return;

	FScopePythonGIL gil;

	py_uobject = ue_get_python_uobject(this);
	if (!py_uobject)
	{
		unreal_engine_py_log_error();
		return;
	}

	PyObject *py_hud_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_hud_module)
	{
		unreal_engine_py_log_error();
		return;
	}

#if WITH_EDITOR
	// todo implement autoreload with a dictionary of module timestamps
	py_hud_module = PyImport_ReloadModule(py_hud_module);
	if (!py_hud_module)
	{
		unreal_engine_py_log_error();
		return;
	}
#endif

	if (PythonClass.IsEmpty())
		return;

	PyObject *py_hud_module_dict = PyModule_GetDict(py_hud_module);
	PyObject *py_hud_class = PyDict_GetItemString(py_hud_module_dict, TCHAR_TO_UTF8(*PythonClass));

	if (!py_hud_class)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to find class %s in module %s"), *PythonClass, *PythonModule);
		return;
	}

	py_hud_instance = PyObject_CallObject(py_hud_class, NULL);
	if (!py_hud_instance)
	{
		unreal_engine_py_log_error();
		return;
	}

	py_uobject->py_proxy = py_hud_instance;

	PyObject_SetAttrString(py_hud_instance, (char*)"uobject", (PyObject *)py_uobject);


	if (!PyObject_HasAttrString(py_hud_instance, (char *)"tick") || PythonTickForceDisabled)
	{
		SetActorTickEnabled(false);
	}

	if (!PythonDisableAutoBinding)
		ue_autobind_events_for_pyclass(py_uobject, py_hud_instance);

	ue_bind_events_for_py_class_by_attribute(this, py_hud_instance);

	if (!PyObject_HasAttrString(py_hud_instance, (char *)"begin_play"))
		return;

	PyObject *bp_ret = PyObject_CallMethod(py_hud_instance, (char *)"begin_play", nullptr);
	if (!bp_ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(bp_ret);
}


// Called every frame
void APyHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!py_hud_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_hud_instance, (char *)"tick"))
		return;

	PyObject *ret = PyObject_CallMethod(py_hud_instance, (char *)"tick", (char *)"f", DeltaTime);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}

void APyHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!py_hud_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_hud_instance, (char *)"draw_hud"))
		return;

	PyObject *ret = PyObject_CallMethod(py_hud_instance, (char *)"draw_hud", nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}


void APyHUD::CallPythonHUDMethod(FString method_name, FString args)
{
	if (!py_hud_instance)
		return;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_hud_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_hud_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}

	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

bool APyHUD::CallPythonHUDMethodBool(FString method_name, FString args)
{
	if (!py_hud_instance)
		return false;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_hud_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_hud_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
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

FString APyHUD::CallPythonHUDMethodString(FString method_name, FString args)
{
	if (!py_hud_instance)
		return FString();

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_hud_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_hud_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
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


APyHUD::~APyHUD()
{
	FScopePythonGIL gil;


	Py_XDECREF(py_hud_instance);

#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Python AHUD %p (mapped to %p) wrapper XDECREF'ed"), this, py_uobject ? py_uobject->py_proxy : nullptr);
#endif

	// this could trigger the distruction of the python/uobject mapper
	Py_XDECREF(py_uobject);
	FUnrealEnginePythonHouseKeeper::Get()->UnregisterPyUObject(this);
}
