#include "UEPyFFoliageInstance.h"

#include "Wrappers/UEPyFVector.h"
#include "Wrappers/UEPyFRotator.h"

static PyObject *ue_PyFFoliageInstance_str(ue_PyFFoliageInstance *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FFoliageInstance %p>",
		&self->instance);
}

static PyObject *py_ue_ffoliage_instance_get_location(ue_PyFFoliageInstance *self, void *closure)
{
	return py_ue_new_fvector(self->instance.Location);
}

static PyObject *py_ue_ffoliage_instance_get_draw_scale3d(ue_PyFFoliageInstance *self, void *closure)
{
	return py_ue_new_fvector(self->instance.DrawScale3D);
}

static PyObject *py_ue_ffoliage_instance_get_flags(ue_PyFFoliageInstance *self, void *closure)
{
	return PyLong_FromUnsignedLong(self->instance.Flags);
}

static PyObject *py_ue_ffoliage_instance_get_pre_align_rotation(ue_PyFFoliageInstance *self, void *closure)
{
	return py_ue_new_frotator(self->instance.PreAlignRotation);
}

static PyObject *py_ue_ffoliage_instance_get_rotation(ue_PyFFoliageInstance *self, void *closure)
{
	return py_ue_new_frotator(self->instance.Rotation);
}

static PyObject *py_ue_ffoliage_instance_get_zoffset(ue_PyFFoliageInstance *self, void *closure)
{
	return PyFloat_FromDouble(self->instance.ZOffset);
}


static PyGetSetDef ue_PyFFoliageInstance_getseters[] = {
	{ (char *)"location", (getter)py_ue_ffoliage_instance_get_location, nullptr, (char *)"", NULL },
	{ (char *)"draw_scale3d", (getter)py_ue_ffoliage_instance_get_draw_scale3d, nullptr, (char *)"", NULL },
	{ (char *)"flags", (getter)py_ue_ffoliage_instance_get_flags, nullptr, (char *)"", NULL },
	{ (char *)"pre_align_rotation", (getter)py_ue_ffoliage_instance_get_pre_align_rotation, nullptr, (char *)"", NULL },
	{ (char *)"rotation", (getter)py_ue_ffoliage_instance_get_rotation, nullptr, (char *)"", NULL },
	{ (char *)"zoffset", (getter)py_ue_ffoliage_instance_get_zoffset, nullptr, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFFoliageInstanceType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FFoliageInstance", /* tp_name */
	sizeof(ue_PyFFoliageInstance), /* tp_basicsize */
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
	(reprfunc)ue_PyFFoliageInstance_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
#endif
	"Unreal Engine FFoliageInstance",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,             /* tp_methods */
	0,
	ue_PyFFoliageInstance_getseters,
};

void ue_python_init_ffoliage_instance(PyObject *ue_module)
{
	ue_PyFFoliageInstanceType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFFoliageInstanceType) < 0)
		return;

	Py_INCREF(&ue_PyFFoliageInstanceType);
	PyModule_AddObject(ue_module, "FoliageInstance", (PyObject *)&ue_PyFFoliageInstanceType);
}

PyObject *py_ue_new_ffoliage_instance(FFoliageInstance instance)
{
	ue_PyFFoliageInstance *ret = (ue_PyFFoliageInstance *)PyObject_New(ue_PyFFoliageInstance, &ue_PyFFoliageInstanceType);
	ret->instance = instance;
	return (PyObject *)ret;
}