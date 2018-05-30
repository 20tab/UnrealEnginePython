
#include "UEPyFSlateApplication.h"
#include "Framework/Application/MenuStack.h"
#include "Layout/WidgetPath.h"
#include "Slate/UEPySWidget.h"


static PyObject *py_ue_get_average_delta_time(PyObject *cls, PyObject * args)
{
	return PyFloat_FromDouble(FSlateApplication::Get().GetAverageDeltaTime());
}

static PyObject *py_ue_get_delta_time(PyObject *cls, PyObject * args)
{
	return PyFloat_FromDouble(FSlateApplication::Get().GetDeltaTime());
}

static PyObject *py_ue_get_modifier_keys(PyObject *cls, PyObject * args)
{
	return py_ue_new_fmodifier_keys_state(FSlateApplication::Get().GetModifierKeys());
}

static PyObject *py_ue_get_cursor_radius(PyObject *cls, PyObject * args)
{
	return PyFloat_FromDouble(FSlateApplication::Get().GetCursorRadius());
}

static PyObject *py_ue_goto_line_in_source(PyObject *cls, PyObject * args)
{
	char *filename;
	int line_number;
	if (!PyArg_ParseTuple(args, "si:goto_line_in_source", &filename, &line_number))
	{
		return nullptr;
	}

	FSlateApplication::Get().GotoLineInSource(FString(UTF8_TO_TCHAR(filename)), line_number);

	Py_RETURN_NONE;
}

static PyObject *py_ue_is_gamepad_attached(PyObject *cls, PyObject * args)
{
	bool bAttached = FSlateApplication::Get().IsGamepadAttached();
	if (bAttached)
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_is_mouse_attached(PyObject *cls, PyObject * args)
{
	bool bAttached = FSlateApplication::Get().IsMouseAttached();
	if (bAttached)
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_set_all_user_focus(PyObject *cls, PyObject * args)
{
	PyObject *py_widget;
	int focus_cause = (int)EFocusCause::SetDirectly;

	if (!PyArg_ParseTuple(args, "O|i:set_all_user_focus", &py_widget, &focus_cause))
	{
		return nullptr;
	}

	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(py_widget);
	if (!Widget.IsValid())
	{
		return nullptr;
	}

	FSlateApplication::Get().SetAllUserFocus(Widget, (EFocusCause)focus_cause);

	Py_RETURN_NONE;
}

static PyObject *py_ue_set_application_scale(PyObject *cls, PyObject * args)
{
	float scale;
	if (!PyArg_ParseTuple(args, "f:set_application_scale", &scale))
	{
		return nullptr;
	}

	FSlateApplication::Get().SetApplicationScale(scale);

	Py_RETURN_NONE;
}

static PyObject *py_ue_set_cursor_pos(PyObject *cls, PyObject * args)
{
	float x;
	float y;
	if (!PyArg_ParseTuple(args, "(ff):set_cursor_pos", &x, &y))
	{
		return nullptr;
	}

	FSlateApplication::Get().SetCursorPos(FVector2D(x, y));

	Py_RETURN_NONE;
}

static PyObject *py_ue_process_key_down_event(PyObject *cls, PyObject * args)
{
	PyObject *py_event;
	if (!PyArg_ParseTuple(args, "O:process_key_down_event", &py_event))
	{
		return nullptr;
	}

	FKeyEvent *InKeyEvent = ue_py_check_struct<FKeyEvent>(py_event);
	if (!InKeyEvent)
	{
		ue_PyFKeyEvent *py_fkey_event = py_ue_is_fkey_event(py_event);
		if (!py_fkey_event)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FKeyEvent");
		}
		InKeyEvent = &py_fkey_event->key_event;
	}

	if (FSlateApplication::Get().ProcessKeyDownEvent(*InKeyEvent))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyObject *py_ue_process_key_char_event(PyObject *cls, PyObject * args)
{
	PyObject *py_event;
	if (!PyArg_ParseTuple(args, "O:process_key_char_event", &py_event))
	{
		return nullptr;
	}

	FCharacterEvent *InCharEvent = ue_py_check_struct<FCharacterEvent>(py_event);
	if (!InCharEvent)
	{
		ue_PyFCharacterEvent *py_fchar_event = py_ue_is_fcharacter_event(py_event);
		if (!py_fchar_event)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FCharacterEvent");
		}
		InCharEvent = &py_fchar_event->character_event;
	}

	if (FSlateApplication::Get().ProcessKeyCharEvent(*InCharEvent))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyObject *py_ue_push_menu(PyObject *cls, PyObject * args)
{
	PyObject *py_parent_widget;
	PyObject *py_menu_widget;
	float x;
	float y;
	int menuSlideDirection = (int)FPopupTransitionEffect::ESlideDirection::None;

	if (!PyArg_ParseTuple(args, "OOffi:push_menu", &py_parent_widget, &py_menu_widget, &x, &y, &menuSlideDirection))
	{
		return nullptr;
	}
	// Parse cursor position as a blueprint struct
    TSharedPtr<SWidget> parentWidget = py_ue_is_swidget<SWidget>(py_parent_widget);
    if (!parentWidget.IsValid())
    { return nullptr; }

    TSharedPtr<SWidget> menuWidget = py_ue_is_swidget<SWidget>(py_menu_widget);
    if (!menuWidget.IsValid())
    { return nullptr; }

	FVector2D CursorPos = FVector2D(x, y);
	
	FSlateApplication::Get().PushMenu(parentWidget.ToSharedRef(), FWidgetPath(), menuWidget.ToSharedRef(), CursorPos, FPopupTransitionEffect((FPopupTransitionEffect::ESlideDirection)menuSlideDirection));
	Py_RETURN_NONE;
}

static PyObject *py_ue_add_window(PyObject *cls, PyObject * args)
{
    PyObject *py_window_obj;
    PyObject *py_show_immediately;

    if (!PyArg_ParseTuple(args, "O|O", &py_window_obj, &py_show_immediately))
    {
        return nullptr;
    }

    ue_PySWindow *py_window = py_ue_is_swindow(py_window_obj);
    if (!py_window)
    {
        return PyErr_Format(PyExc_Exception, "window to add is not an SWindow");
    }

    const bool showImmediately = (py_show_immediately) ? (PyObject_IsTrue(py_show_immediately)) : true;

    FSlateApplication::Get().AddWindow(StaticCastSharedRef<SWindow>(((ue_PySWidget*)py_window)->Widget), showImmediately);

    return py_window_obj;
}

static PyObject *py_ue_destroy_window_immediately(PyObject *cls, PyObject * args)
{
    PyObject *py_window_obj;

    if (!PyArg_ParseTuple(args, "O:window_to_destroy", &py_window_obj))
    {
        return nullptr;
    }

    ue_PySWindow *py_window = py_ue_is_swindow(py_window_obj);

    if (!py_window)
    {
        return PyErr_Format(PyExc_Exception, "window_to_destroy is not an SWindow");
    }
 
    FSlateApplication::Get().DestroyWindowImmediately(StaticCastSharedRef<SWindow>(py_window->s_compound_widget.s_widget.Widget));
    
    Py_RETURN_NONE;
}

static PyMethodDef ue_PyFSlateApplication_methods[] = {
	{ "get_average_delta_time", (PyCFunction)py_ue_get_average_delta_time, METH_VARARGS | METH_CLASS, "" },
	{ "get_cursor_radius", (PyCFunction)py_ue_get_cursor_radius, METH_VARARGS | METH_CLASS, "" },
	{ "get_delta_time", (PyCFunction)py_ue_get_delta_time, METH_VARARGS | METH_CLASS, "" },
	{ "get_modifier_keys", (PyCFunction)py_ue_get_modifier_keys, METH_VARARGS | METH_CLASS, "" },
	{ "goto_line_in_source", (PyCFunction)py_ue_goto_line_in_source, METH_VARARGS | METH_CLASS, "" },
	{ "is_gamepad_attached", (PyCFunction)py_ue_is_gamepad_attached, METH_VARARGS | METH_CLASS, "" },
	{ "is_mouse_attached", (PyCFunction)py_ue_is_mouse_attached, METH_VARARGS | METH_CLASS, "" },
	{ "process_key_down_event", (PyCFunction)py_ue_process_key_down_event, METH_VARARGS | METH_CLASS, "" },
	{ "process_key_char_event", (PyCFunction)py_ue_process_key_char_event, METH_VARARGS | METH_CLASS, "" },
	{ "set_application_scale", (PyCFunction)py_ue_set_application_scale, METH_VARARGS | METH_CLASS, "" },
	{ "set_all_user_focus", (PyCFunction)py_ue_set_all_user_focus, METH_VARARGS | METH_CLASS, "" },
	{ "set_cursor_pos", (PyCFunction)py_ue_set_cursor_pos, METH_VARARGS | METH_CLASS, "" },
	{ "push_menu", (PyCFunction)py_ue_push_menu, METH_VARARGS | METH_CLASS, "" },
    { "add_window", (PyCFunction)py_ue_add_window, METH_VARARGS | METH_CLASS, "" },
    { "destroy_window_immediately", (PyCFunction)py_ue_destroy_window_immediately, METH_VARARGS | METH_CLASS, "" },
	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyFSlateApplicationType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FSlateApplication", /* tp_name */
	sizeof(ue_PyFSlateApplication), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine SlateApplication",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFSlateApplication_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_fslate_application_init(ue_PyFSlateApplication *self, PyObject * args)
{
	PyErr_SetString(PyExc_Exception, "FSlateApplication is a singleton");
	return -1;
}

void ue_python_init_fslate_application(PyObject *ue_module)
{
	ue_PyFSlateApplicationType.tp_new = PyType_GenericNew;
	ue_PyFSlateApplicationType.tp_init = (initproc)py_ue_fslate_application_init;

	if (PyType_Ready(&ue_PyFSlateApplicationType) < 0)
		return;

	Py_INCREF(&ue_PyFSlateApplicationType);
	PyModule_AddObject(ue_module, "FSlateApplication", (PyObject *)&ue_PyFSlateApplicationType);
}
