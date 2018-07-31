
#include "PyUserWidget.h"
#include "PyNativeWidgetHost.h"

#include "PythonDelegate.h"

#include "Slate/UEPyFGeometry.h"
#include "Slate/UEPyFPaintContext.h"

#include "Widgets/Layout/SBox.h"
#include "UMGStyle.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"
#include "Slate/UEPyFKeyEvent.h"
#include "Slate/UEPyFPointerEvent.h"

void UPyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

    WidgetTree->ForEachWidget([&](UWidget* Widget) {
        if (Widget->IsA<UPyNativeWidgetHost>())
        {
            PyNativeWidgetHost = Cast<UPyNativeWidgetHost>(Widget);
        }
    });

	if (PythonModule.IsEmpty())
		return;

	FScopePythonGIL gil;

	py_uobject = ue_get_python_uobject(this);
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
#if ENGINE_MINOR_VERSION < 20
		bCanEverTick = false;
#else
		bHasScriptImplementedTick = false;
#endif

	if (PythonPaintForceDisabled)
#if ENGINE_MINOR_VERSION < 20
		bCanEverPaint = false;
#else
		bHasScriptImplementedPaint = false;
#endif
    
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

#if WITH_EDITOR

const FText UPyUserWidget::GetPaletteCategory()
{
    return NSLOCTEXT("Python", "Python", "Python");
}
#endif

void UPyUserWidget::SetSlateWidget(TSharedRef<SWidget> InContent)
{
    if (PyNativeWidgetHost.IsValid())
    {
        PyNativeWidgetHost->SetContent(InContent);
    }
}


void UPyUserWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> UPyUserWidget::RebuildWidget()
{
    return Super::RebuildWidget();
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

#if ENGINE_MINOR_VERSION < 20
void UPyUserWidget::NativePaint(FPaintContext & InContext) const
#else
int32 UPyUserWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
#endif
{

#if ENGINE_MINOR_VERSION >= 20
	FPaintContext InContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
#endif

	if (!py_user_widget_instance)
#if ENGINE_MINOR_VERSION >= 20
		return -1;
#else
		return;
#endif

	FScopePythonGIL gil;

	if (!PyObject_HasAttrString(py_user_widget_instance, (char *)"paint"))
#if ENGINE_MINOR_VERSION >= 20
		return -1;
#else
		return;
#endif

	PyObject *ret = PyObject_CallMethod(py_user_widget_instance, (char *)"paint", (char *)"O", py_ue_new_fpaint_context(InContext));
	if (!ret) {
		unreal_engine_py_log_error();
#if ENGINE_MINOR_VERSION >= 20
		return -1;
#else
		return;
#endif
	}
#if ENGINE_MINOR_VERSION >= 20
	int32 RetValue = -1;
	if (PyNumber_Check(ret))
	{
		PyObject *py_value = PyNumber_Long(ret);
		RetValue = PyLong_AsLong(py_value);
		Py_DECREF(py_value);
	}
#endif
	Py_DECREF(ret);
#if ENGINE_MINOR_VERSION >= 20
	return RetValue;
#endif
}

UPyUserWidget::UPyUserWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{}

UPyUserWidget::~UPyUserWidget()
{
	FScopePythonGIL gil;

	Py_XDECREF(py_user_widget_instance);

#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Python UUserWidget %p (mapped to %p) wrapper XDECREF'ed"), this, py_uobject ? py_uobject->py_proxy : nullptr);
#endif

	// this could trigger the distruction of the python/uobject mapper
	Py_XDECREF(py_uobject);
	FUnrealEnginePythonHouseKeeper::Get()->UnregisterPyUObject(this);
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
