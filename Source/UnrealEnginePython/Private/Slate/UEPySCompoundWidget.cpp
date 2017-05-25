
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySCompoundWidget.h"

#define GET_s_compound_widget SCompoundWidget *s_compound_widget = (SCompoundWidget*)self->s_widget.s_widget

static PyObject *ue_PySCompoundWidget_str(ue_PySCompoundWidget *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SCompoundWidget '%p'>",
		self->s_widget.s_widget);
}

static PyObject *py_ue_scompound_widget_get_color_and_opacity(ue_PySCompoundWidget *self, PyObject * args) {

	GET_s_compound_widget;

	FLinearColor color = s_compound_widget->GetColorAndOpacity();

	return py_ue_new_flinearcolor(color);
}

static PyMethodDef ue_PySCompoundWidget_methods[] = {
	{ "get_color_and_opacity", (PyCFunction)py_ue_scompound_widget_get_color_and_opacity, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


PyTypeObject ue_PySCompoundWidgetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SCompoundWidget", /* tp_name */
	sizeof(ue_PySCompoundWidget), /* tp_basicsize */
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
	(reprfunc)ue_PySCompoundWidget_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SCompoundWidget",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySCompoundWidget_methods,             /* tp_methods */
};

void ue_python_init_scompound_widget(PyObject *ue_module) {
	ue_PySCompoundWidgetType.tp_new = PyType_GenericNew;

	ue_PySCompoundWidgetType.tp_base = &ue_PySWidgetType;

	if (PyType_Ready(&ue_PySCompoundWidgetType) < 0)
		return;

	Py_INCREF(&ue_PySCompoundWidgetType);
	PyModule_AddObject(ue_module, "SCompoundWidget", (PyObject *)&ue_PySCompoundWidgetType);
}
