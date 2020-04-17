#include "UEPyFQuat.h"

#if ENGINE_MINOR_VERSION > 12
static PyObject *py_ue_fquat_angular_distance(ue_PyFQuat *self, PyObject * args)
{
	FQuat q;
	if (!py_ue_quat_arg(args, q))
	{
		return nullptr;
	}

	return PyFloat_FromDouble(self->quat.AngularDistance(q));
}
#endif

static PyObject *py_ue_fquat_euler(ue_PyFQuat *self, PyObject * args)
{
	return py_ue_new_fvector(self->quat.Euler());
}

static PyObject *py_ue_fquat_get_axis_x(ue_PyFQuat *self, PyObject * args)
{
	return py_ue_new_fvector(self->quat.GetAxisX());
}

static PyObject *py_ue_fquat_get_axis_y(ue_PyFQuat *self, PyObject * args)
{
	return py_ue_new_fvector(self->quat.GetAxisY());
}

static PyObject *py_ue_fquat_get_axis_z(ue_PyFQuat *self, PyObject * args)
{
	return py_ue_new_fvector(self->quat.GetAxisZ());
}

static PyObject *py_ue_fquat_inverse(ue_PyFQuat *self, PyObject * args)
{
	return py_ue_new_fquat(self->quat.Inverse());
}

static PyObject *py_ue_fquat_get_normalized(ue_PyFQuat *self, PyObject * args)
{
	return py_ue_new_fquat(self->quat.GetNormalized());
}

static PyObject *py_ue_fquat_vector(ue_PyFQuat *self, PyObject * args)
{
	return py_ue_new_fvector(self->quat.Vector());
}

static PyMethodDef ue_PyFQuat_methods[] = {
#if ENGINE_MINOR_VERSION > 12
	{ "angular_distance", (PyCFunction)py_ue_fquat_angular_distance, METH_VARARGS, "" },
#endif
	{ "euler", (PyCFunction)py_ue_fquat_euler, METH_VARARGS, "" },
	{ "rotator", (PyCFunction)py_ue_fquat_euler, METH_VARARGS, "" },
	{ "get_axis_x", (PyCFunction)py_ue_fquat_get_axis_x, METH_VARARGS, "" },
	{ "get_axis_y", (PyCFunction)py_ue_fquat_get_axis_y, METH_VARARGS, "" },
	{ "get_axis_z", (PyCFunction)py_ue_fquat_get_axis_z, METH_VARARGS, "" },
	{ "get_forward_vector", (PyCFunction)py_ue_fquat_get_axis_x, METH_VARARGS, "" },
	{ "get_right_vector", (PyCFunction)py_ue_fquat_get_axis_y, METH_VARARGS, "" },
	{ "get_up_vector", (PyCFunction)py_ue_fquat_get_axis_z, METH_VARARGS, "" },
	{ "inverse", (PyCFunction)py_ue_fquat_inverse, METH_VARARGS, "" },
	{ "vector", (PyCFunction)py_ue_fquat_vector, METH_VARARGS, "" },
	{ "get_normalized", (PyCFunction)py_ue_fquat_get_normalized, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_fquat_get_x(ue_PyFQuat *self, void *closure)
{
	return PyFloat_FromDouble(self->quat.X);
}

static int py_ue_fquat_set_x(ue_PyFQuat *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		self->quat.X = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fquat_get_y(ue_PyFQuat *self, void *closure)
{
	return PyFloat_FromDouble(self->quat.Y);
}

static int py_ue_fquat_set_y(ue_PyFQuat *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		self->quat.Y = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fquat_get_z(ue_PyFQuat *self, void *closure)
{
	return PyFloat_FromDouble(self->quat.Z);
}

static int py_ue_fquat_set_z(ue_PyFQuat *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		self->quat.Z = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_fquat_get_w(ue_PyFQuat *self, void *closure)
{
	return PyFloat_FromDouble(self->quat.W);
}

static int py_ue_fquat_set_w(ue_PyFQuat *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		self->quat.W = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyGetSetDef ue_PyFQuat_getseters[] = {
	{ (char*)"x", (getter)py_ue_fquat_get_x, (setter)py_ue_fquat_set_x, (char *)"", NULL },
	{ (char *)"y", (getter)py_ue_fquat_get_y, (setter)py_ue_fquat_set_y, (char *)"", NULL },
	{ (char *)"z", (getter)py_ue_fquat_get_z, (setter)py_ue_fquat_set_z, (char *)"", NULL },
	{ (char *)"w", (getter)py_ue_fquat_get_w, (setter)py_ue_fquat_set_w, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFQuat_str(ue_PyFQuat *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FQuat {'x': %S, 'y': %S, 'z': %S, 'w': %S}>",
		PyFloat_FromDouble(self->quat.X), PyFloat_FromDouble(self->quat.Y), PyFloat_FromDouble(self->quat.Z), PyFloat_FromDouble(self->quat.W));
}

PyTypeObject ue_PyFQuatType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FQuat", /* tp_name */
	sizeof(ue_PyFQuat), /* tp_basicsize */
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
	(reprfunc)ue_PyFQuat_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FQuat",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFQuat_methods,             /* tp_methods */
	0,
	ue_PyFQuat_getseters,
};


static PyObject *ue_py_fquat_add(ue_PyFQuat *self, PyObject *value)
{
	FQuat quat = self->quat;
	ue_PyFQuat *py_quat = py_ue_is_fquat(value);
	if (py_quat)
	{
		quat += py_quat->quat;
	}
	return py_ue_new_fquat(quat);
}

static PyObject *ue_py_fquat_sub(ue_PyFQuat *self, PyObject *value)
{
	FQuat quat = self->quat;
	ue_PyFQuat *py_quat = py_ue_is_fquat(value);
	if (py_quat)
	{
		quat -= py_quat->quat;
	}
	return py_ue_new_fquat(quat);
}

static PyObject *ue_py_fquat_mul(ue_PyFQuat *self, PyObject *value)
{
	if (ue_PyFVector *py_vec = py_ue_is_fvector(value))
	{
		FVector vec = self->quat * py_vec->vec;
		return py_ue_new_fvector(vec);
	}
	if (ue_PyFQuat *py_quat = py_ue_is_fquat(value))
	{
		FQuat quat = self->quat * py_quat->quat;
		return py_ue_new_fquat(quat);
	}
	else if (PyNumber_Check(value))
	{
		FQuat quat = self->quat;
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		quat *= f;
		Py_DECREF(f_value);
		return py_ue_new_fquat(quat);
	}
	return PyErr_Format(PyExc_TypeError, "unsupported argument type");
}

static PyObject *ue_py_fquat_div(ue_PyFQuat *self, PyObject *value)
{
	FQuat quat = self->quat;

	if (PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		if (f == 0)
			return PyErr_Format(PyExc_ZeroDivisionError, "division by zero");
		quat /= f;
		Py_DECREF(f_value);
		return py_ue_new_fquat(quat);
	}
	return PyErr_Format(PyExc_TypeError, "unsupported argument type");

}

PyNumberMethods ue_PyFQuat_number_methods;

static Py_ssize_t ue_py_fquat_seq_length(ue_PyFQuat *self)
{
	return 4;
}

static PyObject *ue_py_fquat_seq_item(ue_PyFQuat *self, Py_ssize_t i)
{
	switch (i)
	{
	case 0:
		return PyFloat_FromDouble(self->quat.X);
	case 1:
		return PyFloat_FromDouble(self->quat.Y);
	case 2:
		return PyFloat_FromDouble(self->quat.Z);
	case 3:
		return PyFloat_FromDouble(self->quat.W);
	}
	return PyErr_Format(PyExc_IndexError, "FQuat has only 4 items");
}

PySequenceMethods ue_PyFQuat_sequence_methods;

static int ue_py_fquat_init(ue_PyFQuat *self, PyObject *args, PyObject *kwargs)
{
	float x = 0, y = 0, z = 0, w = 1;
	if (!PyArg_ParseTuple(args, "|ffff", &x, &y, &z, &w))
		return -1;

	self->quat.X = x;
	self->quat.Y = y;
	self->quat.Z = z;
	self->quat.W = w;

	return 0;
}

void ue_python_init_fquat(PyObject *ue_module)
{
	ue_PyFQuatType.tp_new = PyType_GenericNew;

	ue_PyFQuatType.tp_init = (initproc)ue_py_fquat_init;

	memset(&ue_PyFQuat_number_methods, 0, sizeof(PyNumberMethods));
	ue_PyFQuatType.tp_as_number = &ue_PyFQuat_number_methods;
	ue_PyFQuat_number_methods.nb_add = (binaryfunc)ue_py_fquat_add;
	ue_PyFQuat_number_methods.nb_subtract = (binaryfunc)ue_py_fquat_sub;
	ue_PyFQuat_number_methods.nb_multiply = (binaryfunc)ue_py_fquat_mul;
	ue_PyFQuat_number_methods.nb_divmod = (binaryfunc)ue_py_fquat_div;

	memset(&ue_PyFQuat_sequence_methods, 0, sizeof(PySequenceMethods));
	ue_PyFQuatType.tp_as_sequence = &ue_PyFQuat_sequence_methods;
	ue_PyFQuat_sequence_methods.sq_length = (lenfunc)ue_py_fquat_seq_length;
	ue_PyFQuat_sequence_methods.sq_item = (ssizeargfunc)ue_py_fquat_seq_item;

	if (PyType_Ready(&ue_PyFQuatType) < 0)
		return;

	Py_INCREF(&ue_PyFQuatType);
	PyModule_AddObject(ue_module, "FQuat", (PyObject *)&ue_PyFQuatType);
}

PyObject *py_ue_new_fquat(FQuat quat)
{
	ue_PyFQuat *ret = (ue_PyFQuat *)PyObject_New(ue_PyFQuat, &ue_PyFQuatType);
	ret->quat = quat;
	return (PyObject *)ret;
}

ue_PyFQuat *py_ue_is_fquat(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFQuatType))
		return nullptr;
	return (ue_PyFQuat *)obj;
}

bool py_ue_quat_arg(PyObject *args, FQuat &quat)
{

	if (PyTuple_Size(args) == 1)
	{
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFQuat *py_quat = py_ue_is_fquat(arg);
		if (!py_quat)
		{
			PyErr_Format(PyExc_TypeError, "argument is not a FQuat");
			return false;
		}
		quat = py_quat->quat;
		return true;
	}

	float x, y, z, w;
	if (!PyArg_ParseTuple(args, "ffff", &x, &y, &z, &w))
		return false;
	quat.X = x;
	quat.Y = y;
	quat.Z = z;
	quat.W = w;
	return true;
}

