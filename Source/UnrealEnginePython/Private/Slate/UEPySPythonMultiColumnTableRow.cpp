
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySPythonMultiColumnTableRow.h"


#define sw_python_multicolumn_table_row StaticCastSharedRef<SPythonMultiColumnTableRow>(self->s_compound_widget.s_widget.s_widget)

TSharedRef<SWidget> SPythonMultiColumnTableRow::GenerateWidgetForColumn(const FName& ColumnName)
{
    FScopePythonGIL gil;

    if (!PyObject_HasAttrString(self, (char *)"generate_widget_for_column"))
        return SNullWidget::NullWidget;

    PyObject *py_callable_generate_widget_for_column = PyObject_GetAttrString(self, (char *)"generate_widget_for_column");
    if (!PyCalllable_Check_Extended(py_callable_generate_widget_for_column))
    {
        UE_LOG(LogPython, Error, TEXT("generate_widget_for_column is not a callable"));
        return SNullWidget::NullWidget;
    }

    PyObject *ret = PyObject_CallFunction(py_callable_generate_widget_for_column, (char *)"s", TCHAR_TO_UTF8(*ColumnName.ToString()));
    if (!ret)
    {
        unreal_engine_py_log_error();
        return SNullWidget::NullWidget;
    }

    ue_PySWidget *s_widget = py_ue_is_swidget(ret);
    if (!s_widget)
    {
        Py_DECREF(ret);
        UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
        return SNullWidget::NullWidget;
    }

    TSharedRef<SWidget> value = s_widget->s_widget;
    Py_DECREF(ret);
    return value;
}

FReply SPythonMultiColumnTableRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
    FScopePythonGIL gil;

    if (PyObject_HasAttrString(self, (char *)"on_mouse_button_double_click"))
    {
        PyObject *py_callable_on_mouse_button_double_click = PyObject_GetAttrString(self, (char *)"on_mouse_button_double_click");
        if (!PyCalllable_Check_Extended(py_callable_on_mouse_button_double_click))
        {
            UE_LOG(LogPython, Error, TEXT("on_mouse_button_double_click is not a callable"));
            return FReply::Unhandled();
        }

        PyObject *ret = PyObject_CallFunction(py_callable_on_mouse_button_double_click, (char *)"OO", py_ue_new_fgeometry(InMyGeometry), py_ue_new_fpointer_event(InMouseEvent));
        if (!ret)
        {
            unreal_engine_py_log_error();
            return FReply::Unhandled();
        }

        if (ret == Py_False)
        {
            Py_DECREF(ret);
            return FReply::Unhandled();
        }
        Py_DECREF(ret);
        return FReply::Handled();
    }
    else
    {
        return SPythonMultiColumnTableRow::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
    }
}


static PyMethodDef ue_PySPythonMultiColumnTableRow_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySPythonMultiColumnTableRowType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonMultiColumnTableRow", /* tp_name */
	sizeof(ue_PySPythonMultiColumnTableRow), /* tp_basicsize */
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
	"Unreal Engine SPythonMultiColumnTableRow",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonMultiColumnTableRow_methods,             /* tp_methods */
};

static int ue_py_spython_multicolumn_table_row_init(ue_PySPythonMultiColumnTableRow *self, PyObject *args, PyObject *kwargs) {

    PyObject *py_object = nullptr;
    if (!PyArg_ParseTuple(args, "O", &py_object))
    {
        return -1;
    }

    ue_PySTableViewBase* py_owner_table_view_base = py_ue_is_stable_view_base(py_object);
    if (!py_owner_table_view_base) {
        PyErr_SetString(PyExc_Exception, "Argument is not a STableViewBase");
        return -1;
    }

    Py_INCREF(py_owner_table_view_base);
    ue_py_snew_simple_with_req_args(
        SPythonMultiColumnTableRow, s_compound_widget.s_widget, 
        StaticCastSharedRef<STableViewBase>(py_owner_table_view_base->s_compound_widget.s_widget.s_widget),
        (PyObject *)self);
	return 0;
}

void ue_python_init_spython_multicolumn_table_row(PyObject *ue_module)
{
    ue_PySPythonMultiColumnTableRowType.tp_base = &ue_PySCompoundWidgetType;
    ue_PySPythonMultiColumnTableRowType.tp_init = (initproc)ue_py_spython_multicolumn_table_row_init;

    if (PyType_Ready(&ue_PySPythonMultiColumnTableRowType) < 0)
        return;

    Py_INCREF(&ue_PySPythonMultiColumnTableRowType);
    PyModule_AddObject(ue_module, "SPythonMultiColumnTableRow", (PyObject *)&ue_PySPythonMultiColumnTableRowType);
}

ue_PySPythonMultiColumnTableRow *py_ue_is_spython_multicolumn_table_row(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PySPythonMultiColumnTableRowType))
		return nullptr;
	return (ue_PySPythonMultiColumnTableRow *)obj;
}