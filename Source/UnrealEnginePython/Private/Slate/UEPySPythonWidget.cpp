
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySPythonWidget.h"

#define sw_python_widget StaticCastSharedRef<SPythonWidget>(self->s_compound_widget.s_widget.s_widget)

static PyMethodDef ue_PySPythonWidget_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySPythonWidgetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonWidget", /* tp_name */
	sizeof(ue_PySPythonWidget), /* tp_basicsize */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SPythonWidget",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonWidget_methods,             /* tp_methods */
};

static int ue_py_spython_widget_init(ue_PySPythonWidget *self, PyObject *args, PyObject *kwargs)
{

	ue_py_snew_simple(SPythonWidget, s_compound_widget.s_widget);

	if (kwargs)
	{
		if (PyObject *py_tick = PyDict_GetItemString(kwargs, "tick"))
		{
			if (!PyCallable_Check(py_tick))
			{
				PyErr_SetString(PyExc_Exception, "tick must be a callable");
				return -1;
			}
			sw_python_widget->SetPythonTick(py_tick);
		}
		else
		{
			PyErr_Clear();
		}

		if (PyObject *py_paint = PyDict_GetItemString(kwargs, "paint"))
		{
			if (!PyCallable_Check(py_paint))
			{
				PyErr_SetString(PyExc_Exception, "paint must be a callable");
				return -1;
			}
			sw_python_widget->SetPythonPaint(py_paint);
		}
		else
		{
			PyErr_Clear();
		}
	}

	return 0;
}

void ue_python_init_spython_widget(PyObject *ue_module)
{

	ue_PySPythonWidgetType.tp_base = &ue_PySCompoundWidgetType;
	ue_PySPythonWidgetType.tp_init = (initproc)ue_py_spython_widget_init;

	if (PyType_Ready(&ue_PySPythonWidgetType) < 0)
		return;

	Py_INCREF(&ue_PySPythonWidgetType);
	PyModule_AddObject(ue_module, "SPythonWidget", (PyObject *)&ue_PySPythonWidgetType);
}
