

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySHeaderRow.h"

#define GET_s_header_row SHeaderRow *s_header_row =(SHeaderRow *)self->s_border.s_compound_widget.s_widget.s_widget

static PyObject *ue_PySHeaderRow_str(ue_PySHeaderRow *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SHeaderRow '%p'>",
		self->s_border.s_compound_widget.s_widget.s_widget);
}

static PyObject *py_ue_sheader_row_add_column(ue_PySHeaderRow *self, PyObject * args, PyObject *kwargs) {

	int cell_h_align = 0;
	int cell_v_align = 0;
	char *column_id;
	char *default_label = nullptr;
	char *default_tooltip = nullptr;
	float fill_width = 0;


	char *kwlist[] = {
		(char *)"column_id",
		(char *)"cell_h_align",
		(char *)"cell_v_align",
		(char *)"default_label",
		(char *)"default_tooltip",
		(char *)"fill_width",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|iissf:add_column", kwlist,
		&column_id,
		&cell_h_align,
		&cell_v_align,
		&default_label,
		&default_tooltip,
		&fill_width)) {
		return NULL;
	}

	if (!default_label)
		default_label = column_id;

	if (!default_tooltip)
		default_tooltip = default_label;

	auto &column = SHeaderRow::Column(FName(UTF8_TO_TCHAR(column_id)))
		.DefaultLabel(FText::FromString(UTF8_TO_TCHAR(default_label)))
		.DefaultTooltip(FText::FromString(UTF8_TO_TCHAR(default_tooltip)))
		.HAlignCell((EHorizontalAlignment)cell_h_align)
		.VAlignCell((EVerticalAlignment)cell_v_align);

	if (fill_width)
		column.FillWidth(fill_width);


	GET_s_header_row;

	s_header_row->AddColumn(column);


	Py_INCREF(self);
	return (PyObject *)self;
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
	(reprfunc)ue_PySHeaderRow_str,                         /* tp_str */
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

static int ue_py_sheader_row_init(ue_PySHeaderRow *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SHeaderRow, s_border.s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_sheader_row(PyObject *ue_module) {
	ue_PySHeaderRowType.tp_new = PyType_GenericNew;

	ue_PySHeaderRowType.tp_init = (initproc)ue_py_sheader_row_init;

	ue_PySHeaderRowType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySHeaderRowType) < 0)
		return;

	Py_INCREF(&ue_PySHeaderRowType);
	PyModule_AddObject(ue_module, "SHeaderRow", (PyObject *)&ue_PySHeaderRowType);
}

