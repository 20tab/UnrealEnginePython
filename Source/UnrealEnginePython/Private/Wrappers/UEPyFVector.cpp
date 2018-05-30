#include "UEPyFVector.h"

static PyObject *py_ue_fvector_length(ue_PyFVector *self, PyObject * args)
{
	return PyFloat_FromDouble(py_ue_fvector_get(self).Size());
}

static PyObject *py_ue_fvector_length_squared(ue_PyFVector *self, PyObject * args)
{
	return PyFloat_FromDouble(py_ue_fvector_get(self).SizeSquared());
}

static PyObject *py_ue_fvector_normalized(ue_PyFVector *self, PyObject * args)
{
	FVector vec = py_ue_fvector_get(self);
	vec.Normalize();
	return py_ue_new_fvector(vec);
}

static PyObject *py_ue_fvector_rotation(ue_PyFVector *self, PyObject * args)
{
	FRotator rot = py_ue_fvector_get(self).Rotation();
	return py_ue_new_frotator(rot);
}

static PyObject *py_ue_fvector_dot(ue_PyFVector *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:dot", &py_obj))
		return NULL;
	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_TypeError, "argument is not a FVector");
	return PyFloat_FromDouble(FVector::DotProduct(py_ue_fvector_get(self), py_ue_fvector_get(py_vec)));
}

static PyObject *py_ue_fvector_cross(ue_PyFVector *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:cross", &py_obj))
		return NULL;
	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_TypeError, "argument is not a FVector");
	return py_ue_new_fvector(FVector::CrossProduct(py_ue_fvector_get(self), py_ue_fvector_get(py_vec)));
}

static PyObject *py_ue_fvector_project_on_to(ue_PyFVector *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:cross", &py_obj))
		return nullptr;
	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_TypeError, "argument is not a FVector");
	return py_ue_new_fvector(py_ue_fvector_get(self).ProjectOnTo(py_ue_fvector_get(py_vec)));
}

static PyObject *py_ue_fvector_project_on_to_normal(ue_PyFVector *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:cross", &py_obj))
		return nullptr;
	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_TypeError, "argument is not a FVector");
	return py_ue_new_fvector(py_ue_fvector_get(self).ProjectOnToNormal(py_ue_fvector_get(py_vec)));
}


static PyMethodDef ue_PyFVector_methods[] = {

	{ "length", (PyCFunction)py_ue_fvector_length, METH_VARARGS, "" },
	{ "size", (PyCFunction)py_ue_fvector_length, METH_VARARGS, "" },
	{ "size_squared", (PyCFunction)py_ue_fvector_length_squared, METH_VARARGS, "" },
	{ "length_squared", (PyCFunction)py_ue_fvector_length_squared, METH_VARARGS, "" },
	{ "normalized", (PyCFunction)py_ue_fvector_normalized, METH_VARARGS, "" },
	{ "rotation", (PyCFunction)py_ue_fvector_rotation, METH_VARARGS, "" },
	{ "dot", (PyCFunction)py_ue_fvector_dot, METH_VARARGS, "" },
	{ "cross", (PyCFunction)py_ue_fvector_cross, METH_VARARGS, "" },
	{ "project_on_to", (PyCFunction)py_ue_fvector_project_on_to, METH_VARARGS, "" },
	{ "project_on_to_normal", (PyCFunction)py_ue_fvector_project_on_to_normal, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_fvector_get_x(ue_PyFVector *self, void *closure)
{
	return PyFloat_FromDouble(py_ue_fvector_get(self).X);
}

static int py_ue_fvector_set_x(ue_PyFVector *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		py_ue_fvector_get(self).X = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fvector_get_y(ue_PyFVector *self, void *closure)
{
	return PyFloat_FromDouble(py_ue_fvector_get(self).Y);
}

static int py_ue_fvector_set_y(ue_PyFVector *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		py_ue_fvector_get(self).Y = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fvector_get_z(ue_PyFVector *self, void *closure)
{
	return PyFloat_FromDouble(py_ue_fvector_get(self).Z);
}

static int py_ue_fvector_set_z(ue_PyFVector *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		py_ue_fvector_get(self).Z = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}


static PyGetSetDef ue_PyFVector_getseters[] = {
	{(char *) "x", (getter)py_ue_fvector_get_x, (setter)py_ue_fvector_set_x, (char *)"", NULL },
	{(char *) "y", (getter)py_ue_fvector_get_y, (setter)py_ue_fvector_set_y, (char *)"", NULL },
	{(char *) "z", (getter)py_ue_fvector_get_z, (setter)py_ue_fvector_set_z, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFVector_str(ue_PyFVector *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FVector {'x': %S, 'y': %S, 'z': %S}>",
		PyFloat_FromDouble(py_ue_fvector_get(self).X), PyFloat_FromDouble(py_ue_fvector_get(self).Y), PyFloat_FromDouble(py_ue_fvector_get(self).Z));
}

static PyTypeObject ue_PyFVectorType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FVector", /* tp_name */
	sizeof(ue_PyFVector), /* tp_basicsize */
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
	(reprfunc)ue_PyFVector_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FVector",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFVector_methods,             /* tp_methods */
	0,
	ue_PyFVector_getseters,
};


static PyObject *ue_py_fvector_add(ue_PyFVector *self, PyObject *value)
{
	FVector vec = py_ue_fvector_get(self);
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		vec += py_ue_fvector_get(py_vec);
	}
	else if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		vec.X += f;
		vec.Y += f;
		vec.Z += f;
		Py_DECREF(f_value);
	}
	return py_ue_new_fvector(vec);
}

static PyObject *ue_py_fvector_sub(ue_PyFVector *self, PyObject *value)
{
	FVector vec = py_ue_fvector_get(self);
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		vec -= py_ue_fvector_get(py_vec);
	}
	else if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		vec.X -= f;
		vec.Y -= f;
		vec.Z -= f;
		Py_DECREF(f_value);
	}
	return py_ue_new_fvector(vec);
}

static PyObject *ue_py_fvector_mul(ue_PyFVector *self, PyObject *value)
{
	FVector vec = py_ue_fvector_get(self);
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		vec *= py_ue_fvector_get(py_vec);
	}
	else if (ue_PyFRotator *py_rot = py_ue_is_frotator(value))
	{
		return py_ue_new_fvector(py_ue_frotator_get(py_rot).RotateVector(vec));
	}
	else if (ue_PyFQuat *py_quat = py_ue_is_fquat(value))
	{
		return py_ue_new_fvector(py_ue_fquat_get(py_quat).RotateVector(vec));
	}
	else if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		vec *= f;
		Py_DECREF(f_value);
	}
	return py_ue_new_fvector(vec);
}

static PyObject *ue_py_fvector_div(ue_PyFVector *self, PyObject *value)
{
	FVector vec = py_ue_fvector_get(self);
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		if (py_ue_fvector_get(py_vec).X == 0 || py_ue_fvector_get(py_vec).Y == 0 || py_ue_fvector_get(py_vec).Z == 0)
			return PyErr_Format(PyExc_ZeroDivisionError, "division by zero");
		vec /= py_ue_fvector_get(py_vec);
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
	return py_ue_new_fvector(vec);
}

PyNumberMethods ue_PyFVector_number_methods;

static Py_ssize_t ue_py_fvector_seq_length(ue_PyFVector *self)
{
	return 3;
}

static PyObject *ue_py_fvector_seq_item(ue_PyFVector *self, Py_ssize_t i)
{
	switch (i)
	{
	case 0:
		return PyFloat_FromDouble(py_ue_fvector_get(self).X);
	case 1:
		return PyFloat_FromDouble(py_ue_fvector_get(self).Y);
	case 2:
		return PyFloat_FromDouble(py_ue_fvector_get(self).Z);
	}
	return PyErr_Format(PyExc_IndexError, "FVector has only 3 items");
}

PySequenceMethods ue_PyFVector_sequence_methods;

static int ue_py_fvector_init(ue_PyFVector *self, PyObject *args, PyObject *kwargs)
{
	float x = 0, y = 0, z = 0;
	if (!PyArg_ParseTuple(args, "|fff", &x, &y, &z))
		return -1;

    ue_py_uscriptstruct_alloc(&self->py_base, TBaseStructure<FVector>::Get(), nullptr, false);

	if (PyTuple_Size(args) == 1)
	{
		y = x;
		z = x;
	}

	py_ue_fvector_get(self).X = x;
	py_ue_fvector_get(self).Y = y;
	py_ue_fvector_get(self).Z = z;

	return 0;
}

static PyObject *ue_py_fvector_richcompare(ue_PyFVector *vec1, PyObject *b, int op)
{
	ue_PyFVector *vec2 = py_ue_is_fvector(b);
	if (!vec2 || (op != Py_EQ && op != Py_NE))
	{
		return PyErr_Format(PyExc_NotImplementedError, "can only compare with another FVector");
	}

	if (op == Py_EQ)
	{
		if (py_ue_fvector_get(vec1).X == py_ue_fvector_get(vec2).X &&
			py_ue_fvector_get(vec1).Y == py_ue_fvector_get(vec2).Y &&
			py_ue_fvector_get(vec1).Z == py_ue_fvector_get(vec2).Z)
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
		Py_INCREF(Py_False);
		return Py_False;
	}

	if (py_ue_fvector_get(vec1).X == py_ue_fvector_get(vec2).X &&
		py_ue_fvector_get(vec1).Y == py_ue_fvector_get(vec2).Y &&
		py_ue_fvector_get(vec1).Z == py_ue_fvector_get(vec2).Z)
	{
		Py_INCREF(Py_False);
		return Py_False;
	}
	Py_INCREF(Py_True);
	return Py_True;
}

void ue_python_init_fvector(PyObject *ue_module)
{
	ue_PyFVectorType.tp_new = PyType_GenericNew;

	ue_PyFVectorType.tp_init = (initproc)ue_py_fvector_init;
	ue_PyFVectorType.tp_richcompare = (richcmpfunc)ue_py_fvector_richcompare;

	memset(&ue_PyFVector_number_methods, 0, sizeof(PyNumberMethods));
	ue_PyFVectorType.tp_as_number = &ue_PyFVector_number_methods;
	ue_PyFVector_number_methods.nb_add = (binaryfunc)ue_py_fvector_add;
	ue_PyFVector_number_methods.nb_subtract = (binaryfunc)ue_py_fvector_sub;
	ue_PyFVector_number_methods.nb_multiply = (binaryfunc)ue_py_fvector_mul;
	ue_PyFVector_number_methods.nb_divmod = (binaryfunc)ue_py_fvector_div;

	memset(&ue_PyFVector_sequence_methods, 0, sizeof(PySequenceMethods));
	ue_PyFVectorType.tp_as_sequence = &ue_PyFVector_sequence_methods;
	ue_PyFVector_sequence_methods.sq_length = (lenfunc)ue_py_fvector_seq_length;
	ue_PyFVector_sequence_methods.sq_item = (ssizeargfunc)ue_py_fvector_seq_item;

    ue_PyFVectorType.tp_base = &ue_PyUScriptStructType;

	if (PyType_Ready(&ue_PyFVectorType) < 0)
		return;

	PyDict_SetItemString(ue_PyFVectorType.tp_dict, "forward", py_ue_new_fvector(FVector(1, 0, 0)));
	PyDict_SetItemString(ue_PyFVectorType.tp_dict, "right", py_ue_new_fvector(FVector(0, 1, 0)));
	PyDict_SetItemString(ue_PyFVectorType.tp_dict, "up", py_ue_new_fvector(FVector(0, 0, 1)));

	Py_INCREF(&ue_PyFVectorType);
	PyModule_AddObject(ue_module, "FVector", (PyObject *)&ue_PyFVectorType);
}

PyObject *py_ue_new_fvector(const FVector& vec)
{
	ue_PyFVector *ret = (ue_PyFVector *)PyObject_New(ue_PyFVector, &ue_PyFVectorType);
    ue_py_uscriptstruct_alloc(&ret->py_base, TBaseStructure<FVector>::Get(), (uint8 const*)&vec, false);
	return (PyObject *)ret;
}

PyObject *py_ue_new_fvector_ptr(FVector* vec_ptr)
{
    ue_PyFVector *ret = (ue_PyFVector *)PyObject_New(ue_PyFVector, &ue_PyFVectorType);
    ue_py_uscriptstruct_alloc(&ret->py_base, TBaseStructure<FVector>::Get(), (uint8*)vec_ptr, true);
    return (PyObject *)ret;
}

ue_PyFVector *py_ue_is_fvector(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFVectorType))
		return nullptr;
	return (ue_PyFVector *)obj;
}

bool py_ue_vector_arg(PyObject *args, FVector &vec)
{

	if (PyTuple_Size(args) == 1)
	{
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFVector *py_vec = py_ue_is_fvector(arg);
		if (!py_vec)
		{
			PyErr_Format(PyExc_TypeError, "argument is not a FVector");
			return false;
		}
		vec = py_ue_fvector_get(py_vec);
		return true;
	}

	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff", &x, &y, &z))
		return false;
	vec.X = x;
	vec.Y = y;
	vec.Z = z;
	return true;
}

