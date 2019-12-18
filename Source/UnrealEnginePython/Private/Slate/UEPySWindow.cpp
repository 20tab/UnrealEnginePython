
#include "UEPySWindow.h"

#if WITH_EDITOR
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#endif

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"

static PyObject *py_ue_swindow_set_title(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	char *title;
	if (!PyArg_ParseTuple(args, "s:set_title", &title))
	{
		return nullptr;
	}

	py_SWindow->SetTitle(FText::FromString(UTF8_TO_TCHAR(title)));

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_swindow_get_title(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	
	const char *title = TCHAR_TO_UTF8(*py_SWindow->GetTitle().ToString());

	return PyUnicode_FromString(title);
}

static PyObject *py_ue_swindow_resize(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	int width;
	int height;
	if (!PyArg_ParseTuple(args, "ii:resize", &width, &height))
	{
		return NULL;
	}


	py_SWindow->Resize(FVector2D(width, height));

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_swindow_minimize(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	py_SWindow->Minimize();

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_swindow_set_content(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	PyObject *py_content;
	if (!PyArg_ParseTuple(args, "O:set_content", &py_content))
	{
		return nullptr;
	}

	TSharedPtr<SWidget> Content = py_ue_is_swidget<SWidget>(py_content);
	if (!Content.IsValid())
	{
		return nullptr;
	}

	py_SWindow->SetContent(Content.ToSharedRef());

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_swindow_set_sizing_rule(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	int rule;
	if (!PyArg_ParseTuple(args, "i:set_sizing_rule", &rule))
	{
		return nullptr;
	}


#if ENGINE_MINOR_VERSION > 15
	py_SWindow->SetSizingRule((ESizingRule)rule);
#else
	py_SWindow->SetSizingRule((ESizingRule::Type)rule);
#endif

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_swindow_get_handle(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	return PyLong_FromLongLong((long long)py_SWindow->GetNativeWindow()->GetOSWindowHandle());
}

static PyObject *py_ue_swindow_set_as_owner(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	long long window_ptr;
	if (!PyArg_ParseTuple(args, "L:set_as_owner", &window_ptr))
	{
		return nullptr;
	}
	void *whnd = py_SWindow->GetNativeWindow()->GetOSWindowHandle();
#if PLATFORM_WINDOWS
	SetWindowLongPtr((HWND)window_ptr, GWLP_HWNDPARENT, (LONG_PTR)whnd);
#endif
	Py_RETURN_NONE;
}

static PyObject *py_ue_swindow_request_destroy(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	py_SWindow->RequestDestroyWindow();

	Py_RETURN_NONE;
}

static PyObject *py_ue_swindow_bring_to_front(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);

	PyObject *py_force = nullptr;
	
	if (!PyArg_ParseTuple(args, "|O:set_as_owner", &py_force))
	{
		return nullptr;
	}

	bool bForce = false;
	if (py_force && PyObject_IsTrue(py_force))
		bForce = true;

	py_SWindow->BringToFront(bForce);
	
	Py_RETURN_NONE;
}

#if WITH_EDITOR
static PyObject *py_ue_swindow_add_modal(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	TSharedPtr<SWindow> parent_window;
	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		parent_window = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame").GetParentWindow();
	}
	Py_BEGIN_ALLOW_THREADS;
	FSlateApplication::Get().AddModalWindow(StaticCastSharedRef<SWindow>(py_SWindow->AsShared()), parent_window, false);
	Py_END_ALLOW_THREADS;
	Py_RETURN_NONE;
}
#endif

static PyObject *py_ue_swindow_add_child(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:add_child", &py_obj))
	{
		return NULL;
	}

	ue_PySWindow *py_swindow_child = py_ue_is_swindow(py_obj);
	if (!py_swindow_child)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a SWindow");
	}

	FSlateApplication::Get().AddWindowAsNativeChild(
		StaticCastSharedRef<SWindow>(py_swindow_child->s_compound_widget.s_widget.Widget),
		py_SWindow);

	Py_RETURN_NONE;
}

static PyObject *py_ue_swindow_get_child_windows(ue_PySWindow *self, PyObject * args)
{
	ue_py_slate_cast(SWindow);
	TArray<TSharedRef<SWindow>>& ChildWindows = py_SWindow->GetChildWindows();
	PyObject *py_list = PyList_New(0);
	for (TSharedRef<SWindow> ChildWindow : ChildWindows)
	{
		PyList_Append(py_list, (PyObject *)py_ue_new_swindow(ChildWindow));
	}

	return py_list;
}

static PyMethodDef ue_PySWindow_methods[] = {
	{ "get_child_windows", (PyCFunction)py_ue_swindow_get_child_windows, METH_VARARGS, "" },
	{ "set_title", (PyCFunction)py_ue_swindow_set_title, METH_VARARGS, "" },
	{ "get_title", (PyCFunction)py_ue_swindow_get_title, METH_VARARGS, "" },
	{ "set_sizing_rule", (PyCFunction)py_ue_swindow_set_sizing_rule, METH_VARARGS, "" },
	{ "minimize", (PyCFunction)py_ue_swindow_minimize, METH_VARARGS, "" },
	{ "resize", (PyCFunction)py_ue_swindow_resize, METH_VARARGS, "" },
	{ "set_client_size", (PyCFunction)py_ue_swindow_resize, METH_VARARGS, "" },
	{ "set_content", (PyCFunction)py_ue_swindow_set_content, METH_VARARGS, "" },
	{ "get_handle", (PyCFunction)py_ue_swindow_get_handle, METH_VARARGS, "" },
	{ "set_as_owner", (PyCFunction)py_ue_swindow_set_as_owner, METH_VARARGS, "" },
	{ "request_destroy", (PyCFunction)py_ue_swindow_request_destroy, METH_VARARGS, "" },
    { "bring_to_front", (PyCFunction)py_ue_swindow_bring_to_front, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "add_modal", (PyCFunction)py_ue_swindow_add_modal, METH_VARARGS, "" },
#endif
	{ "add_child", (PyCFunction)py_ue_swindow_add_child, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


DECLARE_UE_PY_SLATE_WIDGET(SWindow);

static int ue_py_swindow_init(ue_PySWindow *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SWindow);
#if ENGINE_MINOR_VERSION > 15
	ue_py_slate_farguments_optional_enum("activation_policy", ActivationPolicy, EWindowActivationPolicy);
#else
	ue_py_slate_farguments_optional_bool("activate_when_first_shown", ActivateWhenFirstShown);
#endif
#if ENGINE_MINOR_VERSION > 15
	ue_py_slate_farguments_optional_enum("auto_center", AutoCenter, EAutoCenter);
#else
	ue_py_slate_farguments_optional_enum("auto_center", AutoCenter, EAutoCenter::Type);
#endif
	ue_py_slate_farguments_optional_bool("drag_anywhere", bDragAnywhere);
	ue_py_slate_farguments_optional_fvector2d("client_size", ClientSize);
	ue_py_slate_farguments_optional_bool("create_title_bar", CreateTitleBar);
	ue_py_slate_farguments_optional_bool("focus_when_first_shown", FocusWhenFirstShown);
	ue_py_slate_farguments_optional_bool("has_close_button", HasCloseButton);
	ue_py_slate_farguments_optional_float("initial_opacity", InitialOpacity);
	ue_py_slate_farguments_optional_bool("is_initially_maximized", IsInitiallyMaximized);
	ue_py_slate_farguments_optional_bool("is_initially_minimized", IsInitiallyMinimized);
	ue_py_slate_farguments_optional_bool("is_popup_window", IsPopupWindow);
	ue_py_slate_farguments_optional_bool("is_topmost_window", IsTopmostWindow);
	ue_py_slate_farguments_optional_struct("layout_border", LayoutBorder, FMargin);
	ue_py_slate_farguments_optional_float("max_height", MaxHeight);
	ue_py_slate_farguments_optional_float("max_width", MaxWidth);
	ue_py_slate_farguments_optional_float("min_height", MinHeight);
	ue_py_slate_farguments_optional_float("min_width", MinWidth);
	ue_py_slate_farguments_optional_bool("sane_window_placement", SaneWindowPlacement);
	ue_py_slate_farguments_optional_fvector2d("screen_position", ScreenPosition);
#if ENGINE_MINOR_VERSION > 12
	ue_py_slate_farguments_optional_bool("should_preserve_aspect_ratio", ShouldPreserveAspectRatio);
#endif
#if ENGINE_MINOR_VERSION > 15
	ue_py_slate_farguments_optional_enum("sizing_rule", SizingRule, ESizingRule);
#else
	ue_py_slate_farguments_optional_enum("sizing_rule", SizingRule, ESizingRule::Type);
#endif
	ue_py_slate_farguments_optional_struct_ptr("style", Style, FWindowStyle);
	ue_py_slate_farguments_optional_bool("supports_maximize", SupportsMaximize);
	ue_py_slate_farguments_text("title", Title);
	ue_py_slate_farguments_optional_enum("type", Type, EWindowType);
	ue_py_slate_farguments_optional_bool("use_os_window_border", UseOSWindowBorder);
	ue_py_slate_farguments_optional_struct("user_resize_border", UserResizeBorder, FMargin);

	ue_py_snew(SWindow);

	ue_py_slate_cast(SWindow);

#if WITH_EDITOR
	// is it a modal window ?
	PyObject *is_modal = ue_py_dict_get_item(kwargs, "modal");
	if (is_modal && PyObject_IsTrue(is_modal))
	{
		return 0;
	}
#endif

	PyObject *on_closed = ue_py_dict_get_item(kwargs, "on_closed");
	if (on_closed && PyCallable_Check(on_closed))
	{
		FOnWindowClosed handler;
		TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewSlateDelegate(self->s_compound_widget.s_widget.Widget, on_closed);
		handler.BindSP(py_delegate, &FPythonSlateDelegate::OnWindowClosed);

		py_SWindow->SetOnWindowClosed(handler);
	}

	// is it a child ?
	PyObject *is_child = ue_py_dict_get_item(kwargs, "child");
	if (!(is_child && PyObject_IsTrue(is_child)))
	{
		FSlateApplication::Get().AddWindow(py_SWindow, true);
	}

	return 0;
}

PyNumberMethods ue_PySWindow_number_methods;

static PyObject *ue_py_swindow_lshift(ue_PySWindow *self, PyObject *value)
{
	ue_py_slate_cast(SWindow);

	TSharedPtr<SWidget> Content = py_ue_is_swidget<SWidget>(value);
	if (!Content.IsValid())
	{
		return nullptr;
	}

	py_SWindow->SetContent(Content.ToSharedRef());

	Py_RETURN_SLATE_SELF;
}


void ue_python_init_swindow(PyObject *ue_module)
{

	ue_PySWindowType.tp_init = (initproc)ue_py_swindow_init;
	ue_PySWindowType.tp_call = (ternaryfunc)py_ue_swindow_set_content;

	memset(&ue_PySWindow_number_methods, 0, sizeof(PyNumberMethods));
	ue_PySWindowType.tp_as_number = &ue_PySWindow_number_methods;
	ue_PySWindow_number_methods.nb_lshift = (binaryfunc)ue_py_swindow_lshift;

	ue_PySWindowType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySWindowType) < 0)
		return;

	Py_INCREF(&ue_PySWindowType);
	PyModule_AddObject(ue_module, "SWindow", (PyObject *)&ue_PySWindowType);
}

ue_PySWindow *py_ue_is_swindow(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PySWindowType))
		return nullptr;
	return (ue_PySWindow *)obj;
}

ue_PySWindow *py_ue_new_swindow(TSharedRef<SWindow> s_window)
{
	ue_PySWindow *ret = (ue_PySWindow *)PyObject_New(ue_PySWindow, &ue_PySWindowType);

	new(&ret->s_compound_widget.s_widget.Widget) TSharedRef<SWindow>(s_window);
	ret->s_compound_widget.s_widget.weakreflist = nullptr;
	return ret;
}