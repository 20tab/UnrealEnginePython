

#include "UEPySHeaderRow.h"

static PyObject *py_ue_sheader_row_add_column(ue_PySHeaderRow *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_cast(SHeaderRow);


	int32 retCode = [&]() {
		ue_py_slate_setup_farguments(SHeaderRow::FColumn);

		// first of all check for values
		PyObject *py_columnid = ue_py_dict_get_item(kwargs, "column_id");
		if (!py_columnid)
		{
			PyErr_SetString(PyExc_TypeError, "you must specify the column_id");
			return -1;
		}

		ue_py_slate_farguments_optional_string("column_id", ColumnId);

		ue_py_slate_farguments_text("default_label", DefaultLabel);
		ue_py_slate_farguments_text("default_tooltip", DefaultTooltip);

		ue_py_slate_farguments_optional_enum("h_align_header", HAlignHeader, EHorizontalAlignment);
		ue_py_slate_farguments_optional_enum("v_align_header", VAlignHeader, EVerticalAlignment);

		ue_py_slate_farguments_float("fill_width", FillWidth);
		ue_py_slate_farguments_optional_float("fixed_width", FixedWidth);
		ue_py_slate_farguments_float("manual_width", ManualWidth);

		ue_py_slate_farguments_optional_named_slot("header_content", HeaderContent);
		ue_py_slate_farguments_optional_struct("header_content_padding", HeaderContentPadding, FMargin);

		ue_py_slate_farguments_optional_named_slot("menu_content", MenuContent);

		ue_py_slate_farguments_optional_enum("h_align_cell", HAlignCell, EHorizontalAlignment);
		ue_py_slate_farguments_optional_enum("v_align_cell", VAlignCell, EVerticalAlignment);

		ue_py_slate_farguments_enum("sort_mode", SortMode, EColumnSortMode::Type);
		ue_py_slate_farguments_enum("sort_priority", SortPriority, EColumnSortPriority::Type);

		ue_py_slate_farguments_event("on_sort", OnSort, FOnSortModeChanged, OnSort);

		ue_py_slate_farguments_bool("should_generate_widget", ShouldGenerateWidget);

		//sw_header_row->AddColumn(
		//    SHeaderRow::Column(FName(UTF8_TO_TCHAR(column_id)))
		//    .DefaultLabel(FText::FromString(UTF8_TO_TCHAR(default_label)))
		//    .DefaultTooltip(FText::FromString(UTF8_TO_TCHAR(default_tooltip)))
		//    .FixedWidth(fixed_width)
		//    .HAlignCell((EHorizontalAlignment)cell_h_align)
		//    .VAlignCell((EVerticalAlignment)cell_v_align)
		//);

		py_SHeaderRow->AddColumn(arguments);
		return 0;
	}();

	if (retCode != 0)
	{
		return PyErr_Format(PyExc_Exception, "could not add column slot");
	}

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySHeaderRow_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_column", (PyCFunction)py_ue_sheader_row_add_column, METH_VARARGS | METH_KEYWORDS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySHeaderRowType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SHeaderRow", /* tp_name */
	sizeof(ue_PySHeaderRow), /* tp_basicsize */
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
	"Unreal Engine SHeaderRow",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySHeaderRow_methods,             /* tp_methods */
};

static int ue_py_sheader_row_init(ue_PySHeaderRow *self, PyObject *args, PyObject *kwargs)
{
	ue_py_snew_simple(SHeaderRow);
	return 0;
}

void ue_python_init_sheader_row(PyObject *ue_module)
{

	ue_PySHeaderRowType.tp_init = (initproc)ue_py_sheader_row_init;
	ue_PySHeaderRowType.tp_call = (ternaryfunc)py_ue_sheader_row_add_column;

	ue_PySHeaderRowType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySHeaderRowType) < 0)
		return;

	Py_INCREF(&ue_PySHeaderRowType);
	PyModule_AddObject(ue_module, "SHeaderRow", (PyObject *)&ue_PySHeaderRowType);
}

ue_PySHeaderRow * py_ue_is_sheader_row(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PySHeaderRowType))
		return nullptr;
	return (ue_PySHeaderRow *)obj;
}

