#include "UnrealEnginePythonPrivatePCH.h"
#include "PyActor.h"



APyActor::APyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	PythonTickForceDisabled = false;
	PythonDisableAutoBinding = false;

	// pre-generate PyUObject (for performance)
	ue_get_python_wrapper(this);
}


// Called when the game starts
void APyActor::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (PythonModule.IsEmpty())
		return;

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
		unreal_engine_py_log_error();
		return;
	}

	py_actor_instance = PyObject_CallObject(py_actor_class, NULL);
	if (!py_actor_instance) {
		unreal_engine_py_log_error();
		return;
	}

	PyObject *py_obj = (PyObject *) ue_get_python_wrapper(this);

	if (py_obj) {
		PyObject_SetAttrString(py_actor_instance, "uobject", py_obj);
	}
	else {
		UE_LOG(LogPython, Error, TEXT("Unable to set 'uobject' field in actor wrapper class"));
	}

	if (!PyObject_HasAttrString(py_actor_instance, "tick") || PythonTickForceDisabled) {
		SetActorTickEnabled(false);
	}

	if (!PythonDisableAutoBinding)
		ue_autobind_events_for_class(this, py_actor_instance);

	if (!PyObject_HasAttrString(py_actor_instance, "begin_play"))
		return;

	PyObject *bp_ret = PyObject_CallMethod(py_actor_instance, "begin_play", NULL);
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

	if (!PyObject_HasAttrString(py_actor_instance, "tick"))
		return;

	PyObject *ret = PyObject_CallMethod(py_actor_instance, "tick", "f", DeltaTime);
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

	PyObject *ret = nullptr;
	if (args.IsEmpty()) {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), "s", TCHAR_TO_UTF8(*args));
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

	PyObject *ret = nullptr;
	if (args.IsEmpty()) {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), "s", TCHAR_TO_UTF8(*args));
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

	PyObject *ret = nullptr;
	if (args.IsEmpty()) {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else {
		ret = PyObject_CallMethod(py_actor_instance, TCHAR_TO_UTF8(*method_name), "s", TCHAR_TO_UTF8(*args));
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
	Py_XDECREF(py_actor_instance);
	UE_LOG(LogPython, Warning, TEXT("Python AActor wrapper XDECREF'ed"));
}