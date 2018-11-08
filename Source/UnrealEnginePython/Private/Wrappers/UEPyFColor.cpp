#include "UEPyFColor.h"

static PyObject *py_ue_fcolor_to_hex(ue_PyFColor *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->color.ToString()));
}

static PyObject *py_ue_fcolor_to_linear(ue_PyFColor *self, PyObject * args)
{
	return py_ue_new_flinearcolor(self->color.ReinterpretAsLinear());
}

static PyMethodDef ue_PyFColor_methods[] = {
	{ "to_hex", (PyCFunction)py_ue_fcolor_to_hex, METH_VARARGS, "" },
	{ "to_linear", (PyCFunction)py_ue_fcolor_to_linear, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_fcolor_get_r(ue_PyFColor *self, void *closure)
{
	return PyLong_FromLong(self->color.R);
}

static int py_ue_fcolor_set_r(ue_PyFColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Long(value);
		self->color.R = PyLong_AsLong(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fcolor_get_g(ue_PyFColor *self, void *closure)
{
	return PyLong_FromLong(self->color.G);
}

static int py_ue_fcolor_set_g(ue_PyFColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Long(value);
		self->color.G = PyLong_AsLong(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fcolor_get_b(ue_PyFColor *self, void *closure)
{
	return PyLong_FromLong(self->color.B);
}

static int py_ue_fcolor_set_b(ue_PyFColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Long(value);
		self->color.B = PyLong_AsLong(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fcolor_get_a(ue_PyFColor *self, void *closure)
{
	return PyLong_FromLong(self->color.A);
}

static int py_ue_fcolor_set_a(ue_PyFColor *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Long(value);
		self->color.A = PyLong_AsLong(l_value);
		Py_DECREF(l_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}


static PyGetSetDef ue_PyFColor_getseters[] = {
	{ (char*)"r", (getter)py_ue_fcolor_get_r, (setter)py_ue_fcolor_set_r, (char *)"", NULL },
	{ (char *)"g", (getter)py_ue_fcolor_get_g, (setter)py_ue_fcolor_set_g, (char *)"", NULL },
	{ (char *)"b", (getter)py_ue_fcolor_get_b, (setter)py_ue_fcolor_set_b, (char *)"", NULL },
	{ (char *)"a", (getter)py_ue_fcolor_get_a, (setter)py_ue_fcolor_set_a, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFColor_str(ue_PyFColor *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FColor {'r': %d, 'g': %d, 'b': %d, 'a': %d}>",
		self->color.R, self->color.G, self->color.B, self->color.A);
}

PyTypeObject ue_PyFColorType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FColor", /* tp_name */
	sizeof(ue_PyFColor), /* tp_basicsize */
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
	(reprfunc)ue_PyFColor_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FColor",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFColor_methods,             /* tp_methods */
	0,
	ue_PyFColor_getseters,
};


static PyObject *ue_py_fcolor_add(ue_PyFColor *self, PyObject *value)
{
	FColor color = self->color;
	ue_PyFColor *py_color = py_ue_is_fcolor(value);
	if (py_color)
	{
		color += py_color->color;
	}
	else if (PyNumber_Check(value))
	{
		PyObject *l_value = PyNumber_Long(value);
		long l = PyLong_AsLong(l_value);
		color.R += l;
		color.G += l;
		color.B += l;
		color.A += l;
		Py_DECREF(l_value);
	}
	return py_ue_new_fcolor(color);
}

PyNumberMethods ue_PyFColor_number_methods;

static Py_ssize_t ue_py_fcolor_seq_length(ue_PyFColor *self)
{
	return 4;
}

static PyObject *ue_py_fcolor_seq_item(ue_PyFColor *self, Py_ssize_t i)
{
	switch (i)
	{
	case 0:
		return PyLong_FromLong(self->color.R);
	case 1:
		return PyLong_FromLong(self->color.G);
	case 2:
		return PyLong_FromLong(self->color.B);
	case 3:
		return PyLong_FromLong(self->color.A);
	}
	return PyErr_Format(PyExc_IndexError, "FColor has only 4 items");
}

PySequenceMethods ue_PyFColor_sequence_methods;

static int ue_py_fcolor_init(ue_PyFColor *self, PyObject *args, PyObject *kwargs)
{
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 255;
	if (!PyArg_ParseTuple(args, "|iiii", &r, &g, &b, &a))
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

static void fcolor_add_color(const char *color_name, FColor fcolor)
{
	PyObject *color = py_ue_new_fcolor(fcolor);
	// not required
	Py_INCREF(color);
	PyDict_SetItemString(ue_PyFColorType.tp_dict, color_name, color);
}

void ue_python_init_fcolor(PyObject *ue_module)
{
	ue_PyFColorType.tp_new = PyType_GenericNew;

	ue_PyFColorType.tp_init = (initproc)ue_py_fcolor_init;

	memset(&ue_PyFColor_number_methods, 0, sizeof(PyNumberMethods));
	ue_PyFColorType.tp_as_number = &ue_PyFColor_number_methods;
	ue_PyFColor_number_methods.nb_add = (binaryfunc)ue_py_fcolor_add;

	memset(&ue_PyFColor_sequence_methods, 0, sizeof(PySequenceMethods));
	ue_PyFColorType.tp_as_sequence = &ue_PyFColor_sequence_methods;
	ue_PyFColor_sequence_methods.sq_length = (lenfunc)ue_py_fcolor_seq_length;
	ue_PyFColor_sequence_methods.sq_item = (ssizeargfunc)ue_py_fcolor_seq_item;

	if (PyType_Ready(&ue_PyFColorType) < 0)
		return;

	Py_INCREF(&ue_PyFColorType);
	PyModule_AddObject(ue_module, "FColor", (PyObject *)&ue_PyFColorType);

	fcolor_add_color("Black", FColor::Black);
	fcolor_add_color("Blue", FColor::Blue);
	fcolor_add_color("Cyan", FColor::Cyan);
	fcolor_add_color("Emerald", FColor::Emerald);
	fcolor_add_color("Green", FColor::Green);
	fcolor_add_color("Magenta", FColor::Magenta);
	fcolor_add_color("Orange", FColor::Orange);
	fcolor_add_color("Purple", FColor::Purple);
	fcolor_add_color("Red", FColor::Red);
	fcolor_add_color("Silver", FColor::Silver);
	fcolor_add_color("Turquoise", FColor::Turquoise);
	fcolor_add_color("White", FColor::White);
	fcolor_add_color("Yellow", FColor::Yellow);
}

PyObject *py_ue_new_fcolor(FColor color)
{
	ue_PyFColor *ret = (ue_PyFColor *)PyObject_New(ue_PyFColor, &ue_PyFColorType);
	ret->color = color;
	return (PyObject *)ret;
}

ue_PyFColor *py_ue_is_fcolor(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFColorType))
		return nullptr;
	return (ue_PyFColor *)obj;
}

bool py_ue_get_fcolor(PyObject *obj, FColor &color)
{

	if (ue_PyFColor *py_fcolor = py_ue_is_fcolor(obj))
	{
		color = py_fcolor->color;
		return true;
	}


	if (ue_PyFLinearColor *py_flinearcolor = py_ue_is_flinearcolor(obj))
	{
		color = py_flinearcolor->color.ToFColor(true);
		return true;
	}

	return false;
}

bool py_ue_color_arg(PyObject *args, FColor &color)
{

	if (PyTuple_Size(args) == 1)
	{
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFColor *py_color = py_ue_is_fcolor(arg);
		if (!py_color)
		{
			PyErr_Format(PyExc_TypeError, "argument is not a FColor");
			return false;
		}
		color = py_color->color;
		return true;
	}

	int r = 0;
	int g = 0;
	int b = 0;
	int a = 255;
	if (!PyArg_ParseTuple(args, "iii|i", &r, &g, &b, &a))
		return false;
	color.R = r;
	color.G = g;
	color.B = b;
	color.A = a;
	return true;
}

