
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySExpanderArrow.h"


#define sw_expander_arrow StaticCastSharedRef<SExpanderArrow>(self->s_compound_widget.s_widget.s_widget)

static PyMethodDef ue_PySExpanderArrow_methods[] = {
	{ NULL }  /* Sentinel */
};

static void ue_PySExpanderArrow_dealloc(ue_PySExpanderArrow *self)
{
#if defined(UEPY_MEMORY_DEBUG)
    UE_LOG(LogPython, Warning, TEXT("Destroying ue_PySExpanderArrow %p"), self);
#endif

    Py_XDECREF(self->owner_row_py);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyTypeObject ue_PySExpanderArrowType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SExpanderArrow", /* tp_name */
	sizeof(ue_PySExpanderArrow), /* tp_basicsize */
	0,                         /* tp_itemsize */
    (destructor)ue_PySExpanderArrow_dealloc,       /* tp_dealloc */
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
	"Unreal Engine SExpanderArrow",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
    ue_PySExpanderArrow_methods,             /* tp_methods */
};

static int ue_py_sexpander_arrow_init(ue_PySExpanderArrow *self, PyObject *args, PyObject *kwargs)
{
    PyObject *py_object = nullptr;
    if (!PyArg_ParseTuple(args, "O", &py_object))
    {
        return -1;
    }

    ue_PySPythonMultiColumnTableRow* py_owner_table_row = py_ue_is_spython_multicolumn_table_row(py_object);
    if (!py_owner_table_row) {
        PyErr_SetString(PyExc_Exception, "Argument is not a SPythonMultiColumnTableRow");
        return -1;
    }

	ue_py_slate_setup_farguments(SExpanderArrow);
    ue_py_slate_farguments_attribute_float("indent_amount", IndentAmount);
    ue_py_slate_farguments_attribute_int("base_indent_level", BaseIndentLevel);    
    ue_py_snew_with_args(SExpanderArrow, s_compound_widget.s_widget, StaticCastSharedRef<SPythonMultiColumnTableRow>(py_owner_table_row->s_compound_widget.s_widget.s_widget));
    
    self->owner_row_py = py_owner_table_row;
    Py_INCREF(py_owner_table_row);    

	return 0;
}

void ue_python_init_sexpander_arrow(PyObject *ue_module)
{
    ue_PySExpanderArrowType.tp_init = (initproc)ue_py_sexpander_arrow_init;
    ue_PySExpanderArrowType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySExpanderArrowType) < 0)
		return;

	Py_INCREF(&ue_PySExpanderArrowType);
	PyModule_AddObject(ue_module, "SExpanderArrow", (PyObject *)&ue_PySExpanderArrowType);
}
