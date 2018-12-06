#include "UEPyFVector2D.h"

static PyObject *py_ue_fvector2d_length(ue_PyFVector2D *self, PyObject * args)
{
	return PyFloat_FromDouble(self->vec.Size());
}

static PyObject *py_ue_fvector2d_length_squared(ue_PyFVector2D *self, PyObject * args)
{
	return PyFloat_FromDouble(self->vec.SizeSquared());
}

static PyObject *py_ue_fvector2d_normalized(ue_PyFVector2D *self, PyObject * args)
{
	FVector2D vec = self->vec;
	vec.Normalize();
	return py_ue_new_fvector2d(vec);
}

static PyObject *py_ue_fvector2d_dot(ue_PyFVector2D *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:dot", &py_obj))
		return NULL;
	ue_PyFVector2D *py_vec = py_ue_is_fvector2d(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_TypeError, "argument is not a FVector2D");
	return PyFloat_FromDouble(FVector2D::DotProduct(self->vec, py_vec->vec));
}

static PyObject *py_ue_fvector2d_cross(ue_PyFVector2D *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:cross", &py_obj))
		return NULL;
	ue_PyFVector2D *py_vec = py_ue_is_fvector2d(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_TypeError, "argument is not a FVector2D");
	return PyFloat_FromDouble(FVector2D::CrossProduct(self->vec, py_vec->vec));
}

static PyMethodDef ue_PyFVector2D_methods[] = {

	{ "length", (PyCFunction)py_ue_fvector2d_length, METH_VARARGS, "" },
	{ "size", (PyCFunction)py_ue_fvector2d_length, METH_VARARGS, "" },
	{ "size_squared", (PyCFunction)py_ue_fvector2d_length_squared, METH_VARARGS, "" },
	{ "length_squared", (PyCFunction)py_ue_fvector2d_length_squared, METH_VARARGS, "" },
	{ "normalized", (PyCFunction)py_ue_fvector2d_normalized, METH_VARARGS, "" },
	{ "dot", (PyCFunction)py_ue_fvector2d_dot, METH_VARARGS, "" },
	{ "cross", (PyCFunction)py_ue_fvector2d_cross, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_fvector2d_get_x(ue_PyFVector2D *self, void *closure)
{
	return PyFloat_FromDouble(self->vec.X);
}

static int py_ue_fvector2d_set_x(ue_PyFVector2D *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		self->vec.X = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fvector2d_get_y(ue_PyFVector2D *self, void *closure)
{
	return PyFloat_FromDouble(self->vec.Y);
}

static int py_ue_fvector2d_set_y(ue_PyFVector2D *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		self->vec.Y = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}


static PyGetSetDef ue_PyFVector2D_getseters[] = {
	{(char *) "x", (getter)py_ue_fvector2d_get_x, (setter)py_ue_fvector2d_set_x, (char *)"", NULL },
	{(char *) "y", (getter)py_ue_fvector2d_get_y, (setter)py_ue_fvector2d_set_y, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFVector2D_str(ue_PyFVector2D *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FVector2D {'x': %S, 'y': %S}>",
		PyFloat_FromDouble(self->vec.X), PyFloat_FromDouble(self->vec.Y));
}

PyTypeObject ue_PyFVector2DType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FVector2D", /* tp_name */
	sizeof(ue_PyFVector2D), /* tp_basicsize */
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
	(reprfunc)ue_PyFVector2D_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FVector2D",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFVector2D_methods,             /* tp_methods */
	0,
	ue_PyFVector2D_getseters,
};


static PyObject *ue_py_fvector2d_add(ue_PyFVector2D *self, PyObject *value)
{
	FVector2D vec = self->vec;
	ue_PyFVector2D *py_vec = py_ue_is_fvector2d(value);
	if (py_vec)
	{
		vec += py_vec->vec;
	}
	else if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		vec.X += f;
		vec.Y += f;
		Py_DECREF(f_value);
	}
	return py_ue_new_fvector2d(vec);
}

static PyObject *ue_py_fvector2d_sub(ue_PyFVector2D *self, PyObject *value)
{
	FVector2D vec = self->vec;
	ue_PyFVector2D *py_vec = py_ue_is_fvector2d(value);
	if (py_vec)
	{
		vec -= py_vec->vec;
	}
	else if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		vec.X -= f;
		vec.Y -= f;
		Py_DECREF(f_value);
	}
	return py_ue_new_fvector2d(vec);
}

static PyObject *ue_py_fvector2d_mul(ue_PyFVector2D *self, PyObject *value)
{
	FVector2D vec = self->vec;
	ue_PyFVector2D *py_vec = py_ue_is_fvector2d(value);
	if (py_vec)
	{
		vec *= py_vec->vec;
	}
	else if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		vec *= f;
		Py_DECREF(f_value);
	}
	return py_ue_new_fvector2d(vec);
}

static PyObject *ue_py_fvector2d_div(ue_PyFVector2D *self, PyObject *value)
{
	FVector2D vec = self->vec;
	ue_PyFVector2D *py_vec = py_ue_is_fvector2d(value);
	if (py_vec)
	{
		if (py_vec->vec.X == 0 || py_vec->vec.Y == 0)
			return PyErr_Format(PyExc_ZeroDivisionError, "division by zero");
		vec /= py_vec->vec;
	}
	else if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		if (f == 0)
			return PyErr_Format(PyExc_ZeroDivisionError, "division by zero");
		vec /= f;
		Py_DECREF(f_value);
	}
	return py_ue_new_fvector2d(vec);
}

static PyObject *ue_py_fvector2d_floor_div(ue_PyFVector2D *self, PyObject *value)
{
	FVector2D vec = self->vec;
	if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		if (f == 0)
			return PyErr_Format(PyExc_ZeroDivisionError, "division by zero");
		vec.X = floor(vec.X / f);
		vec.Y = floor(vec.Y / f);
		Py_DECREF(f_value);
		return py_ue_new_fvector2d(vec);
	}
	return PyErr_Format(PyExc_TypeError, "value is not numeric");
}

PyNumberMethods ue_PyFVector2D_number_methods;

static Py_ssize_t ue_py_fvector2d_seq_length(ue_PyFVector2D *self)
{
	return 2;
}

static PyObject *ue_py_fvector2d_seq_item(ue_PyFVector2D *self, Py_ssize_t i)
{
	switch (i)
	{
	case 0:
		return PyFloat_FromDouble(self->vec.X);
	case 1:
		return PyFloat_FromDouble(self->vec.Y);
	}
	return PyErr_Format(PyExc_IndexError, "FVector2D has only 2 items");
}

PySequenceMethods ue_PyFVector2D_sequence_methods;

static int ue_py_fvector2d_init(ue_PyFVector2D *self, PyObject *args, PyObject *kwargs)
{
	float x = 0, y = 0;
	if (!PyArg_ParseTuple(args, "|ff", &x, &y))
		return -1;

	if (PyTuple_Size(args) == 1)
	{
		y = x;
	}

	self->vec.X = x;
	self->vec.Y = y;

	return 0;
}

static PyObject *ue_py_fvector2d_richcompare(ue_PyFVector2D *vec1, PyObject *b, int op)
{
	ue_PyFVector2D *vec2 = py_ue_is_fvector2d(b);
	if (!vec2 || (op != Py_EQ && op != Py_NE))
	{
		return PyErr_Format(PyExc_NotImplementedError, "can only compare with another FVector2D");
	}

	if (op == Py_EQ)
	{
		if (vec1->vec.X == vec2->vec.X &&
			vec1->vec.Y == vec2->vec.Y)
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
		Py_INCREF(Py_False);
		return Py_False;
	}

	if (vec1->vec.X == vec2->vec.X &&
		vec1->vec.Y == vec2->vec.Y)
	{
		Py_INCREF(Py_False);
		return Py_False;
	}
	Py_INCREF(Py_True);
	return Py_True;
}

void ue_python_init_fvector2d(PyObject *ue_module)
{
	ue_PyFVector2DType.tp_new = PyType_GenericNew;

	ue_PyFVector2DType.tp_init = (initproc)ue_py_fvector2d_init;
	ue_PyFVector2DType.tp_richcompare = (richcmpfunc)ue_py_fvector2d_richcompare;

	memset(&ue_PyFVector2D_number_methods, 0, sizeof(PyNumberMethods));
	ue_PyFVector2DType.tp_as_number = &ue_PyFVector2D_number_methods;
	ue_PyFVector2D_number_methods.nb_add = (binaryfunc)ue_py_fvector2d_add;
	ue_PyFVector2D_number_methods.nb_subtract = (binaryfunc)ue_py_fvector2d_sub;
	ue_PyFVector2D_number_methods.nb_multiply = (binaryfunc)ue_py_fvector2d_mul;
	ue_PyFVector2D_number_methods.nb_true_divide = (binaryfunc)ue_py_fvector2d_div;
	ue_PyFVector2D_number_methods.nb_floor_divide = (binaryfunc)ue_py_fvector2d_floor_div;

	memset(&ue_PyFVector2D_sequence_methods, 0, sizeof(PySequenceMethods));
	ue_PyFVector2DType.tp_as_sequence = &ue_PyFVector2D_sequence_methods;
	ue_PyFVector2D_sequence_methods.sq_length = (lenfunc)ue_py_fvector2d_seq_length;
	ue_PyFVector2D_sequence_methods.sq_item = (ssizeargfunc)ue_py_fvector2d_seq_item;

	if (PyType_Ready(&ue_PyFVector2DType) < 0)
		return;

	Py_INCREF(&ue_PyFVector2DType);
	PyModule_AddObject(ue_module, "FVector2D", (PyObject *)&ue_PyFVector2DType);
}

PyObject *py_ue_new_fvector2d(FVector2D vec)
{
	ue_PyFVector2D *ret = (ue_PyFVector2D *)PyObject_New(ue_PyFVector2D, &ue_PyFVector2DType);
	ret->vec = vec;
	return (PyObject *)ret;
}

ue_PyFVector2D *py_ue_is_fvector2d(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFVector2DType))
		return nullptr;
	return (ue_PyFVector2D *)obj;
}

bool py_ue_vector2d_arg(PyObject *args, FVector2D &vec)
{

	if (PyTuple_Size(args) == 1)
	{
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFVector2D *py_vec = py_ue_is_fvector2d(arg);
		if (!py_vec)
		{
			PyErr_Format(PyExc_TypeError, "argument is not a FVector2D");
			return false;
		}
		vec = py_vec->vec;
		return true;
	}

	float x, y;
	if (!PyArg_ParseTuple(args, "ff", &x, &y))
		return false;
	vec.X = x;
	vec.Y = y;
	return true;
}

