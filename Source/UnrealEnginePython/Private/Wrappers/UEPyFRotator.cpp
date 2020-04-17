#include "UEPyFRotator.h"

static PyObject *py_ue_frotator_get_vector(ue_PyFRotator *self, PyObject * args) {
	FVector vec = self->rot.Vector();
	return py_ue_new_fvector(vec);
}

static PyObject *py_ue_frotator_get_euler(ue_PyFRotator *self, PyObject * args) {
	FVector vec = self->rot.Euler();
	return py_ue_new_fvector(vec);
}

static PyObject *py_ue_frotator_inversed(ue_PyFRotator *self, PyObject * args) {
	FRotator rot = self->rot.GetInverse();
	return py_ue_new_frotator(rot);
}

static PyObject *py_ue_frotator_normalized(ue_PyFRotator *self, PyObject * args) {
	FRotator rot = self->rot.GetNormalized();
	return py_ue_new_frotator(rot);
}

static PyObject *py_ue_frotator_quaternion(ue_PyFRotator *self, PyObject * args) {
	FQuat quat = self->rot.Quaternion();
	return py_ue_new_fquat(quat);
}

static PyMethodDef ue_PyFRotator_methods[] = {
	{ "get_vector", (PyCFunction)py_ue_frotator_get_vector, METH_VARARGS, "" },
	{ "get_euler", (PyCFunction)py_ue_frotator_get_euler, METH_VARARGS, "" },
	{ "normalized", (PyCFunction)py_ue_frotator_normalized, METH_VARARGS, "" },
	{ "inversed", (PyCFunction)py_ue_frotator_inversed, METH_VARARGS, "" },
	{ "quaternion", (PyCFunction)py_ue_frotator_quaternion, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_frotator_get_pitch(ue_PyFRotator *self, void *closure) {
	return PyFloat_FromDouble(self->rot.Pitch);
}

static int py_ue_frotator_set_pitch(ue_PyFRotator *self, PyObject *value, void *closure) {
	if (value && PyNumber_Check(value)) {
		PyObject *f_value = PyNumber_Float(value);
		self->rot.Pitch = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_frotator_get_yaw(ue_PyFRotator *self, void *closure) {
	return PyFloat_FromDouble(self->rot.Yaw);
}

static int py_ue_frotator_set_yaw(ue_PyFRotator *self, PyObject *value, void *closure) {
	if (value && PyNumber_Check(value)) {
		PyObject *f_value = PyNumber_Float(value);
		self->rot.Yaw = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyObject *py_ue_frotator_get_roll(ue_PyFRotator *self, void *closure) {
	return PyFloat_FromDouble(self->rot.Roll);
}

static int py_ue_frotator_set_roll(ue_PyFRotator *self, PyObject *value, void *closure) {
	if (value && PyNumber_Check(value)) {
		PyObject *f_value = PyNumber_Float(value);
		self->rot.Roll = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}


static PyGetSetDef ue_PyFRotator_getseters[] = {
	{ (char*)"pitch", (getter)py_ue_frotator_get_pitch, (setter)py_ue_frotator_set_pitch, (char *)"", NULL },
	{ (char *)"yaw", (getter)py_ue_frotator_get_yaw, (setter)py_ue_frotator_set_yaw, (char *)"", NULL },
	{ (char *)"roll", (getter)py_ue_frotator_get_roll, (setter)py_ue_frotator_set_roll, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFRotator_str(ue_PyFRotator *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FRotator {'roll': %S, 'pitch': %S, 'yaw': %S}>",
		PyFloat_FromDouble(self->rot.Roll), PyFloat_FromDouble(self->rot.Pitch), PyFloat_FromDouble(self->rot.Yaw));
}

PyTypeObject ue_PyFRotatorType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FRotator", /* tp_name */
	sizeof(ue_PyFRotator), /* tp_basicsize */
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
	(reprfunc)ue_PyFRotator_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FRotator",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFRotator_methods,             /* tp_methods */
	0,
	ue_PyFRotator_getseters,
};


static PyObject *ue_py_frotator_add(ue_PyFRotator *self, PyObject *value) {
	FRotator rot = self->rot;
	ue_PyFRotator *py_rot = py_ue_is_frotator(value);
	if (py_rot) {
		rot += py_rot->rot;
	}
	else if (PyNumber_Check(value)) {
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		rot.Pitch += f;
		rot.Yaw += f;
		rot.Roll += f;
		Py_DECREF(f_value);
	}
	return py_ue_new_frotator(rot);
}

static PyObject *ue_py_frotator_sub(ue_PyFRotator *self, PyObject *value) {
	FRotator rot = self->rot;
	ue_PyFRotator *py_rot = py_ue_is_frotator(value);
	if (py_rot) {
		rot -= py_rot->rot;
	}
	else if (PyNumber_Check(value)) {
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		rot.Pitch -= f;
		rot.Yaw -= f;
		rot.Roll -= f;
		Py_DECREF(f_value);
	}
	return py_ue_new_frotator(rot);
}

static PyObject *ue_py_frotator_mul(ue_PyFRotator *self, PyObject *value) {
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec) {
		FVector vec = self->rot.RotateVector(py_vec->vec);
		return py_ue_new_fvector(vec);
	}
	else if (PyNumber_Check(value)) {
		FRotator rot = self->rot;
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		rot.Pitch *= f;
		rot.Yaw *= f;
		rot.Roll *= f;
		Py_DECREF(f_value);
		return py_ue_new_frotator(rot);
	}
	return PyErr_Format(PyExc_TypeError, "unsupported argument type");
}

static PyObject *ue_py_frotator_div(ue_PyFRotator *self, PyObject *value) {
	FRotator rot = self->rot;
	
	if (PyNumber_Check(value)) {
		PyObject *f_value = PyNumber_Float(value);
		float f = PyFloat_AsDouble(f_value);
		if (f == 0)
			return PyErr_Format(PyExc_ZeroDivisionError, "division by zero");
		rot.Pitch /= f;
		rot.Yaw /= f;
		rot.Roll /= f;
		Py_DECREF(f_value);
		return py_ue_new_frotator(rot);
	}
	return PyErr_Format(PyExc_TypeError, "unsupported argument type");
	
}

PyNumberMethods ue_PyFRotator_number_methods;

static Py_ssize_t ue_py_frotator_seq_length(ue_PyFRotator *self) {
	return 3;
}

static PyObject *ue_py_frotator_seq_item(ue_PyFRotator *self, Py_ssize_t i) {
	switch (i) {
	case 0:
		return PyFloat_FromDouble(self->rot.Roll);
	case 1:
		return PyFloat_FromDouble(self->rot.Pitch);
	case 2:
		return PyFloat_FromDouble(self->rot.Yaw);
	}
	return PyErr_Format(PyExc_IndexError, "FRotator has only 3 items");
}

PySequenceMethods ue_PyFRotator_sequence_methods;

static int ue_py_frotator_init(ue_PyFRotator *self, PyObject *args, PyObject *kwargs) {
	float pitch = 0, yaw = 0, roll = 0;
    
    if (PyTuple_Size(args) == 1) {
        if (ue_PyFQuat *py_quat = py_ue_is_fquat(PyTuple_GetItem(args, 0))) {
            self->rot = FRotator(py_quat->quat);
            return 0;
        }
    }
    
	if (!PyArg_ParseTuple(args, "|fff", &roll, &pitch, &yaw))
		return -1;

	if (PyTuple_Size(args) == 1) {
		yaw = pitch;
		roll = pitch;
	}

	self->rot.Pitch = pitch;
	self->rot.Yaw = yaw;
	self->rot.Roll = roll;

	return 0;
}

void ue_python_init_frotator(PyObject *ue_module) {
	ue_PyFRotatorType.tp_new = PyType_GenericNew;

	ue_PyFRotatorType.tp_init = (initproc)ue_py_frotator_init;

	memset(&ue_PyFRotator_number_methods, 0, sizeof(PyNumberMethods));
	ue_PyFRotatorType.tp_as_number = &ue_PyFRotator_number_methods;
	ue_PyFRotator_number_methods.nb_add = (binaryfunc)ue_py_frotator_add;
	ue_PyFRotator_number_methods.nb_subtract = (binaryfunc)ue_py_frotator_sub;
	ue_PyFRotator_number_methods.nb_multiply = (binaryfunc)ue_py_frotator_mul;
	ue_PyFRotator_number_methods.nb_divmod = (binaryfunc)ue_py_frotator_div;

	memset(&ue_PyFRotator_sequence_methods, 0, sizeof(PySequenceMethods));
	ue_PyFRotatorType.tp_as_sequence = &ue_PyFRotator_sequence_methods;
	ue_PyFRotator_sequence_methods.sq_length = (lenfunc)ue_py_frotator_seq_length;
	ue_PyFRotator_sequence_methods.sq_item = (ssizeargfunc)ue_py_frotator_seq_item;

	if (PyType_Ready(&ue_PyFRotatorType) < 0)
		return;

	Py_INCREF(&ue_PyFRotatorType);
	PyModule_AddObject(ue_module, "FRotator", (PyObject *)&ue_PyFRotatorType);
}

PyObject *py_ue_new_frotator(FRotator rot) {
	ue_PyFRotator *ret = (ue_PyFRotator *)PyObject_New(ue_PyFRotator, &ue_PyFRotatorType);
	ret->rot = rot;
	return (PyObject *)ret;
}

ue_PyFRotator *py_ue_is_frotator(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFRotatorType))
		return nullptr;
	return (ue_PyFRotator *)obj;
}

bool py_ue_rotator_arg(PyObject *args, FRotator &rot) {

	if (PyTuple_Size(args) == 1) {
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFRotator *py_rot = py_ue_is_frotator(arg);
		if (!py_rot) {
			PyErr_Format(PyExc_TypeError, "argument is not a FRotator");
			return false;
		}
		rot = py_rot->rot;
		return true;
	}

	float pitch, yaw, roll;
	if (!PyArg_ParseTuple(args, "fff", &roll, &pitch, &yaw))
		return false;
	rot.Pitch = pitch;
	rot.Yaw = yaw;
	rot.Roll = roll;
	return true;
}

