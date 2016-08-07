#include "UnrealEnginePythonPrivatePCH.h"
#include "PythonComponent.h"



UPythonComponent::UPyActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// pre-generate PyUObject (for performance)
	ue_get_python_wrapper(this);
}


// Called when the game starts
void UPythonComponent::BeginPlay()
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

	PyObject *py_component_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_component_module) {
		unreal_engine_py_log_error();
		return;
	}

	// todo implement autoreload with a dictionary of mule timestamps
	py_component_module = PyImport_ReloadModule(py_component_module);
	if (!py_component_module) {
		unreal_engine_py_log_error();
		return;
	}

	if (PythonClass.IsEmpty())
		return;

	PyObject *py_component_module_dict = PyModule_GetDict(py_component_module);
	PyObject *py_component_class = PyDict_GetItemString(py_component_module_dict, TCHAR_TO_UTF8(*PythonClass));

	if (!py_component_class) {
		unreal_engine_py_log_error();
		return;
	}

	py_component_instance = PyObject_CallObject(py_component_class, NULL);
	if (!py_component_instance) {
		unreal_engine_py_log_error();
		return;
	}

	PyObject *py_obj = (PyObject *) ue_get_python_wrapper(this);

	if (py_obj) {
		PyObject_SetAttrString(py_component_instance, "uobject", py_obj);
	}
	else {
		UE_LOG(LogPython, Error, TEXT("Unable to set 'uobject' field in component wrapper class"));
	}

	PyObject *bp_ret = PyObject_CallMethod(py_component_instance, "begin_play", NULL);
	if (!bp_ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(bp_ret);
}


// Called every frame
void UPythonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!py_component_instance)
		return;

	PyObject *ret = PyObject_CallMethod(py_component_instance, "tick", "f", DeltaTime);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}

UPythonComponent::~UPyActorComponent()
{
	Py_XDECREF(py_component_instance);
	UE_LOG(LogPython, Error, TEXT("Python UActorComponent wrapper XDECREF'ed"));
}