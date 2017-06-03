
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySTextBlock.h"


#define sw_text_block StaticCastSharedRef<STextBlock>(self->s_leaf_widget.s_widget.s_widget)

static PyObject *py_ue_stext_block_set_text(ue_PySTextBlock *self, PyObject * args) {
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_text", &text)) {
		return NULL;
	}

	sw_text_block->SetText(FString(UTF8_TO_TCHAR(text)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_stext_block_set_color_and_opacity(ue_PySTextBlock *self, PyObject * args) {

	PyObject *py_color;

	if (!PyArg_ParseTuple(args, "O:set_color_and_opacity", &py_color)) {
		return NULL;
	}

	ue_PyFLinearColor *py_linear_color = py_ue_is_flinearcolor(py_color);
	if (!py_linear_color) {
		return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor");
	}

	sw_text_block->SetColorAndOpacity(py_linear_color->color);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_stext_block_get_text(ue_PySTextBlock *self, PyObject * args) {

	return PyUnicode_FromString(TCHAR_TO_UTF8(*sw_text_block->GetText().ToString()));

}

static PyMethodDef ue_PySTextBlock_methods[] = {
	{ "set_color_and_opacity", (PyCFunction)py_ue_stext_block_set_color_and_opacity, METH_VARARGS, "" },
	{ "set_text", (PyCFunction)py_ue_stext_block_set_text, METH_VARARGS, "" },
	{ "get_text", (PyCFunction)py_ue_stext_block_get_text, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySTextBlockType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.STextBlock", /* tp_name */
	sizeof(ue_PySTextBlock), /* tp_basicsize */
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
	"Unreal Engine STextBlock",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySTextBlock_methods,             /* tp_methods */
};

static int ue_py_stext_block_init(ue_PySTextBlock *self, PyObject *args, PyObject *kwargs) {
	
	ue_py_slate_setup_farguments(STextBlock);

	ue_py_slate_farguments_bool("autowrap_text", AutoWrapText);
	ue_py_slate_farguments_struct("color_and_opacity", ColorAndOpacity, FSlateColor);
	ue_py_slate_farguments_struct("font", Font, FSlateFontInfo);
	ue_py_slate_farguments_flinear_color("highlight_color", HighlightColor);
	ue_py_slate_farguments_optional_struct_ptr("highlight_shape", HighlightShape, FSlateBrush);
	ue_py_slate_farguments_text("highlight_text", HighlightText);
	ue_py_slate_farguments_enum("justification", Justification, ETextJustify::Type);
	ue_py_slate_farguments_float("line_height_percentage", LineHeightPercentage);
	ue_py_slate_farguments_struct("margin", Margin, FMargin);
	ue_py_slate_farguments_float("min_desired_width", MinDesiredWidth);
	ue_py_slate_farguments_event("on_double_clicked", OnDoubleClicked, FOnClicked, OnClicked);
	ue_py_slate_farguments_flinear_color("shadow_color_and_opacity", ShadowColorAndOpacity);
	ue_py_slate_farguments_fvector2d("shadow_offset", ShadowOffset);
	ue_py_slate_farguments_text("text", Text);
	ue_py_slate_farguments_optional_enum("text_flow_direction", TextFlowDirection, ETextFlowDirection);
	ue_py_slate_farguments_optional_enum("text_shaping_method", TextShapingMethod, ETextShapingMethod);
	ue_py_slate_farguments_optional_struct_ptr("text_style", TextStyle, FTextBlockStyle);
	ue_py_slate_farguments_enum("wrapping_policy", WrappingPolicy, ETextWrappingPolicy);
	ue_py_slate_farguments_float("wrap_text_at", WrapTextAt);

	ue_py_snew(STextBlock, s_leaf_widget.s_widget);
	return 0;
}

void ue_python_init_stext_block(PyObject *ue_module) {
	ue_PySTextBlockType.tp_init = (initproc)ue_py_stext_block_init;

	ue_PySTextBlockType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySTextBlockType) < 0)
		return;

	Py_INCREF(&ue_PySTextBlockType);
	PyModule_AddObject(ue_module, "STextBlock", (PyObject *)&ue_PySTextBlockType);
}
