#include "UnrealEnginePythonPrivatePCH.h"


static PyMethodDef ue_PyFTransform_methods[] = {

	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_ftransform_get_translation(ue_PyFTransform *self, void *closure) {
	return py_ue_new_fvector(self->transform.GetTranslation());
}

static PyObject *py_ue_ftransform_get_scale(ue_PyFTransform *self, void *closure) {
	return py_ue_new_fvector(self->transform.GetScale3D());
}

static PyObject *py_ue_ftransform_get_rotation(ue_PyFTransform *self, void *closure) {
	return py_ue_new_frotator(self->transform.GetRotation().Rotator());
}

static int py_ue_ftransform_set_translation(ue_PyFTransform *self, PyObject *value, void *closure) {
	if (ue_PyFVector *py_vec = py_ue_is_fvector(value)) {
		self->transform.SetLocation(py_vec->vec);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a vector");
	return -1;
}

static int py_ue_ftransform_set_rotation(ue_PyFTransform *self, PyObject *value, void *closure) {
	if (ue_PyFRotator *py_rot = py_ue_is_frotator(value)) {
		self->transform.SetRotation(py_rot->rot.Quaternion());
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a rotator");
	return -1;
}

static int py_ue_ftransform_set_scale(ue_PyFTransform *self, PyObject *value, void *closure) {
	if (ue_PyFVector *py_vec = py_ue_is_fvector(value)) {
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
	{ NULL }  /* Sentinel */
};



static PyTypeObject ue_PyFTransformType = {
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
	0,                         /* tp_str */
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

static int ue_py_ftransform_init(ue_PyFTransform *self, PyObject *args, PyObject *kwargs) {
	PyObject *py_translation = nullptr;
	PyObject *py_rotation = nullptr;
	PyObject *py_scale = nullptr;
	if (!PyArg_ParseTuple(args, "|OOO:__init__", &py_translation, &py_rotation, &py_scale)) {
		return NULL;
	}

	if (py_translation) {
		if (ue_PyFVector *py_vec = py_ue_is_fvector(py_translation)) {
			self->transform.SetTranslation(py_vec->vec);
		}
	}

	if (py_rotation) {
		if (ue_PyFRotator *py_rot = py_ue_is_frotator(py_rotation)) {
			self->transform.SetRotation(py_rot->rot.Quaternion());
		}
	}

	// ensure scaling is set to 1,1,1
	FVector scale(1, 1, 1);

	if (py_scale) {
		if (ue_PyFVector *py_vec = py_ue_is_fvector(py_scale)) {
			scale = py_vec->vec;
		}
	}
	self->transform.SetScale3D(scale);
	return 0;
}

void ue_python_init_ftransform(PyObject *ue_module) {
	ue_PyFTransformType.tp_new = PyType_GenericNew;

	ue_PyFTransformType.tp_init = (initproc)ue_py_ftransform_init;

	if (PyType_Ready(&ue_PyFTransformType) < 0)
		return;

	Py_INCREF(&ue_PyFTransformType);
	PyModule_AddObject(ue_module, "FTransform", (PyObject *)&ue_PyFTransformType);
}

PyObject *py_ue_new_ftransform(FTransform transform) {
	ue_PyFTransform *ret = (ue_PyFTransform *)PyObject_New(ue_PyFTransform, &ue_PyFTransformType);
	ret->transform = transform;
	return (PyObject *)ret;
}

ue_PyFTransform *py_ue_is_ftransform(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFTransformType))
		return nullptr;
	return (ue_PyFTransform *)obj;
}
