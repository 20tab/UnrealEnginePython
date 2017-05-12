#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySWindow.h"

#include "Runtime/SlateCore/Public/Widgets/SWindow.h"

#define GET_s_window TSharedRef<SWindow> s_window = StaticCastSharedRef<SWindow>(self->s_compound_widget.s_widget.s_widget)

static PyObject *py_ue_swindow_set_title(ue_PySWindow *self, PyObject * args) {
	char *title;
	if (!PyArg_ParseTuple(args, "s:set_title", &title)) {
		return NULL;
	}

	GET_s_window;

	s_window.Get().SetTitle(FText::FromString(UTF8_TO_TCHAR(title)));

	return (PyObject *)self;
}

static PyObject *ue_PySWindow_str(ue_PySWindow *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SWindow '%p'>",
		&self->s_compound_widget.s_widget.s_widget.Get());
}

static PyMethodDef ue_PySWindow_methods[] = {
	{ "set_title", (PyCFunction)py_ue_swindow_set_title, METH_VARARGS, "" },
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
	self->s_compound_widget.s_widget.s_widget = TSharedRef<SWindow>(SNew(SWindow));
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


#endif