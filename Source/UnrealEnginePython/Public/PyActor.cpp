#include "UnrealEnginePythonPrivatePCH.h"
#include "PyActor.h"

#include "PythonDelegate.h"

APyActor::APyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	PythonTickForceDisabled = false;
	PythonDisableAutoBinding = false;

}


// Called when the game starts
void APyActor::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (PythonModule.IsEmpty())
		return;

	FScopePythonGIL gil;

	py_uobject = ue_get_python_wrapper(this);
	if (!py_uobject) {
		unreal_engine_py_log_error();
		return;
	}

	PyObject *py_actor_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_actor_module) {
		unreal_engine_py_log_error();
		return;
	}

#if WITH_EDITOR
	// todo implement autoreload with a dictionary of module timestamps
	py_actor_module = PyImport_ReloadModule(py_actor_module);
	if (!py_actor_module) {
		unreal_engine_py_log_error();
		return;
	}
#endif

	if (PythonClass.IsEmpty())
		return;

	PyObject *py_actor_module_dict = PyModule_GetDict(py_actor_module);
	PyObject *py_actor_class = PyDict_GetItemString(py_actor_module_dict, TCHAR_TO_UTF8(*PythonClass));

	if (!py_actor_class) {
		UE_LOG(LogPython, Error, TEXT("Unable to find class %s in module %s"), *PythonClass, *PythonModule);
		return;
	}

	py_actor_instance = PyObject_CallObject(py_actor_class, NULL);
	if (!py_actor_instance) {
		unreal_engine_py_log_error();
		return;
	}

	py_uobject->py_proxy = py_actor_instance;

	PyObject_SetAttrString(py_actor_instance, (char*)"uobject", (PyObject *)py_uobject);


	if (!PyObject_HasAttrString(py_actor_instance, (char *)"tick") || PythonTickForceDisabled) {
		SetActorTickEnabled(false);
	}

	if (!PythonDisableAutoBinding)
		ue_autobind_events_for_pyclass(py_uobject, py_actor_instance);


	if (!PyObject_HasAttrString(py_actor_instance, (char *)"begin_play"))
		return;

	PyObject *bp_ret = PyObject_CallMethod(py_actor_instance, (char *)"begin_play", NULL);
	if (!bp_ret) {
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
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}


void APyActor::CallPythonActorMethod(FString method_name, FString args)
{
	if (!py_actor_instance)
		return;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty()) {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}

	if (!ret) {
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
	if (args.IsEmpty()) {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}
	if (!ret) {
		unreal_engine_py_log_error();
		return false;
	}

	if (PyObject_IsTrue(ret)) {
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
	if (args.IsEmpty()) {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}
	if (!ret) {
		unreal_engine_py_log_error();
		return FString();
	}

	PyObject *py_str = PyObject_Str(ret);
	if (!py_str) {
		Py_DECREF(ret);
		return FString();
	}

	char *str_ret = PyUnicode_AsUTF8(py_str);

	FString ret_fstring = FString(UTF8_TO_TCHAR(str_ret));

	Py_DECREF(py_str);

	return ret_fstring;
}


APyActor::~APyActor()
{
	FScopePythonGIL gil;

	ue_pydelegates_cleanup(py_uobject);

#if UEPY_MEMORY_DEBUG
	if (py_actor_instance && py_actor_instance->ob_refcnt != 1) {
		UE_LOG(LogPython, Error, TEXT("Inconsistent Python AActor wrapper refcnt = %d"), py_actor_instance->ob_refcnt);
	}
#endif
	Py_XDECREF(py_actor_instance);

#if UEPY_MEMORY_DEBUG
	UE_LOG(LogPython, Warning, TEXT("Python AActor %p (mapped to %p) wrapper XDECREF'ed"), this, py_uobject ? py_uobject->ue_object : nullptr);
#endif

	// this could trigger the distruction of the python/uobject mapper
	Py_XDECREF(py_uobject);
}
