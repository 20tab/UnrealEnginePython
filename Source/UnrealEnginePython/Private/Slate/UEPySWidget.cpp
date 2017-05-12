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


static PyMethodDef ue_PySWidget_methods[] = {
	{ "get_children", (PyCFunction)py_ue_swidget_get_children, METH_VARARGS, "" },
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


#endif