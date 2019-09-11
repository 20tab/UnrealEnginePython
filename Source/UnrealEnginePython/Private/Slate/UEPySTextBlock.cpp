
#include "UEPySTextBlock.h"


static PyObject *py_ue_stext_block_set_text(ue_PySTextBlock *self, PyObject * args)
{
	ue_py_slate_cast(STextBlock);
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_text", &text))
	{
		return nullptr;
	}

	py_STextBlock->SetText(FString(UTF8_TO_TCHAR(text)));

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_stext_block_set_color_and_opacity(ue_PySTextBlock *self, PyObject * args)
{

	ue_py_slate_cast(STextBlock);

	PyObject *py_color;

	if (!PyArg_ParseTuple(args, "O:set_color_and_opacity", &py_color))
	{
		return nullptr;
	}

	ue_PyFLinearColor *py_linear_color = py_ue_is_flinearcolor(py_color);
	if (!py_linear_color)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor");
	}

	py_STextBlock->SetColorAndOpacity(py_linear_color->color);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_stext_block_get_text(ue_PySTextBlock *self, PyObject * args)
{
	ue_py_slate_cast(STextBlock);
	return PyUnicode_FromString(TCHAR_TO_UTF8(*py_STextBlock->GetText().ToString()));

}

static PyMethodDef ue_PySTextBlock_methods[] = {
	{ "set_color_and_opacity", (PyCFunction)py_ue_stext_block_set_color_and_opacity, METH_VARARGS, "" },
	{ "set_text", (PyCFunction)py_ue_stext_block_set_text, METH_VARARGS, "" },
	{ "get_text", (PyCFunction)py_ue_stext_block_get_text, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

DECLARE_UE_PY_SLATE_WIDGET(STextBlock);

static int ue_py_stext_block_init(ue_PySTextBlock *self, PyObject *args, PyObject *kwargs)
{

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
#if ENGINE_MINOR_VERSION >= 23
	ue_py_slate_farguments_event("on_double_clicked", OnDoubleClicked, FPointerEventHandler, OnMouseEvent);
#else
	ue_py_slate_farguments_event("on_double_clicked", OnDoubleClicked, FOnClicked, OnClicked);
#endif
	ue_py_slate_farguments_flinear_color("shadow_color_and_opacity", ShadowColorAndOpacity);
	ue_py_slate_farguments_fvector2d("shadow_offset", ShadowOffset);
	ue_py_slate_farguments_text("text", Text);
	ue_py_slate_farguments_optional_enum("text_flow_direction", TextFlowDirection, ETextFlowDirection);
	ue_py_slate_farguments_optional_enum("text_shaping_method", TextShapingMethod, ETextShapingMethod);
	ue_py_slate_farguments_optional_struct_ptr("text_style", TextStyle, FTextBlockStyle);
	ue_py_slate_farguments_enum("wrapping_policy", WrappingPolicy, ETextWrappingPolicy);
	ue_py_slate_farguments_float("wrap_text_at", WrapTextAt);

	ue_py_snew(STextBlock);
	return 0;
}

void ue_python_init_stext_block(PyObject *ue_module)
{
	ue_PySTextBlockType.tp_init = (initproc)ue_py_stext_block_init;

	ue_PySTextBlockType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySTextBlockType) < 0)
		return;

	Py_INCREF(&ue_PySTextBlockType);
	PyModule_AddObject(ue_module, "STextBlock", (PyObject *)&ue_PySTextBlockType);
}
