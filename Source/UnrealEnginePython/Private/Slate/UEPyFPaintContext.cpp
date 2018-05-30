#include "UEPyFPaintContext.h"

#include "UEPyFGeometry.h"

static FSlateBrush global_simple_brush;

static PyObject *py_ue_fpaint_context_draw_line(ue_PyFPaintContext *self, PyObject * args)
{
	float x1, y1, x2, y2;
	PyObject *py_linear_color = nullptr;
	PyObject *py_antialias = nullptr;
	float thickness = 1.0;

	if (!PyArg_ParseTuple(args, "(ff)(ff)|OOf:draw_line", &x1, &y1, &x2, &y2, &py_linear_color, &py_antialias, &thickness))
		return nullptr;

	FLinearColor tint = FLinearColor::White;

	if (py_linear_color)
	{
		if (!py_ue_get_flinearcolor(py_linear_color, tint))
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor or FColor.");
		}
	}

	FPaintContext context = self->paint_context;

	context.MaxLayer++;

	TArray<FVector2D> points;
	points.Add(FVector2D(x1, y1));
	points.Add(FVector2D(x2, y2));

#if ENGINE_MINOR_VERSION >= 17
	FSlateDrawElement::MakeLines(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(),
		points, ESlateDrawEffect::None, tint, (py_antialias && !PyObject_IsTrue(py_antialias)), thickness);
#else
	FSlateDrawElement::MakeLines(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(),
		points, context.MyClippingRect, ESlateDrawEffect::None, tint, (py_antialias && !PyObject_IsTrue(py_antialias)), thickness);
#endif

	Py_RETURN_NONE;
}

static PyObject *py_ue_fpaint_context_draw_box(ue_PyFPaintContext *self, PyObject * args)
{
	float x, y, w, h;

	PyObject *py_brush = nullptr;
	PyObject *py_linear_color = nullptr;

	if (!PyArg_ParseTuple(args, "(ff)(ff)|OO:draw_box", &x, &y, &w, &h, &py_linear_color, &py_brush))
		return nullptr;

	FLinearColor tint = FLinearColor::White;

	if (py_linear_color)
	{
		if (!py_ue_get_flinearcolor(py_linear_color, tint))
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor or FColor.");
		}
	}

	FSlateBrush *brush = &global_simple_brush;

	if (py_brush)
	{
		FSlateBrush *custom_brush = ue_py_check_struct<FSlateBrush>(py_brush);
		if (!custom_brush)
			return PyErr_Format(PyExc_Exception, "argument is not a FSlateBrush");
		brush = custom_brush;
	}

	FPaintContext context = self->paint_context;
	context.MaxLayer++;

#if ENGINE_MINOR_VERSION >= 17
	FSlateDrawElement::MakeBox(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(FVector2D(x, y), FVector2D(w, h)),
		brush, ESlateDrawEffect::None, tint);
#else
	FSlateDrawElement::MakeBox(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(FVector2D(x, y), FVector2D(w, h)),
		brush, context.MyClippingRect, ESlateDrawEffect::None, tint);
#endif

	Py_RETURN_NONE;
}


static PyObject *py_ue_fpaint_context_draw_text(ue_PyFPaintContext *self, PyObject * args)
{
	float x, y;
	char *text;
	float size = 0;
	PyObject *py_linear_color = nullptr;
	PyObject *py_font = nullptr;

	if (!PyArg_ParseTuple(args, "(ff)s|fOO:draw_text", &x, &y, &text, &size, &py_linear_color, &py_font))
		return nullptr;

	FLinearColor tint = FLinearColor::White;
	FSlateFontInfo font = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;

	if (py_linear_color)
	{
		if (!py_ue_get_flinearcolor(py_linear_color, tint))
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor or FColor.");
		}
	}

	if (py_font)
	{
		FSlateFontInfo *font_struct = ue_py_check_struct<FSlateFontInfo>(py_font);
		if (font_struct)
		{
			font = *font_struct;
		}
		else
		{
			return PyErr_Format(PyExc_Exception, "argument is not a SlateFontInfo USTRUCT");
		}
	}
	if (size <= 0)
		size = font.Size;

	font.Size = size;

	FVector2D position = FVector2D(x, y);

	FPaintContext context = self->paint_context;

	context.MaxLayer++;

#if ENGINE_MINOR_VERSION >= 17
	FSlateDrawElement::MakeText(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToOffsetPaintGeometry(position),
		FText::FromString(UTF8_TO_TCHAR(text)), font, ESlateDrawEffect::None, tint);
#else
	FSlateDrawElement::MakeText(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToOffsetPaintGeometry(position),
		FText::FromString(UTF8_TO_TCHAR(text)), font, context.MyClippingRect, ESlateDrawEffect::None, tint);
#endif

	Py_RETURN_NONE;
}

static PyObject *py_ue_fpaint_context_draw_spline(ue_PyFPaintContext *self, PyObject * args)
{
	float x1, y1, dx1, dy1;
	float x2, y2, dx2, dy2;
	PyObject *py_linear_color = nullptr;

	float thickness = 1.0;

	if (!PyArg_ParseTuple(args, "(ff)(ff)(ff)(ff)|Of:draw_spline", &x1, &y1, &dx1, &dy1, &x2, &y2, &dx2, &dy2, &py_linear_color, &thickness))
		return nullptr;

	FLinearColor tint = FLinearColor::White;

	if (py_linear_color)
	{
		if (!py_ue_get_flinearcolor(py_linear_color, tint))
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor or FColor.");
		}
	}

	FPaintContext context = self->paint_context;

	context.MaxLayer++;

#if ENGINE_MINOR_VERSION >= 17
	FSlateDrawElement::MakeSpline(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(),
		FVector2D(x1, y1), FVector2D(dx1, dy1), FVector2D(x2, y2), FVector2D(dx2, dy2),
		thickness, ESlateDrawEffect::None, tint);
#else
	FSlateDrawElement::MakeSpline(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(),
		FVector2D(x1, y1), FVector2D(dx1, dy1), FVector2D(x2, y2), FVector2D(dx2, dy2),
		context.MyClippingRect, thickness, ESlateDrawEffect::None, tint);
#endif

	Py_RETURN_NONE;
}


static PyObject *py_ue_fpaint_context_draw_lines(ue_PyFPaintContext *self, PyObject * args)
{
	PyObject *py_points;
	PyObject *py_linear_color = nullptr;
	PyObject *py_antialias = nullptr;
	float thickness = 1.0;

	if (!PyArg_ParseTuple(args, "O|OOf:draw_lines", &py_points, &py_linear_color, &py_antialias, &thickness))
		return nullptr;

	TArray<FVector2D> points;

	PyObject *py_iter = PyObject_GetIter(py_points);
	if (!py_iter)
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of tuples");

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		if (!PyTuple_Check(py_item))
		{
			return PyErr_Format(PyExc_Exception, "iterable item is not a tuple");
			Py_DECREF(py_iter);
		}

		float x, y;
		if (!PyArg_ParseTuple(py_item, "ff", &x, &y))
		{
			Py_DECREF(py_iter);
			return nullptr;
		}
		points.Add(FVector2D(x, y));
	}
	Py_DECREF(py_iter);

	FLinearColor tint = FLinearColor::White;

	if (py_linear_color)
	{
		if (!py_ue_get_flinearcolor(py_linear_color, tint))
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor or FColor.");
		}
	}

	FPaintContext context = self->paint_context;

	context.MaxLayer++;

#if ENGINE_MINOR_VERSION >= 17
	FSlateDrawElement::MakeLines(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(),
		points, ESlateDrawEffect::None, tint, (py_antialias && !PyObject_IsTrue(py_antialias)), thickness);
#else
	FSlateDrawElement::MakeLines(context.OutDrawElements, context.MaxLayer, context.AllottedGeometry.ToPaintGeometry(),
		points, context.MyClippingRect, ESlateDrawEffect::None, tint, (py_antialias && !PyObject_IsTrue(py_antialias)), thickness);
#endif

	Py_RETURN_NONE;
}

static PyObject *py_ue_fpaint_context_get_geometry(ue_PyFPaintContext *self, PyObject * args)
{
	return py_ue_new_fgeometry(self->paint_context.AllottedGeometry);
}

static PyMethodDef ue_PyFPaintContext_methods[] = {
	{ "get_geometry", (PyCFunction)py_ue_fpaint_context_get_geometry, METH_VARARGS, "" },
	{ "draw_line", (PyCFunction)py_ue_fpaint_context_draw_line, METH_VARARGS, "" },
	{ "draw_box", (PyCFunction)py_ue_fpaint_context_draw_box, METH_VARARGS, "" },
	{ "draw_lines", (PyCFunction)py_ue_fpaint_context_draw_lines, METH_VARARGS, "" },
	{ "draw_spline", (PyCFunction)py_ue_fpaint_context_draw_spline, METH_VARARGS, "" },
	{ "draw_text", (PyCFunction)py_ue_fpaint_context_draw_text, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFPaintContext_str(ue_PyFPaintContext *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FPaintContext '%s'>",
		TCHAR_TO_UTF8(*self->paint_context.AllottedGeometry.ToString()));
}

static PyTypeObject ue_PyFPaintContextType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FPaintContext", /* tp_name */
	sizeof(ue_PyFPaintContext), /* tp_basicsize */
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
	(reprfunc)ue_PyFPaintContext_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FPaintContext",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFPaintContext_methods,             /* tp_methods */
};

void ue_python_init_fpaint_context(PyObject *ue_module)
{
	ue_PyFPaintContextType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFPaintContextType) < 0)
		return;

	Py_INCREF(&ue_PyFPaintContextType);
	PyModule_AddObject(ue_module, "FPaintContext", (PyObject *)&ue_PyFPaintContextType);
}

PyObject *py_ue_new_fpaint_context(FPaintContext paint_context)
{
	ue_PyFPaintContext *ret = (ue_PyFPaintContext *)PyObject_New(ue_PyFPaintContext, &ue_PyFPaintContextType);
	ret->paint_context = paint_context;
	return (PyObject *)ret;
}
