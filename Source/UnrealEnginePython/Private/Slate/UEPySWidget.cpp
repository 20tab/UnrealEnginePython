
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySWidget.h"

static PyObject *ue_PySWidget_str(ue_PySWidget *self) {
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_FromFormat("<unreal_engine.%s '%p' (slate ref count: %d, py ref count: %d)>",
		TCHAR_TO_UTF8(*self->s_widget->GetTypeAsString()), &self->s_widget.Get(), self->s_widget.GetSharedReferenceCount(), self->ob_base.ob_refcnt);
#else
	return PyUnicode_FromFormat("<unreal_engine.%s '%p' (slate ref count: %d)>",
		TCHAR_TO_UTF8(*self->s_widget->GetTypeAsString()), &self->s_widget.Get(), self->s_widget.GetSharedReferenceCount());
#endif
}

static PyObject *py_ue_swidget_get_children(ue_PySWidget *self, PyObject * args) {
	FChildren *children = self->s_widget->GetChildren();
	PyObject *py_list = PyList_New(0);
	for (int32 i = 0; i < children->Num(); i++) {
		TSharedRef<SWidget> widget = children->GetChildAt(i);
		PyObject *item = (PyObject *)ue_py_get_swidget(widget);
		PyList_Append(py_list, item);
		Py_DECREF(item);
	}
	return py_list;
}

static PyObject *py_ue_swidget_set_tooltip_text(ue_PySWidget *self, PyObject * args) {
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_tooltip_text", &text)) {
		return NULL;
	}

	self->s_widget->SetToolTipText(FText::FromString(UTF8_TO_TCHAR(text)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_set_cursor(ue_PySWidget *self, PyObject * args) {
	int cursor;
	if (!PyArg_ParseTuple(args, "i:set_cursor", &cursor)) {
		return NULL;
	}

	self->s_widget->SetCursor((EMouseCursor::Type)cursor);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_set_enabled(ue_PySWidget *self, PyObject * args) {
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_enabled", &py_bool)) {
		return NULL;
	}

	self->s_widget->SetEnabled(PyObject_IsTrue(py_bool) ? true : false);

	Py_INCREF(self);
	return (PyObject *)self;
}

#if ENGINE_MINOR_VERSION > 12
static PyObject *py_ue_swidget_bind_on_mouse_button_down(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_button_down", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseButtonDown(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_bind_on_mouse_button_up(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_button_up", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseButtonUp(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_bind_on_mouse_double_click(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_double_click", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseDoubleClick(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_bind_on_mouse_move(ue_PySWidget *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_mouse_move", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FPointerEventHandler handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseEvent);

	self->s_widget->SetOnMouseMove(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}
#endif


static PyObject *py_ue_swidget_has_keyboard_focus(ue_PySWidget *self, PyObject * args) {

	if (self->s_widget->HasKeyboardFocus()) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_swidget_get_type(ue_PySWidget *self, PyObject * args) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->s_widget->GetTypeAsString())));
}

static PyObject *py_ue_swidget_get_shared_reference_count(ue_PySWidget *self, PyObject * args) {
	return PyLong_FromLong(self->s_widget.GetSharedReferenceCount());
}

static PyMethodDef ue_PySWidget_methods[] = {
	{ "get_shared_reference_count", (PyCFunction)py_ue_swidget_get_shared_reference_count, METH_VARARGS, "" },
	{ "get_children", (PyCFunction)py_ue_swidget_get_children, METH_VARARGS, "" },
	{ "get_type", (PyCFunction)py_ue_swidget_get_type, METH_VARARGS, "" },
	{ "set_tooltip_text", (PyCFunction)py_ue_swidget_set_tooltip_text, METH_VARARGS, "" },
	{ "set_cursor", (PyCFunction)py_ue_swidget_set_cursor, METH_VARARGS, "" },
	{ "set_enabled", (PyCFunction)py_ue_swidget_set_enabled, METH_VARARGS, "" },
	{ "has_keyboard_focus", (PyCFunction)py_ue_swidget_has_keyboard_focus, METH_VARARGS, "" },
#if ENGINE_MINOR_VERSION > 12
	{ "bind_on_mouse_button_down", (PyCFunction)py_ue_swidget_bind_on_mouse_button_down, METH_VARARGS, "" },
	{ "bind_on_mouse_button_up", (PyCFunction)py_ue_swidget_bind_on_mouse_button_down, METH_VARARGS, "" },
	{ "bind_on_mouse_double_click", (PyCFunction)py_ue_swidget_bind_on_mouse_double_click, METH_VARARGS, "" },
	{ "bind_on_mouse_move", (PyCFunction)py_ue_swidget_bind_on_mouse_move, METH_VARARGS, "" },
#endif
	{ NULL }  /* Sentinel */
};

static void ue_PySWidgett_dealloc(ue_PySWidget *self) {
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PySWidget %p mapped to %s %p (slate refcount: %d)"), self, *self->s_widget->GetTypeAsString(), &self->s_widget.Get(), self->s_widget.GetSharedReferenceCount());
#endif
	Py_DECREF(self->py_dict);
	for (UPythonSlateDelegate *item : self->delegates) {
		if (item->IsValidLowLevel() && item->IsRooted())
			item->RemoveFromRoot();
	}
	for (ue_PySWidget *item : self->py_swidget_slots) {
		Py_DECREF(item);
	}
	// decref content (if any)
	Py_XDECREF(self->py_swidget_content);
	for (PyObject *item : self->py_refs) {
		Py_DECREF(item);
	}
	ue_py_unregister_swidget(&self->s_widget.Get());
	// decrement widget reference count
	// but only if python vm is still fully active (hack to avoid crashes on editor shutdown)
	if (Py_IsInitialized()) {
		self->s_widget = SNullWidget::NullWidget;
	}
	else {
		UE_LOG(LogPython, Warning, TEXT("Python VM is being destroyed, skipping ue_PySWidget destruction"));
	}
	Py_TYPE(self)->tp_free((PyObject *)self);

}

PyTypeObject ue_PySWidgetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SWidget", /* tp_name */
	sizeof(ue_PySWidget), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_PySWidgett_dealloc,       /* tp_dealloc */
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
	(reprfunc)ue_PySWidget_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SWidget",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySWidget_methods,             /* tp_methods */
};

static PyObject *
ue_PySWidget_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	ue_PySWidget *self;

	self = (ue_PySWidget *)type->tp_alloc(type, 0);
	if (self != NULL) {
		ue_py_setup_swidget(self);
	}

	return (PyObject *)self;
}

void ue_python_init_swidget(PyObject *ue_module) {
	ue_PySWidgetType.tp_new = ue_PySWidget_new;

	ue_PySWidgetType.tp_getattro = PyObject_GenericGetAttr;
	ue_PySWidgetType.tp_setattro = PyObject_GenericSetAttr;
	ue_PySWidgetType.tp_dictoffset = offsetof(ue_PySWidget, py_dict);

	if (PyType_Ready(&ue_PySWidgetType) < 0)
		return;

	Py_INCREF(&ue_PySWidgetType);
	PyModule_AddObject(ue_module, "SWidget", (PyObject *)&ue_PySWidgetType);
}

ue_PySWidget *py_ue_is_swidget(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PySWidgetType))
		return nullptr;
	return (ue_PySWidget *)obj;
}

