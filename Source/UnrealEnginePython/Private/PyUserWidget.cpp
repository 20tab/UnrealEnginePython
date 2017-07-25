#include "UnrealEnginePythonPrivatePCH.h"
#include "PyUserWidget.h"

#include "PythonDelegate.h"

bool UPyUserWidget::Initialize()
{
	if (!Super::Initialize())
		return false;

	if (PythonModule.IsEmpty())
		return false;

	FScopePythonGIL gil;

	py_uobject = ue_get_python_wrapper(this);
	if (!py_uobject) {
		unreal_engine_py_log_error();
		return false;
	}

	PyObject *py_user_widget_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_user_widget_module) {
		unreal_engine_py_log_error();
		return false;
	}

#if WITH_EDITOR
	// todo implement autoreload with a dictionary of module timestamps
	py_user_widget_module = PyImport_ReloadModule(py_user_widget_module);
	if (!py_user_widget_module) {
		unreal_engine_py_log_error();
		return false;
	}
#endif

	if (PythonClass.IsEmpty())
		return false;

	PyObject *py_user_widget_module_dict = PyModule_GetDict(py_user_widget_module);
	PyObject *py_user_widget_class = PyDict_GetItemString(py_user_widget_module_dict, TCHAR_TO_UTF8(*PythonClass));

	if (!py_user_widget_class) {
		UE_LOG(LogPython, Error, TEXT("Unable to find class %s in module %s"), *PythonClass, *PythonModule);
		return false;
	}

	py_user_widget_instance = PyObject_CallObject(py_user_widget_class, NULL);
	if (!py_user_widget_instance) {
		unreal_engine_py_log_error();
		return false;
	}

	py_uobject->py_proxy = py_user_widget_instance;

	PyObject_SetAttrString(py_user_widget_instance, (char*)"uobject", (PyObject *)py_uobject);

	return true;
}


void UPyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!py_user_widget_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"construct"))
		return;

	PyObject *bp_ret = PyObject_CallMethod(py_user_widget_instance, (char *)"construct", NULL);
	if (!bp_ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(bp_ret);
}

void UPyUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (!py_user_widget_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"destruct"))
		return;

	PyObject *bp_ret = PyObject_CallMethod(py_user_widget_instance, (char *)"destruct", NULL);
	if (!bp_ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(bp_ret);
}

// Called every frame
void UPyUserWidget::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!py_user_widget_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"tick"))
		return;

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"tick", (char *)"Of", py_ue_new_uscriptstruct(FGeometry::StaticStruct(), (uint8 *) &MyGeometry), InDeltaTime);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);

}

UPyUserWidget::~UPyUserWidget()
{
	FScopePythonGIL gil;

	ue_pydelegates_cleanup(py_uobject);

#if defined(UEPY_MEMORY_DEBUG)
	if (py_user_widget_instance && py_user_widget_instance->ob_refcnt != 1) {
		UE_LOG(LogPython, Error, TEXT("Inconsistent Python UUserWidget wrapper refcnt = %d"), py_user_widget_instance->ob_refcnt);
	}
#endif
	Py_XDECREF(py_user_widget_instance);

#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Python UUserWidget %p (mapped to %p) wrapper XDECREF'ed"), this, py_uobject ? py_uobject->ue_object : nullptr);
#endif

	// this could trigger the distruction of the python/uobject mapper
	Py_XDECREF(py_uobject);
}
