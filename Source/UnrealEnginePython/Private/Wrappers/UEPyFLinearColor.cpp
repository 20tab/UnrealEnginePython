#include "UEPyFLinearColor.h"

static PyObject *py_ue_flinearcolor_to_fcolor(ue_PyFLinearColor *self, PyObject * args)
{
	PyObject *py_srgb = nullptr;
	if (!PyArg_ParseTuple(args, "|O:to_fcolor", &py_srgb))
		return NULL;
	bool b_srgb = false;
	if (py_srgb && PyObject_IsTrue(py_srgb))
		b_srgb = true;
	return py_ue_new_fcolor(self->color.ToFColor(b_srgb));
}

static PyMethodDef ue_PyFLinearColor_methods[] = {
	{ "to_fcolor", (PyCFunction)py_ue_flinearcolor_to_fcolor, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_flinearcolor_get_r(ue_PyFLinearColor *self, void *closure)
{
	return PyFloat_FromDouble(self->color.R);
}

static int py_ue_flinearcolor_set_r(ue_PyFLinearColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Float(value);
		self->color.R = PyFloat_AsDouble(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_flinearcolor_get_g(ue_PyFLinearColor *self, void *closure)
{
	return PyFloat_FromDouble(self->color.G);
}

static int py_ue_flinearcolor_set_g(ue_PyFLinearColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Float(value);
		self->color.G = PyFloat_AsDouble(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_flinearcolor_get_b(ue_PyFLinearColor *self, void *closure)
{
	return PyFloat_FromDouble(self->color.B);
}

static int py_ue_flinearcolor_set_b(ue_PyFLinearColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Float(value);
		self->color.B = PyFloat_AsDouble(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_flinearcolor_get_a(ue_PyFLinearColor *self, void *closure)
{
	return PyFloat_FromDouble(self->color.A);
}

static int py_ue_flinearcolor_set_a(ue_PyFLinearColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Float(value);
		self->color.A = PyFloat_AsDouble(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}


static PyGetSetDef ue_PyFLinearColor_getseters[] = {
	{ (char*)"r", (getter)py_ue_flinearcolor_get_r, (setter)py_ue_flinearcolor_set_r, (char *)"", NULL },
	{ (char *)"g", (getter)py_ue_flinearcolor_get_g, (setter)py_ue_flinearcolor_set_g, (char *)"", NULL },
	{ (char *)"b", (getter)py_ue_flinearcolor_get_b, (setter)py_ue_flinearcolor_set_b, (char *)"", NULL },
	{ (char *)"a", (getter)py_ue_flinearcolor_get_a, (setter)py_ue_flinearcolor_set_a, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFLinearColor_str(ue_PyFLinearColor *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FLinearColor {'r': %S, 'g': %S, 'b': %S, 'a': %S}>",
		PyFloat_FromDouble(self->color.R),
		PyFloat_FromDouble(self->color.G),
		PyFloat_FromDouble(self->color.B),
		PyFloat_FromDouble(self->color.A));
}

PyTypeObject ue_PyFLinearColorType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FLinearColor", /* tp_name */
	sizeof(ue_PyFLinearColor), /* tp_basicsize */
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
	(reprfunc)ue_PyFLinearColor_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FLinearColor",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFLinearColor_methods,             /* tp_methods */
	0,
	ue_PyFLinearColor_getseters,
};


static PyObject *ue_py_flinearcolor_add(ue_PyFLinearColor *self, PyObject *value)
{
	FLinearColor color = self->color;
	ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(value);
	if (py_color)
	{
		color += py_color->color;
	}
	else if (PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Float(value);
		long l = PyFloat_AsDouble(l_value);
		color.R += l;
		color.G += l;
		color.B += l;
		color.A += l;
		Py_DECREF(l_value);
	}
	return py_ue_new_flinearcolor(color);
}

PyNumberMethods ue_PyFLinearColor_number_methods;

static Py_ssize_t ue_py_flinearcolor_seq_length(ue_PyFLinearColor *self)
{
	return 4;
}

static PyObject *ue_py_flinearcolor_seq_item(ue_PyFLinearColor *self, Py_ssize_t i)
{
	switch (i)
	{
	case 0:
		return PyFloat_FromDouble(self->color.R);
	case 1:
		return PyFloat_FromDouble(self->color.G);
	case 2:
		return PyFloat_FromDouble(self->color.B);
	case 3:
		return PyFloat_FromDouble(self->color.A);
	}
	return PyErr_Format(PyExc_IndexError, "FLinearColor has only 4 items");
}

PySequenceMethods ue_PyFLinearColor_sequence_methods;

static int ue_py_flinearcolor_init(ue_PyFLinearColor *self, PyObject *args, PyObject *kwargs)
{
	float r = 0;
	float g = 0;
	float b = 0;
	float a = 1.0;
	if (!PyArg_ParseTuple(args, "|ffff", &r, &g, &b, &a))
		return -1;

	if (PyTuple_Size(args) == 1)
	{
		g = a;
		b = a;
	}

	self->color.R = r;
	self->color.G = g;
	self->color.B = b;
	self->color.A = a;
	return 0;
}

static void flinearcolor_add_color(const char *color_name, FLinearColor lcolor)
{
	PyObject *color = py_ue_new_flinearcolor(lcolor);
	// not required
	Py_INCREF(color);
	PyDict_SetItemString(ue_PyFLinearColorType.tp_dict, color_name, color);
}

void ue_python_init_flinearcolor(PyObject *ue_module)
{
	ue_PyFLinearColorType.tp_new = PyType_GenericNew;

	ue_PyFLinearColorType.tp_init = (initproc)ue_py_flinearcolor_init;

	memset(&ue_PyFLinearColor_number_methods, 0, sizeof(PyNumberMethods));
	ue_PyFLinearColorType.tp_as_number = &ue_PyFLinearColor_number_methods;
	ue_PyFLinearColor_number_methods.nb_add = (binaryfunc)ue_py_flinearcolor_add;

	memset(&ue_PyFLinearColor_sequence_methods, 0, sizeof(PySequenceMethods));
	ue_PyFLinearColorType.tp_as_sequence = &ue_PyFLinearColor_sequence_methods;
	ue_PyFLinearColor_sequence_methods.sq_length = (lenfunc)ue_py_flinearcolor_seq_length;
	ue_PyFLinearColor_sequence_methods.sq_item = (ssizeargfunc)ue_py_flinearcolor_seq_item;

	if (PyType_Ready(&ue_PyFLinearColorType) < 0)
		return;

	Py_INCREF(&ue_PyFLinearColorType);
	PyModule_AddObject(ue_module, "FLinearColor", (PyObject *)&ue_PyFLinearColorType);

	flinearcolor_add_color("Black", FLinearColor::Black);
	flinearcolor_add_color("Blue", FLinearColor::Blue);
	flinearcolor_add_color("Gray", FLinearColor::Gray);
	flinearcolor_add_color("Green", FLinearColor::Green);
	flinearcolor_add_color("Red", FLinearColor::Red);
	flinearcolor_add_color("White", FLinearColor::White);
	flinearcolor_add_color("Yellow", FLinearColor::Yellow);
}

PyObject *py_ue_new_flinearcolor(FLinearColor color)
{
	ue_PyFLinearColor *ret = (ue_PyFLinearColor *)PyObject_New(ue_PyFLinearColor, &ue_PyFLinearColorType);
	ret->color = color;
	return (PyObject *)ret;
}

ue_PyFLinearColor *py_ue_is_flinearcolor(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFLinearColorType))
		return nullptr;
	return (ue_PyFLinearColor *)obj;
}

bool py_ue_get_flinearcolor(PyObject *obj, FLinearColor &color)
{

	if (ue_PyFLinearColor *py_flinearcolor = py_ue_is_flinearcolor(obj))
	{
		color = py_flinearcolor->color;
		return true;
	}

	if (ue_PyFColor *py_fcolor = py_ue_is_fcolor(obj))
	{
		color = py_fcolor->color;
		return true;
	}


	return false;
}

bool py_ue_color_arg(PyObject *args, FLinearColor &color)
{

	if (PyTuple_Size(args) == 1)
	{
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(arg);
		if (!py_color)
		{
			PyErr_Format(PyExc_TypeError, "argument is not a FLinearColor");
			return false;
		}
		color = py_color->color;
		return true;
	}

	float r = 0;
	float g = 0;
	float b = 0;
	float a = 1;
	if (!PyArg_ParseTuple(args, "fff|f", &r, &g, &b, &a))
		return false;
	color.R = r;
	color.G = g;
	color.B = b;
	color.A = a;
	return true;
}

