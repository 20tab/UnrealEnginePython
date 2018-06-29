
#include "PythonComponent.h"
#include "UEPyModule.h"

UPythonComponent::UPythonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
#if ENGINE_MINOR_VERSION < 14
	bWantsBeginPlay = true;
#endif
	PrimaryComponentTick.bCanEverTick = true;

	PythonTickForceDisabled = false;
	PythonDisableAutoBinding = false;
	PythonTickEnableGenerator = false;

	bWantsInitializeComponent = true;

	py_generator = nullptr;
}

void UPythonComponent::InitializePythonComponent()
{
	if (PythonModule.IsEmpty())
		return;

	FScopePythonGIL gil;

	py_uobject = ue_get_python_uobject(this);
	if (!py_uobject)
	{
		unreal_engine_py_log_error();
		return;
	}

	PyObject *py_component_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_component_module)
	{
		unreal_engine_py_log_error();
		return;
	}

#if WITH_EDITOR
	// todo implement autoreload with a dictionary of module timestamps
	py_component_module = PyImport_ReloadModule(py_component_module);
	if (!py_component_module)
	{
		unreal_engine_py_log_error();
		return;
	}
#endif

	if (PythonClass.IsEmpty())
		return;

	PyObject *py_component_module_dict = PyModule_GetDict(py_component_module);
	PyObject *py_component_class = PyDict_GetItemString(py_component_module_dict, TCHAR_TO_UTF8(*PythonClass));

	if (!py_component_class)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to find class %s in module %s"), *PythonClass, *PythonModule);
		return;
	}

	py_component_instance = PyObject_CallObject(py_component_class, NULL);
	if (!py_component_instance)
	{
		unreal_engine_py_log_error();
		return;
	}

	py_uobject->py_proxy = py_component_instance;

	PyObject_SetAttrString(py_component_instance, (char *)"uobject", (PyObject *)py_uobject);


	// disable ticking if no tick method is exposed
	if (!PyObject_HasAttrString(py_component_instance, (char *)"tick") || PythonTickForceDisabled)
	{
		PrimaryComponentTick.bCanEverTick = false;
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}

	if (!PythonDisableAutoBinding)
		ue_autobind_events_for_pyclass(py_uobject, py_component_instance);

	ue_bind_events_for_py_class_by_attribute(this, py_component_instance);

	if (!PyObject_HasAttrString(py_component_instance, (char *)"initialize_component"))
	{
		return;
	}

	PyObject *ic_ret = PyObject_CallMethod(py_component_instance, (char *)"initialize_component", NULL);
	if (!ic_ret)
	{
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ic_ret);

}

void UPythonComponent::InitializeComponent()
{
	Super::InitializeComponent();
	InitializePythonComponent();
}


// Called when the game starts
void UPythonComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_component_instance, (char *)"begin_play"))
	{
		return;
	}

	PyObject *bp_ret = PyObject_CallMethod(py_component_instance, (char *)"begin_play", NULL);
	if (!bp_ret)
	{
		unreal_engine_py_log_error();
	}
	Py_XDECREF(bp_ret);

}

void UPythonComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (PyObject_HasAttrString(py_component_instance, (char *)"end_play"))
	{
		PyObject *ep_ret = PyObject_CallMethod(py_component_instance, (char *)"end_play", (char*)"i", (int)EndPlayReason);

		if (!ep_ret)
		{
			unreal_engine_py_log_error();
		}

		Py_XDECREF(ep_ret);
	}

	Super::EndPlay(EndPlayReason);

	// ...
}

// Called every frame
void UPythonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (PythonTickEnableGenerator && py_generator)
	{
		PyObject *ret = PyIter_Next(py_generator);
		if (!ret)
		{
			if (PyErr_Occurred())
			{
				unreal_engine_py_log_error();
			}
			Py_DECREF(py_generator);
			py_generator = nullptr;
			return;
		}
		Py_DECREF(ret);
		return;
	}

	// no need to check for method availability, we did it in component initialization

	PyObject *ret = PyObject_CallMethod(py_component_instance, (char *)"tick", (char *)"f", DeltaTime);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}

	if (PythonTickEnableGenerator)
	{
		py_generator = PyObject_GetIter(ret);
		if (!py_generator)
		{
			UE_LOG(LogPython, Error, TEXT("tick is not a python generator"));
		}
	}
	Py_DECREF(ret);

}

void UPythonComponent::CallPythonComponentMethod(FString method_name, FString args)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}

	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonComponent::SetPythonAttrObject(FString attr, UObject *object)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	ue_PyUObject *py_obj = ue_get_python_uobject(object);
	if (!py_obj)
	{
		PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		unreal_engine_py_log_error();
		return;
	}

	if (PyObject_SetAttrString(py_component_instance, TCHAR_TO_UTF8(*attr), (PyObject *)py_obj) < 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to set attribute %s"), *attr);
	}
}

void UPythonComponent::SetPythonAttrString(FString attr, FString s)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (PyObject_SetAttrString(py_component_instance, TCHAR_TO_UTF8(*attr), PyUnicode_FromString(TCHAR_TO_UTF8(*s))) < 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to set attribute %s"), *attr);
	}
}

void UPythonComponent::SetPythonAttrFloat(FString attr, float f)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (PyObject_SetAttrString(py_component_instance, TCHAR_TO_UTF8(*attr), PyFloat_FromDouble(f)) < 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to set attribute %s"), *attr);
	}
}



void UPythonComponent::SetPythonAttrInt(FString attr, int n)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (PyObject_SetAttrString(py_component_instance, TCHAR_TO_UTF8(*attr), PyLong_FromLong(n)) < 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to set attribute %s"), *attr);
	}
}

void UPythonComponent::SetPythonAttrVector(FString attr, FVector vec)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (PyObject_SetAttrString(py_component_instance, TCHAR_TO_UTF8(*attr), py_ue_new_fvector(vec)) < 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to set attribute %s"), *attr);
	}
}

void UPythonComponent::SetPythonAttrRotator(FString attr, FRotator rot)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	if (PyObject_SetAttrString(py_component_instance, TCHAR_TO_UTF8(*attr), py_ue_new_frotator(rot)) < 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to set attribute %s"), *attr);
	}
}

void UPythonComponent::SetPythonAttrBool(FString attr, bool b)
{
	if (!py_component_instance)
		return;

	FScopePythonGIL gil;

	PyObject *py_bool = Py_False;
	if (b)
	{
		py_bool = Py_True;
	}

	if (PyObject_SetAttrString(py_component_instance, TCHAR_TO_UTF8(*attr), py_bool) < 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to set attribute %s"), *attr);
	}
}

bool UPythonComponent::CallPythonComponentMethodBool(FString method_name, FString args)
{
	if (!py_component_instance)
		return false;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
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

float UPythonComponent::CallPythonComponentMethodFloat(FString method_name, FString args)
{
	if (!py_component_instance)
		return 0;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}


	if (!ret)
	{
		unreal_engine_py_log_error();
		return 0;
	}

	float value = 0;

	if (PyNumber_Check(ret))
	{
		PyObject *py_value = PyNumber_Float(ret);
		value = PyFloat_AsDouble(py_value);
		Py_DECREF(py_value);
	}

	Py_DECREF(ret);
	return value;
}

int UPythonComponent::CallPythonComponentMethodInt(FString method_name, FString args)
{
	if (!py_component_instance)
		return 0;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}


	if (!ret)
	{
		unreal_engine_py_log_error();
		return 0;
	}

	int value = 0;

	if (PyNumber_Check(ret))
	{
		PyObject *py_value = PyNumber_Long(ret);
		value = PyLong_AsLong(py_value);
		Py_DECREF(py_value);
	}

	Py_DECREF(ret);
	return value;
}

FString UPythonComponent::CallPythonComponentMethodString(FString method_name, FString args)
{
	if (!py_component_instance)
		return FString();

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
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

UObject *UPythonComponent::CallPythonComponentMethodObject(FString method_name, UObject *arg)
{
	if (!py_component_instance)
		return nullptr;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (!arg)
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		PyObject *py_arg_uobject = (PyObject *)ue_get_python_uobject(arg);
		if (!py_arg_uobject)
		{
			unreal_engine_py_log_error();
			return nullptr;
		}
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"O", py_arg_uobject);
	}

	if (!ret)
	{
		unreal_engine_py_log_error();
		return nullptr;
	}

	if (ue_is_pyuobject(ret))
	{
		ue_PyUObject *py_obj = (ue_PyUObject *)ret;
		return py_obj->ue_object;
	}
	return nullptr;
}

#if ENGINE_MINOR_VERSION >= 15
TMap<FString, FString> UPythonComponent::CallPythonComponentMethodMap(FString method_name, FString args)
{
	TMap<FString, FString> output_map;

	if (!py_component_instance)
		return output_map;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}

	if (!ret)
	{
		unreal_engine_py_log_error();
		return output_map;
	}

	if (!PyDict_Check(ret))
	{
		UE_LOG(LogPython, Error, TEXT("return value is not a dict"));
		return output_map;
	}

	PyObject *py_keys = PyDict_Keys(ret);
	Py_ssize_t len = PyList_Size(py_keys);

	for (Py_ssize_t i = 0; i < len; i++)
	{
		PyObject *py_key = PyList_GetItem(py_keys, i);
		PyObject *py_str_key = PyObject_Str(py_key);
		PyObject *py_str_value = PyObject_Str(PyDict_GetItem(ret, py_key));

		if (!py_str_key || !py_str_value)
		{
			Py_DECREF(ret);
			return output_map;
		}

		const char *str_key = UEPyUnicode_AsUTF8(py_str_key);
		const char *str_value = UEPyUnicode_AsUTF8(py_str_value);

		FString ret_fstring_key = FString(UTF8_TO_TCHAR(str_key));
		FString ret_fstring_value = FString(UTF8_TO_TCHAR(str_value));

		output_map.Add(ret_fstring_key, ret_fstring_value);

		Py_DECREF(py_str_key);
		Py_DECREF(py_str_value);
	}

	Py_DECREF(ret);

	return output_map;
}
#endif

void UPythonComponent::CallPythonComponentMethodStringArray(FString method_name, FString args, TArray<FString> &output_strings)
{
	if (!py_component_instance)
		return;

	output_strings.Empty();

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_component_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}

	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}

	if (!PyList_Check(ret))
	{
		UE_LOG(LogPython, Error, TEXT("return value is not a list"));
		return;
	}

	Py_ssize_t len = PyList_Size(ret);

	for (Py_ssize_t i = 0; i < len; i++)
	{
		PyObject *py_str = PyObject_Str(PyList_GetItem(ret, i));
		if (!py_str)
		{
			Py_DECREF(ret);
			return;
		}

		const char *str_ret = UEPyUnicode_AsUTF8(py_str);

		FString ret_fstring = FString(UTF8_TO_TCHAR(str_ret));

		output_strings.Add(ret_fstring);

		Py_DECREF(py_str);
	}

	Py_DECREF(ret);
}


UPythonComponent::~UPythonComponent()
{
	FScopePythonGIL gil;

#
	Py_XDECREF(py_component_instance);

#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Python UActorComponent %p (mapped to %p) wrapper XDECREF'ed"), this, py_uobject ? py_uobject->py_proxy : nullptr);
#endif

	// this could trigger the distruction of the python/uobject mapper
	Py_XDECREF(py_uobject);
	FUnrealEnginePythonHouseKeeper::Get()->UnregisterPyUObject(this);
}
