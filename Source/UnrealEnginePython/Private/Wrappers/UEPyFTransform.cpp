#include "UEPyFTransform.h"

static PyObject *py_ue_ftransform_inverse(ue_PyFTransform *self, PyObject * args)
{
	return py_ue_new_ftransform(self->transform.Inverse());
}

static PyObject *py_ue_ftransform_normalize_rotation(ue_PyFTransform *self, PyObject * args)
{
	FTransform transform = self->transform;
	transform.NormalizeRotation();
	return py_ue_new_ftransform(transform);
}

static PyObject *py_ue_ftransform_get_relative_transform(ue_PyFTransform *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O", &py_obj))
	{
		return nullptr;
	}

	ue_PyFTransform *py_transform = py_ue_is_ftransform(py_obj);
	if (!py_transform)
		return PyErr_Format(PyExc_Exception, "argument is not a FTransform");
	return py_ue_new_ftransform(self->transform.GetRelativeTransform(py_transform->transform));
}

static PyObject *py_ue_ftransform_transform_vector(ue_PyFTransform *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O", &py_obj))
	{
		return nullptr;
	}

	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");
	return py_ue_new_fvector(self->transform.TransformVector(py_vec->vec));
}

static PyObject *py_ue_ftransform_transform_vector_no_scale(ue_PyFTransform *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O", &py_obj))
	{
		return nullptr;
	}

	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");
	return py_ue_new_fvector(self->transform.TransformVectorNoScale(py_vec->vec));
}

static PyObject *py_ue_ftransform_transform_position(ue_PyFTransform *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O", &py_obj))
	{
		return nullptr;
	}

	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");
	return py_ue_new_fvector(self->transform.TransformPosition(py_vec->vec));
}

static PyObject *py_ue_ftransform_transform_position_no_scale(ue_PyFTransform *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O", &py_obj))
	{
		return nullptr;
	}

	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");
	return py_ue_new_fvector(self->transform.TransformPositionNoScale(py_vec->vec));
}

#if ENGINE_MINOR_VERSION > 17
static PyObject *py_ue_ftransform_transform_rotation(ue_PyFTransform *self, PyObject * args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O", &py_obj))
	{
		return nullptr;
	}

	ue_PyFQuat *py_quat = py_ue_is_fquat(py_obj);
	if (!py_quat)
		return PyErr_Format(PyExc_Exception, "argument is not a FQuat");
	return py_ue_new_fquat(self->transform.TransformRotation(py_quat->quat));
}
#endif

static PyObject *py_ue_ftransform_get_matrix(ue_PyFTransform *self, PyObject * args)
{
	FTransform transform = self->transform;
	transform.NormalizeRotation();
	FMatrix matrix = transform.ToMatrixWithScale();
	UScriptStruct *u_struct = FindObject<UScriptStruct>(ANY_PACKAGE, UTF8_TO_TCHAR("Matrix"));
	if (!u_struct)
	{
		return PyErr_Format(PyExc_Exception, "unable to get Matrix struct");
	}
	return py_ue_new_owned_uscriptstruct(u_struct, (uint8 *)&matrix);
}

static PyMethodDef ue_PyFTransform_methods[] = {
	{ "inverse", (PyCFunction)py_ue_ftransform_inverse, METH_VARARGS, "" },
	{ "get_relative_transform", (PyCFunction)py_ue_ftransform_get_relative_transform, METH_VARARGS, "" },
	{ "normalize_rotation", (PyCFunction)py_ue_ftransform_normalize_rotation, METH_VARARGS, "" },
	{ "get_matrix", (PyCFunction)py_ue_ftransform_get_matrix, METH_VARARGS, "" },
	{ "transform_vector", (PyCFunction)py_ue_ftransform_transform_vector, METH_VARARGS, "" },
	{ "transform_vector_no_scale", (PyCFunction)py_ue_ftransform_transform_vector_no_scale, METH_VARARGS, "" },
	{ "transform_position", (PyCFunction)py_ue_ftransform_transform_position, METH_VARARGS, "" },
	{ "transform_position_no_scale", (PyCFunction)py_ue_ftransform_transform_position_no_scale, METH_VARARGS, "" },
#if ENGINE_MINOR_VERSION > 17
	{ "transform_rotation", (PyCFunction)py_ue_ftransform_transform_rotation, METH_VARARGS, "" },
#endif
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_ftransform_get_translation(ue_PyFTransform *self, void *closure)
{
	return py_ue_new_fvector(self->transform.GetTranslation());
}

static PyObject *py_ue_ftransform_get_scale(ue_PyFTransform *self, void *closure)
{
	return py_ue_new_fvector(self->transform.GetScale3D());
}

static PyObject *py_ue_ftransform_get_rotation(ue_PyFTransform *self, void *closure)
{
	return py_ue_new_frotator(self->transform.GetRotation().Rotator());
}

static PyObject *py_ue_ftransform_get_quaternion(ue_PyFTransform *self, void *closure)
{
	return py_ue_new_fquat(self->transform.GetRotation());
}

static int py_ue_ftransform_set_translation(ue_PyFTransform *self, PyObject *value, void *closure)
{
	if (ue_PyFVector *py_vec = py_ue_is_fvector(value))
	{
		self->transform.SetLocation(py_vec->vec);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a vector");
	return -1;
}

static int py_ue_ftransform_set_rotation(ue_PyFTransform *self, PyObject *value, void *closure)
{
	if (ue_PyFRotator *py_rot = py_ue_is_frotator(value))
	{
		self->transform.SetRotation(py_rot->rot.Quaternion());
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a rotator");
	return -1;
}

static int py_ue_ftransform_set_quaternion(ue_PyFTransform *self, PyObject *value, void *closure)
{
	if (ue_PyFQuat *py_quat = py_ue_is_fquat(value))
	{
		self->transform.SetRotation(py_quat->quat);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a quaternion");
	return -1;
}

static int py_ue_ftransform_set_scale(ue_PyFTransform *self, PyObject *value, void *closure)
{
	if (ue_PyFVector *py_vec = py_ue_is_fvector(value))
	{
		self->transform.SetScale3D(py_vec->vec);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a vector");
	return -1;
}



static PyGetSetDef ue_PyFTransform_getseters[] = {
	{(char *) "translation", (getter)py_ue_ftransform_get_translation, (setter)py_ue_ftransform_set_translation, (char *)"", NULL },
	{(char *) "scale", (getter)py_ue_ftransform_get_scale, (setter)py_ue_ftransform_set_scale, (char *)"", NULL },
	{(char *) "rotation", (getter)py_ue_ftransform_get_rotation, (setter)py_ue_ftransform_set_rotation, (char *)"", NULL },
	{(char *) "quaternion", (getter)py_ue_ftransform_get_quaternion, (setter)py_ue_ftransform_set_quaternion, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFTransform_str(ue_PyFTransform *self)
{
	FVector vec = self->transform.GetTranslation();
	FRotator rot = self->transform.Rotator();
	FVector scale = self->transform.GetScale3D();

	return PyUnicode_FromFormat("<unreal_engine.FTransform {'translation': (%S, %S, %S), 'rotation': (%S, %S, %S), 'scale': (%S, %S, %S)}>",
		PyFloat_FromDouble(vec.X),
		PyFloat_FromDouble(vec.Y),
		PyFloat_FromDouble(vec.Z),
		PyFloat_FromDouble(rot.Roll),
		PyFloat_FromDouble(rot.Pitch),
		PyFloat_FromDouble(rot.Yaw),
		PyFloat_FromDouble(scale.X),
		PyFloat_FromDouble(scale.Y),
		PyFloat_FromDouble(scale.Z)
	);
}


PyTypeObject ue_PyFTransformType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FTransform", /* tp_name */
	sizeof(ue_PyFTransform), /* tp_basicsize */
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
	(reprfunc)ue_PyFTransform_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FTransform",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFTransform_methods,             /* tp_methods */
	0,
	ue_PyFTransform_getseters,
};

static int ue_py_ftransform_init(ue_PyFTransform *self, PyObject *args, PyObject *kwargs)
{
	PyObject *py_translation = nullptr;
	PyObject *py_rotation = nullptr;
	PyObject *py_scale = nullptr;
	if (!PyArg_ParseTuple(args, "|OOO:__init__", &py_translation, &py_rotation, &py_scale))
	{
		return -1;
	}

	if (py_translation)
	{
		if (ue_PyFVector *py_vec = py_ue_is_fvector(py_translation))
		{
			self->transform.SetTranslation(py_vec->vec);
		}
		else
		{
			PyObject *py_iter = PyObject_GetIter(py_translation);
			if (py_iter)
			{
				FMatrix matrix;
				for (int row = 0; row < 4; row++)
				{
					for (int column = 0; column < 4; column++)
					{
						PyObject *py_item = PyIter_Next(py_iter);
						if (!py_item)
						{
							PyErr_SetString(PyExc_Exception, "matrix is not 4x4");
							Py_DECREF(py_iter);
							return -1;
						}

						if (!PyNumber_Check(py_item))
						{
							PyErr_SetString(PyExc_Exception, "matrix can contains only float");
							Py_DECREF(py_iter);
							return -1;
						}

						PyObject *py_num = PyNumber_Float(py_item);
						if (!py_num)
						{
							PyErr_SetString(PyExc_Exception, "matrix can contains only float");
							Py_DECREF(py_iter);
							return -1;
						}
						matrix.M[row][column] = PyFloat_AsDouble(py_num);
						Py_DECREF(py_num);
					}
				}
				self->transform.SetFromMatrix(matrix);
				Py_DECREF(py_iter);
				return 0;
			}
			PyErr_SetString(PyExc_Exception, "argument is not a FVector or a 4x4 float matrix");
			return -1;
		}
	}

	if (py_rotation)
	{
		if (ue_PyFRotator *py_rot = py_ue_is_frotator(py_rotation))
		{
			self->transform.SetRotation(py_rot->rot.Quaternion());
		}
		else if (ue_PyFQuat *py_quat = py_ue_is_fquat(py_rotation))
		{
			self->transform.SetRotation(py_quat->quat);
		}
		else
		{
			PyErr_SetString(PyExc_Exception, "argument is not a FRotator or a FQuat");
			return -1;
		}
	}
	else
	{
		self->transform.SetRotation(FQuat::Identity);
	}

	// ensure scaling is set to 1,1,1
	FVector scale(1, 1, 1);

	if (py_scale)
	{
		if (ue_PyFVector *py_vec = py_ue_is_fvector(py_scale))
		{
			scale = py_vec->vec;
		}
		else
		{
			PyErr_SetString(PyExc_Exception, "argument is not a FVector");
			return -1;
		}
	}
	self->transform.SetScale3D(scale);
	return 0;
}

static PyObject *ue_py_ftransform_mul(ue_PyFTransform *self, PyObject *value)
{
	FTransform t = self->transform;
	if (ue_PyFQuat *py_quat = py_ue_is_fquat(value))
	{
		t *= py_quat->quat;
	}
	else if (ue_PyFRotator *py_rot = py_ue_is_frotator(value))
	{
		t *= py_rot->rot.Quaternion();
	}
	else if (ue_PyFTransform *py_transform = py_ue_is_ftransform(value))
	{
		t *= py_transform->transform;
	}
	else
	{
		return PyErr_Format(PyExc_TypeError, "FTransform can be multiplied only for an FQuat, an FRotator or an FTransform");
	}
	return py_ue_new_ftransform(t);
}

PyNumberMethods ue_PyFTransform_number_methods;

void ue_python_init_ftransform(PyObject *ue_module)
{
	ue_PyFTransformType.tp_new = PyType_GenericNew;

	ue_PyFTransformType.tp_init = (initproc)ue_py_ftransform_init;

	memset(&ue_PyFTransform_number_methods, 0, sizeof(PyNumberMethods));
	ue_PyFTransformType.tp_as_number = &ue_PyFTransform_number_methods;
	ue_PyFTransform_number_methods.nb_multiply = (binaryfunc)ue_py_ftransform_mul;

	if (PyType_Ready(&ue_PyFTransformType) < 0)
		return;

	Py_INCREF(&ue_PyFTransformType);
	PyModule_AddObject(ue_module, "FTransform", (PyObject *)&ue_PyFTransformType);
}

PyObject *py_ue_new_ftransform(FTransform transform)
{
	ue_PyFTransform *ret = (ue_PyFTransform *)PyObject_New(ue_PyFTransform, &ue_PyFTransformType);
	ret->transform = transform;
	return (PyObject *)ret;
}

ue_PyFTransform *py_ue_is_ftransform(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFTransformType))
		return nullptr;
	return (ue_PyFTransform *)obj;
}

bool py_ue_transform_arg(PyObject *args, FTransform &t)
{

	if (PyTuple_Size(args) == 1)
	{
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFTransform *py_t = py_ue_is_ftransform(arg);
		if (!py_t)
		{
			PyErr_Format(PyExc_TypeError, "argument is not a FTransform");
			return false;
		}
		t = py_t->transform;
		return true;
	}

	float x, y, z;
	float roll, pitch, yaw;
	float sx, sy, sz;
	if (!PyArg_ParseTuple(args, "fffffffff", &x, &y, &z, &roll, &pitch, &yaw, &sx, &sy, &sz))
		return false;

	t.SetLocation(FVector(x, y, z));
	t.SetRotation(FRotator(pitch, yaw, roll).Quaternion());
	t.SetScale3D(FVector(sx, sy, sz));
	return true;
}
