#include "UnrealEnginePythonPrivatePCH.h"
#include "PyUserWidget.h"

#include "PythonDelegate.h"

#include "Slate/UEPyFGeometry.h"
#include "Slate/UEPyFPaintContext.h"

void UPyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PythonModule.IsEmpty())
		return;

	FScopePythonGIL gil;

	py_uobject = ue_get_python_wrapper(this);
	if (!py_uobject) {
		unreal_engine_py_log_error();
		return;
	}

	PyObject *py_user_widget_module = PyImport_ImportModule(TCHAR_TO_UTF8(*PythonModule));
	if (!py_user_widget_module) {
		unreal_engine_py_log_error();
		return;
	}

#if WITH_EDITOR
	// todo implement autoreload with a dictionary of module timestamps
	py_user_widget_module = PyImport_ReloadModule(py_user_widget_module);
	if (!py_user_widget_module) {
		unreal_engine_py_log_error();
		return;
	}
#endif

	if (PythonClass.IsEmpty())
		return;

	PyObject *py_user_widget_module_dict = PyModule_GetDict(py_user_widget_module);
	PyObject *py_user_widget_class = PyDict_GetItemString(py_user_widget_module_dict, TCHAR_TO_UTF8(*PythonClass));

	if (!py_user_widget_class) {
		UE_LOG(LogPython, Error, TEXT("Unable to find class %s in module %s"), *PythonClass, *PythonModule);
		return;
	}

	py_user_widget_instance = PyObject_CallObject(py_user_widget_class, NULL);
	if (!py_user_widget_instance) {
		unreal_engine_py_log_error();
		return;
	}

	py_uobject->py_proxy = py_user_widget_instance;

	PyObject_SetAttrString(py_user_widget_instance, (char*)"uobject", (PyObject *)py_uobject);

	if (PythonTickForceDisabled)
		bCanEverTick = false;

	if (PythonPaintForceDisabled)
		bCanEverPaint = false;

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

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"tick", (char *)"Of", py_ue_new_fgeometry(MyGeometry), InDeltaTime);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

FReply UPyUserWidget::NativeOnMouseButtonDown(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (!py_user_widget_instance)
		return FReply::Unhandled();

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"on_mouse_button_down"))
		return FReply::Unhandled();

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"on_mouse_button_down", (char *)"OO", py_ue_new_fgeometry(InGeometry), py_ue_new_fpointer_event(InMouseEvent));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}
	if (PyObject_IsTrue(ret)) {
		Py_DECREF(ret);
		return FReply::Handled();
	}
	Py_DECREF(ret);
	return FReply::Unhandled();
}

FReply UPyUserWidget::NativeOnMouseButtonUp(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (!py_user_widget_instance)
		return FReply::Unhandled();

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"on_mouse_button_up"))
		return FReply::Unhandled();

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"on_mouse_button_up", (char *)"OO", py_ue_new_fgeometry(InGeometry), py_ue_new_fpointer_event(InMouseEvent));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}
	if (PyObject_IsTrue(ret)) {
		Py_DECREF(ret);
		return FReply::Handled();
	}
	Py_DECREF(ret);
	return FReply::Unhandled();
}

FReply UPyUserWidget::NativeOnKeyUp(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent)
{
	Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	if (!py_user_widget_instance)
		return FReply::Unhandled();

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"on_key_up"))
		return FReply::Unhandled();

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"on_key_up", (char *)"OO", py_ue_new_fgeometry(InGeometry), py_ue_new_fkey_event(InKeyEvent));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}
	if (PyObject_IsTrue(ret)) {
		Py_DECREF(ret);
		return FReply::Handled();
	}
	Py_DECREF(ret);
	return FReply::Unhandled();
}

FReply UPyUserWidget::NativeOnKeyDown(const FGeometry & InGeometry, const FKeyEvent & InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (!py_user_widget_instance)
		return FReply::Unhandled();

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"on_key_down"))
		return FReply::Unhandled();

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"on_key_down", (char *)"OO", py_ue_new_fgeometry(InGeometry), py_ue_new_fkey_event(InKeyEvent));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}
	if (PyObject_IsTrue(ret)) {
		Py_DECREF(ret);
		return FReply::Handled();
	}
	Py_DECREF(ret);
	return FReply::Unhandled();
}

FReply UPyUserWidget::NativeOnMouseWheel(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);

	if (!py_user_widget_instance)
		return FReply::Unhandled();

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"on_mouse_wheel"))
		return FReply::Unhandled();

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"on_mouse_wheel", (char *)"OO", py_ue_new_fgeometry(InGeometry), py_ue_new_fpointer_event(InMouseEvent));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}
	if (PyObject_IsTrue(ret)) {
		Py_DECREF(ret);
		return FReply::Handled();
	}
	Py_DECREF(ret);
	return FReply::Unhandled();
}

FReply UPyUserWidget::NativeOnMouseMove(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (!py_user_widget_instance)
		return FReply::Unhandled();

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"on_mouse_move"))
		return FReply::Unhandled();

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"on_mouse_move", (char *)"OO", py_ue_new_fgeometry(InGeometry), py_ue_new_fpointer_event(InMouseEvent));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}
	if (PyObject_IsTrue(ret)) {
		Py_DECREF(ret);
		return FReply::Handled();
	}
	Py_DECREF(ret);
	return FReply::Unhandled();
}

bool UPyUserWidget::NativeIsInteractable() const
{
	if (!py_user_widget_instance)
		return false;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"is_interactable"))
		return false;

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"is_interactable", nullptr);
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

void UPyUserWidget::NativePaint(FPaintContext & InContext) const
{
	if (!py_user_widget_instance)
		return;

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"paint"))
		return;

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"paint", (char *)"O", py_ue_new_fpaint_context(InContext));
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

void UPyUserWidget::CallPythonUserWidgetMethod(FString method_name, FString args)
{
	if (!py_user_widget_instance)
		return;

	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (args.IsEmpty())
	{
		ret = PyObject_CallMethod(py_user_widget_instance, TCHAR_TO_UTF8(*method_name), NULL);
	}
	else
	{
		ret = PyObject_CallMethod(py_user_widget_instance, TCHAR_TO_UTF8(*method_name), (char *)"s", TCHAR_TO_UTF8(*args));
	}

	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}
