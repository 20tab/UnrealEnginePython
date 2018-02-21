
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySBorder.h"


#define sw_border StaticCastSharedRef<SBorder>(self->s_compound_widget.s_widget.s_widget)

static PyObject *py_ue_sborder_clear_content(ue_PySBorder *self, PyObject * args)
{

	sw_border->ClearContent();

	Py_RETURN_NONE;
}

static PyObject *py_ue_sborder_set_content(ue_PySBorder *self, PyObject * args)
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


	sw_border->SetContent(py_swidget->s_widget->AsShared());

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_sborder_set_padding(ue_PySBorder *self, PyObject * args)
{
	PyObject *py_padding;
	if (!PyArg_ParseTuple(args, "O:set_padding", &py_padding))
	{
		return NULL;
	}

	FMargin *margin = ue_py_check_struct<FMargin>(py_padding);
	if (!margin)
	{
		if (!PyNumber_Check(py_padding))
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FMargin or a number");
		}
		PyObject *py_float = PyNumber_Float(py_padding);
		FMargin new_margin(PyFloat_AsDouble(py_float));
		margin = &new_margin;
		Py_DECREF(py_float);
	}

	sw_border->SetPadding(*margin);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_sborder_set_h_align(ue_PySBorder *self, PyObject * args)
{
	int align;
	if (!PyArg_ParseTuple(args, "i:set_h_align", &align))
	{
		return NULL;
	}

	sw_border->SetHAlign((EHorizontalAlignment)align);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_sborder_set_v_align(ue_PySBorder *self, PyObject * args)
{
	int align;
	if (!PyArg_ParseTuple(args, "i:set_v_align", &align))
	{
		return NULL;
	}

	sw_border->SetVAlign((EVerticalAlignment)align);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_sborder_set_border_image(ue_PySBorder *self, PyObject * args)
{
	PyObject *py_brush;
	if (!PyArg_ParseTuple(args, "O:set_border_image", &py_brush))
	{
		return NULL;
	}

	FSlateBrush *brush = ue_py_check_struct<FSlateBrush>(py_brush);
	if (!brush)
		return PyErr_Format(PyExc_Exception, "argument is not a FSlateBrush");

	sw_border->SetBorderImage(brush);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyMethodDef ue_PySBorder_methods[] = {
	{ "clear_content",    (PyCFunction)py_ue_sborder_clear_content, METH_VARARGS, "" },
	{ "set_content",      (PyCFunction)py_ue_sborder_set_content, METH_VARARGS, "" },
	{ "set_padding",      (PyCFunction)py_ue_sborder_set_padding, METH_VARARGS, "" },
	{ "set_h_align",      (PyCFunction)py_ue_sborder_set_h_align, METH_VARARGS, "" },
	{ "set_v_align",      (PyCFunction)py_ue_sborder_set_v_align, METH_VARARGS, "" },
	{ "set_border_image", (PyCFunction)py_ue_sborder_set_border_image, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySBorderType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SBorder", /* tp_name */
	sizeof(ue_PySBorder), /* tp_basicsize */
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
	"Unreal Engine SBorder",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySBorder_methods,             /* tp_methods */
};

static int ue_py_sborder_init(ue_PySBorder *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SBorder);
	ue_py_slate_farguments_struct("border_background_color", BorderBackgroundColor, FSlateColor);
	ue_py_slate_farguments_flinear_color("color_and_opacity", ColorAndOpacity);
	ue_py_slate_farguments_optional_struct_ptr("border_image", BorderImage, FSlateBrush);
	ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
	ue_py_slate_farguments_optional_enum("v_align", VAlign, EVerticalAlignment);
	ue_py_slate_farguments_struct("padding", Padding, FMargin);
	ue_py_slate_farguments_struct("foreground_color", ForegroundColor, FSlateColor);
	ue_py_slate_farguments_fvector2d("content_scale", ContentScale);
	ue_py_slate_farguments_fvector2d("desired_size_scale", DesiredSizeScale);
	ue_py_snew(SBorder, s_compound_widget.s_widget);

	return 0;
}

void ue_python_init_sborder(PyObject *ue_module)
{

	ue_PySBorderType.tp_init = (initproc)ue_py_sborder_init;
	ue_PySBorderType.tp_call = (ternaryfunc)py_ue_sborder_set_content;

	ue_PySBorderType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySBorderType) < 0)
		return;

	Py_INCREF(&ue_PySBorderType);
	PyModule_AddObject(ue_module, "SBorder", (PyObject *)&ue_PySBorderType);
}
