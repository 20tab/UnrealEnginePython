#include "UEPyFMorphTargetDelta.h"

static PyObject *py_ue_fmorph_target_delta_get_position_delta(ue_PyFMorphTargetDelta *self, void *closure)
{
	return py_ue_new_fvector(self->morph_target_delta.PositionDelta);
}

static int py_ue_fmorph_target_delta_set_position_delta(ue_PyFMorphTargetDelta *self, PyObject *value, void *closure)
{
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		self->morph_target_delta.PositionDelta = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a FVector");
	return -1;
}

static PyObject *py_ue_fmorph_target_delta_get_tangent_z_delta(ue_PyFMorphTargetDelta *self, void *closure)
{
	return py_ue_new_fvector(self->morph_target_delta.TangentZDelta);
}

static int py_ue_fmorph_target_delta_set_tangent_z_delta(ue_PyFMorphTargetDelta *self, PyObject *value, void *closure)
{
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		self->morph_target_delta.TangentZDelta = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a FVector");
	return -1;
}


static int py_ue_fmorph_target_delta_set_source_idx(ue_PyFMorphTargetDelta *self, PyObject *value, void *closure)
{

	if (PyNumber_Check(value))
	{
		PyObject *py_num = PyNumber_Long(value);
		self->morph_target_delta.SourceIdx = PyLong_AsUnsignedLong(py_num);
		Py_DECREF(py_num);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a number");
	return -1;
}

static PyObject *py_ue_fmorph_target_delta_get_source_idx(ue_PyFMorphTargetDelta *self, void *closure)
{
	return PyLong_FromUnsignedLong(self->morph_target_delta.SourceIdx);
}

static PyGetSetDef ue_PyFMorphTargetDelta_getseters[] = {
	{ (char *) "position_delta", (getter)py_ue_fmorph_target_delta_get_position_delta, (setter)py_ue_fmorph_target_delta_set_position_delta, (char *)"", NULL },
	{ (char *) "tangent_z_delta", (getter)py_ue_fmorph_target_delta_get_tangent_z_delta, (setter)py_ue_fmorph_target_delta_set_tangent_z_delta, (char *)"", NULL },
	{ (char *) "source_idx", (getter)py_ue_fmorph_target_delta_get_source_idx, (setter)py_ue_fmorph_target_delta_set_source_idx, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFMorphTargetDelta_str(ue_PyFMorphTargetDelta *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FMorphTargetDelta {'position_delta': {'x': %S, 'y': %S, 'z': %S}, 'source_idx': %d}>",
		PyFloat_FromDouble(self->morph_target_delta.PositionDelta.X),
		PyFloat_FromDouble(self->morph_target_delta.PositionDelta.Y),
		PyFloat_FromDouble(self->morph_target_delta.PositionDelta.Z),
		self->morph_target_delta.SourceIdx);
}

static PyTypeObject ue_PyFMorphTargetDeltaType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FMorphTargetDelta", /* tp_name */
	sizeof(ue_PyFMorphTargetDelta), /* tp_basicsize */
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
	(reprfunc)ue_PyFMorphTargetDelta_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FMorphTargetDelta",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,             /* tp_methods */
	0,
	ue_PyFMorphTargetDelta_getseters,
};

static int py_ue_fmorph_target_delta_init(ue_PyFMorphTargetDelta *self, PyObject * args)
{
	new(&self->morph_target_delta) FMorphTargetDelta();
	return 0;
}

void ue_python_init_fmorph_target_delta(PyObject *ue_module)
{
	ue_PyFMorphTargetDeltaType.tp_new = PyType_GenericNew;
	ue_PyFMorphTargetDeltaType.tp_init = (initproc)py_ue_fmorph_target_delta_init;

	if (PyType_Ready(&ue_PyFMorphTargetDeltaType) < 0)
		return;

	Py_INCREF(&ue_PyFMorphTargetDeltaType);
	PyModule_AddObject(ue_module, "FMorphTargetDelta", (PyObject *)&ue_PyFMorphTargetDeltaType);
}

ue_PyFMorphTargetDelta *py_ue_is_fmorph_target_delta(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFMorphTargetDeltaType))
		return nullptr;
	return (ue_PyFMorphTargetDelta *)obj;
}

PyObject *py_ue_new_fmorph_target_delta(FMorphTargetDelta morph_target_delta)
{
	ue_PyFMorphTargetDelta *ret = (ue_PyFMorphTargetDelta *)PyObject_New(ue_PyFMorphTargetDelta, &ue_PyFMorphTargetDeltaType);
	new(&ret->morph_target_delta) FMorphTargetDelta(morph_target_delta);
	return (PyObject *)ret;
}