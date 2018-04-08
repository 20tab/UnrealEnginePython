#include "UEPySImage.h"

#include "Engine/Texture2D.h"

static PyObject *py_ue_simage_set_brush(ue_PySImage *self, PyObject * args)
{
	ue_py_slate_cast(SImage);
	PyObject *py_brush;
	if (!PyArg_ParseTuple(args, "O:set_brush", &py_brush))
	{
		return nullptr;
	}

	FSlateBrush *brush = ue_py_check_struct<FSlateBrush>(py_brush);
	if (!brush)
		return PyErr_Format(PyExc_Exception, "argument is not a FSlateBrush");

	self->brush = *brush;

	py_SImage->SetImage(&self->brush);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_simage_set_texture(ue_PySImage *self, PyObject * args)
{
	ue_py_slate_cast(SImage);
	PyObject *py_texture;
	PyObject *py_linear_color = nullptr;
	if (!PyArg_ParseTuple(args, "O|O:set_texture", &py_texture, &py_linear_color))
	{
		return nullptr;
	}

	UTexture2D *texture = ue_py_check_type<UTexture2D>(py_texture);
	if (!texture)
		return PyErr_Format(PyExc_Exception, "argument is not a Texture");

	FLinearColor tint(1, 1, 1, 1);

	if (py_linear_color)
	{
		ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(py_linear_color);
		if (!py_color)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor");
		}
		tint = py_color->color;
	}

#if ENGINE_MINOR_VERSION > 15
	self->brush = FSlateImageBrush(texture, FVector2D(texture->GetSurfaceWidth(), texture->GetSurfaceHeight()), tint);
#else
	self->brush = FSlateBrush();
	self->brush.ImageSize = FVector2D(texture->GetSurfaceWidth(), texture->GetSurfaceHeight());
	self->brush.SetResourceObject(texture);
	self->brush.TintColor = tint;
#endif

	py_SImage->SetImage(&self->brush);

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySImage_methods[] = {
	{ "set_brush", (PyCFunction)py_ue_simage_set_brush, METH_VARARGS, "" },
	{ "set_image", (PyCFunction)py_ue_simage_set_brush, METH_VARARGS, "" },
	{ "set_texture", (PyCFunction)py_ue_simage_set_texture, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySImageType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SImage", /* tp_name */
	sizeof(ue_PySImage), /* tp_basicsize */
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
	"Unreal Engine SImage",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySImage_methods,             /* tp_methods */
};

static int ue_py_simage_init(ue_PySImage *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SImage);

	ue_py_slate_farguments_struct("color_and_opacity", ColorAndOpacity, FSlateColor);
	ue_py_slate_farguments_optional_struct_ptr("image", Image, FSlateBrush);
	ue_py_slate_farguments_event("on_mouse_button_down", OnMouseButtonDown, FPointerEventHandler, OnMouseEvent);

	ue_py_snew(SImage);
	return 0;
}

void ue_python_init_simage(PyObject *ue_module)
{

	ue_PySImageType.tp_init = (initproc)ue_py_simage_init;

	ue_PySImageType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySImageType) < 0)
		return;

	Py_INCREF(&ue_PySImageType);
	PyModule_AddObject(ue_module, "SImage", (PyObject *)&ue_PySImageType);
}
