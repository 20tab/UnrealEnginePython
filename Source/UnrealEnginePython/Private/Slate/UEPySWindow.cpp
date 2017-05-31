
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySWindow.h"

#define GET_s_window SWindow *s_window = (SWindow*)self->s_compound_widget.s_widget.s_widget

static PyObject *py_ue_swindow_set_title(ue_PySWindow *self, PyObject * args) {
	char *title;
	if (!PyArg_ParseTuple(args, "s:set_title", &title)) {
		return NULL;
	}

	GET_s_window;

	s_window->SetTitle(FText::FromString(UTF8_TO_TCHAR(title)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swindow_resize(ue_PySWindow *self, PyObject * args) {
	int width;
	int height;
	if (!PyArg_ParseTuple(args, "ii:resize", &width, &height)) {
		return NULL;
	}

	GET_s_window;

	s_window->Resize(FVector2D(width, height));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swindow_set_content(ue_PySWindow *self, PyObject * args) {
	PyObject *py_content;
	if (!PyArg_ParseTuple(args, "O:set_content", &py_content)) {
		return NULL;
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
	if (!py_swidget) {
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}
	// TODO: decrement reference when destroying parent
	Py_INCREF(py_swidget);

	GET_s_window;

	s_window->SetContent(py_swidget->s_widget->AsShared());

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swindow_set_sizing_rule(ue_PySWindow *self, PyObject * args) {
	int rule;
	if (!PyArg_ParseTuple(args, "i:set_sizing_rule", &rule)) {
		return NULL;
	}

	GET_s_window;

	s_window->SetSizingRule((ESizingRule::Type)rule);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_swindow_get_handle(ue_PySWindow *self, PyObject * args) {

	GET_s_window;

	return PyLong_FromLongLong((long long)s_window->GetNativeWindow()->GetOSWindowHandle());
}

static PyObject *ue_PySWindow_str(ue_PySWindow *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SWindow '%p'>",
		self->s_compound_widget.s_widget.s_widget);
}

static PyMethodDef ue_PySWindow_methods[] = {
	{ "set_title", (PyCFunction)py_ue_swindow_set_title, METH_VARARGS, "" },
	{ "set_sizing_rule", (PyCFunction)py_ue_swindow_set_sizing_rule, METH_VARARGS, "" },
	{ "resize", (PyCFunction)py_ue_swindow_resize, METH_VARARGS, "" },
	{ "set_client_size", (PyCFunction)py_ue_swindow_resize, METH_VARARGS, "" },
	{ "set_content", (PyCFunction)py_ue_swindow_set_content, METH_VARARGS, "" },
	{ "get_handle", (PyCFunction)py_ue_swindow_get_handle, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySWindowType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SWindow", /* tp_name */
	sizeof(ue_PySWindow), /* tp_basicsize */
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
	(reprfunc)ue_PySWindow_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SWindow",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySWindow_methods,             /* tp_methods */
};

static int ue_py_swindow_init(ue_PySWindow *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SWindow, s_compound_widget.s_widget);

	GET_s_window;

	FSlateApplication::Get().AddWindow(StaticCastSharedRef<SWindow>(s_window->AsShared()), true);

	return 0;
}

void ue_python_init_swindow(PyObject *ue_module) {
	ue_PySWindowType.tp_new = PyType_GenericNew;

	ue_PySWindowType.tp_init = (initproc)ue_py_swindow_init;

	ue_PySWindowType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySWindowType) < 0)
		return;

	Py_INCREF(&ue_PySWindowType);
	PyModule_AddObject(ue_module, "SWindow", (PyObject *)&ue_PySWindowType);
}
