#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySWidget.h"

static PyObject *ue_PySWidget_str(ue_PySWidget *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SWidget '%p'>",
		&self->s_widget.Get());
}

static PyObject *py_ue_swidget_get_children(ue_PySWidget *self, PyObject * args) {
	FChildren *children = self->s_widget->GetChildren();
	PyObject *py_list = PyList_New(0);
	for (int32 i = 0; i < children->Num(); i++) {
		TSharedRef<SWidget> widget = children->GetChildAt(i);
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
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnMouseButtonDown);

	self->s_widget->SetOnMouseButtonDown(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swidget_has_keyboard_focus(ue_PySWidget *self, PyObject * args) {
	
	if (self->s_widget->HasKeyboardFocus()) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyMethodDef ue_PySWidget_methods[] = {
	{ "get_children", (PyCFunction)py_ue_swidget_get_children, METH_VARARGS, "" },
	{ "set_tooltip_text", (PyCFunction)py_ue_swidget_set_tooltip_text, METH_VARARGS, "" },
	{ "has_keyboard_focus", (PyCFunction)py_ue_swidget_has_keyboard_focus, METH_VARARGS, "" },
	{ "bind_on_mouse_button_down", (PyCFunction)py_ue_swidget_bind_on_mouse_button_down, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySWidgetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SWidget", /* tp_name */
	sizeof(ue_PySWidget), /* tp_basicsize */
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

void ue_python_init_swidget(PyObject *ue_module) {
	ue_PySWidgetType.tp_new = PyType_GenericNew;

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

#endif