#include "UEPyFRandomStream.h"

static PyObject *py_ue_frandomstream_frand(ue_PyFRandomStream *self, PyObject * args) {
	return PyFloat_FromDouble(self->rstream.FRand());
}

static PyObject *py_ue_frandomstream_frand_range(ue_PyFRandomStream *self, PyObject * args) {
	float min;
	float max;
	if (!PyArg_ParseTuple(args, "ff:frand_range", &min, &max))
		return NULL;
	return PyFloat_FromDouble(self->rstream.FRandRange(min, max));
}

static PyObject *py_ue_frandomstream_generate_new_seed(ue_PyFRandomStream *self, PyObject * args) {
	self->rstream.GenerateNewSeed();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_frandomstream_get_current_seed(ue_PyFRandomStream *self, PyObject * args) {
	return PyLong_FromLong(self->rstream.GetCurrentSeed());
}

static PyObject *py_ue_frandomstream_get_fraction(ue_PyFRandomStream *self, PyObject * args) {
	return PyFloat_FromDouble(self->rstream.GetFraction());
}

static PyObject *py_ue_frandomstream_get_initial_seed(ue_PyFRandomStream *self, PyObject * args) {
	return PyLong_FromLong(self->rstream.GetInitialSeed());
}

static PyObject *py_ue_frandomstream_get_unit_vector(ue_PyFRandomStream *self, PyObject * args) {
	return py_ue_new_fvector(self->rstream.GetUnitVector());
}

static PyObject *py_ue_frandomstream_get_unsigned_int(ue_PyFRandomStream *self, PyObject * args) {
	return PyLong_FromUnsignedLong(self->rstream.GetUnsignedInt());
}

static PyObject *py_ue_frandomstream_initialize(ue_PyFRandomStream *self, PyObject * args) {
	int seed;
	if (!PyArg_ParseTuple(args, "i:initialize", &seed))
		return NULL;
	self->rstream.Initialize(seed);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_frandomstream_rand_helper(ue_PyFRandomStream *self, PyObject * args) {
	int max;
	if (!PyArg_ParseTuple(args, "i:rand_helper", &max))
		return NULL;
	return PyLong_FromLong(self->rstream.RandHelper(max));
}

static PyObject *py_ue_frandomstream_rand_range(ue_PyFRandomStream *self, PyObject * args) {
	int min;
	int max;
	if (!PyArg_ParseTuple(args, "ii:rand_range", &min, &max))
		return NULL;
	return PyLong_FromLong(self->rstream.RandRange(min, max));
}

static PyObject *py_ue_frandomstream_reset(ue_PyFRandomStream *self, PyObject * args) {
	self->rstream.Reset();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_frandomstream_vrand(ue_PyFRandomStream *self, PyObject * args) {
	return py_ue_new_fvector(self->rstream.VRand());
}

static PyObject *py_ue_frandomstream_vrand_cone(ue_PyFRandomStream *self, PyObject * args) {
	PyObject *py_obj;
	float horizontal = 0;
	float vertical = -1;
	if (!PyArg_ParseTuple(args, "Of|f:vrand_cone", &py_obj, &horizontal, &vertical))
		return NULL;
	ue_PyFVector *py_vec = py_ue_is_fvector(py_obj);
	if (!py_vec)
		return PyErr_Format(PyExc_TypeError, "argument is not a FVector");
	if (vertical < 0)
		vertical = horizontal;
	return py_ue_new_fvector(self->rstream.VRandCone(py_vec->vec, horizontal, vertical));
}


static PyMethodDef ue_PyFRandomStream_methods[] = {

	{ "frand", (PyCFunction)py_ue_frandomstream_frand, METH_VARARGS, "" },
	{ "frand_range", (PyCFunction)py_ue_frandomstream_frand_range, METH_VARARGS, "" },
	{ "generate_new_seed", (PyCFunction)py_ue_frandomstream_generate_new_seed, METH_VARARGS, "" },
	{ "get_current_seed", (PyCFunction)py_ue_frandomstream_get_current_seed, METH_VARARGS, "" },
	{ "get_fraction", (PyCFunction)py_ue_frandomstream_get_fraction, METH_VARARGS, "" },
	{ "get_initial_seed", (PyCFunction)py_ue_frandomstream_get_initial_seed, METH_VARARGS, "" },
	{ "get_unit_vector", (PyCFunction)py_ue_frandomstream_get_unit_vector, METH_VARARGS, "" },
	{ "get_unsigned_int", (PyCFunction)py_ue_frandomstream_get_unsigned_int, METH_VARARGS, "" },
	{ "initialize", (PyCFunction)py_ue_frandomstream_initialize, METH_VARARGS, "" },
	{ "rand_helper", (PyCFunction)py_ue_frandomstream_rand_helper, METH_VARARGS, "" },
	{ "rand_range", (PyCFunction)py_ue_frandomstream_rand_range, METH_VARARGS, "" },
	{ "reset", (PyCFunction)py_ue_frandomstream_reset, METH_VARARGS, "" },
	{ "vrand", (PyCFunction)py_ue_frandomstream_vrand, METH_VARARGS, "" },
	{ "vrand_cone", (PyCFunction)py_ue_frandomstream_vrand_cone, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};



static PyObject *ue_PyFRandomStream_str(ue_PyFRandomStream *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FRandomStream {'seed': %d}>",
		self->rstream.GetCurrentSeed());
}

static PyTypeObject ue_PyFRandomStreamType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FRandomStream", /* tp_name */
	sizeof(ue_PyFRandomStream), /* tp_basicsize */
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
	(reprfunc)ue_PyFRandomStream_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FRandomStream",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFRandomStream_methods,             /* tp_methods */
	0,
	0,
};

static int ue_py_frandomstream_init(ue_PyFRandomStream *self, PyObject *args, PyObject *kwargs) {
	self->rstream.GenerateNewSeed();
	return 0;
}

void ue_python_init_frandomstream(PyObject *ue_module) {
	ue_PyFRandomStreamType.tp_new = PyType_GenericNew;

	ue_PyFRandomStreamType.tp_init = (initproc)ue_py_frandomstream_init;

	if (PyType_Ready(&ue_PyFRandomStreamType) < 0)
		return;

	Py_INCREF(&ue_PyFRandomStreamType);
	PyModule_AddObject(ue_module, "FRandomStream", (PyObject *)&ue_PyFRandomStreamType);
}


