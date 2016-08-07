#include "UnrealEnginePythonPrivatePCH.h"
#include "PyActor.h"



APyActor::APyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// pre-generate PyUObject (for performance)
	ue_get_python_wrapper(this);
}


// Called when the game starts
void APyActor::BeginPlay()
{
	Super::BeginPlay();

	// ...

	const TCHAR *blob = *PythonCode;

	int ret = PyRun_SimpleString(TCHAR_TO_UTF8(blob));

	UE_LOG(LogPython, Warning, TEXT("Python ret = %d"), ret);

	if (ret) {
		unreal_engine_py_log_error();
	}

	if (PythonModule.IsEmpty())
		return;

	PyObject *py_actor_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_actor_module) {
		unreal_engine_py_log_error();
		return;
	}

	// todo implement autoreload with a dictionary of module timestamps
	py_actor_module = PyImport_ReloadModule(py_actor_module);
	if (!py_actor_module) {
		unreal_engine_py_log_error();
		return;
	}

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

	PyObject *ret = PyObject_CallMethod(py_actor_instance, "tick", "f", DeltaTime);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}

APyActor::~APyActor()
{
	Py_XDECREF(py_actor_instance);
	UE_LOG(LogPython, Error, TEXT("Python AActor wrapper XDECREF'ed"));
}