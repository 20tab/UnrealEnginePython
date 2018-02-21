
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySPythonWidget.h"

#define sw_python_widget StaticCastSharedRef<SPythonWidget>(self->s_compound_widget.s_widget.s_widget)

static PyObject *py_ue_spython_widget_set_active(ue_PySPythonWidget *self, PyObject *args)
{
	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "|O:set_active", &py_bool))
	{
		return nullptr;
	}
	bool bActive = true;

	if (py_bool)
	{
		if (PyObject_IsTrue(py_bool))
		{
			bActive = true;
		}
		else
		{
			bActive = false;
		}
	}

	sw_python_widget->SetActive(bActive);
	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_spython_widget_set_content(ue_PySPythonWidget *self, PyObject *args)
{
    PyObject *py_content;
    if (!PyArg_ParseTuple(args, "O:set_content", &py_content))
    {
        return NULL;
    }

    ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
    if (!py_swidget)
    {
        return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
    }

    Py_INCREF(py_swidget);

    sw_python_widget->SetContent(py_swidget->s_widget->AsShared());

    Py_INCREF(self);
    return (PyObject *)self;
}

static PyObject *py_ue_spython_widget_clear_content(ue_PySPythonWidget *self, PyObject *args)
{
    sw_python_widget->ClearContent();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef ue_PySPythonWidget_methods[] = {
	{ "set_active",    (PyCFunction)py_ue_spython_widget_set_active, METH_VARARGS | METH_KEYWORDS, "" },
    { "set_content",   (PyCFunction)py_ue_spython_widget_set_content, METH_VARARGS, "" },
    { "clear_content", (PyCFunction)py_ue_spython_widget_clear_content, METH_VARARGS, "" },
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
	sw_python_widget->SetPyObject((PyObject *)self);
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
