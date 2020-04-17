#include "UEPyFFrameNumber.h"

#if ENGINE_MINOR_VERSION >= 20

static PyObject *ue_PyFFrameNumber_str(ue_PyFFrameNumber *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FFrameNumber %d>",
		self->frame_number.Value);
}

static PyTypeObject ue_PyFFrameNumberType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FFrameNumber", /* tp_name */
	sizeof(ue_PyFFrameNumber), /* tp_basicsize */
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
	(reprfunc)ue_PyFFrameNumber_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FFrameNumber",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,             /* tp_methods */
	0,
	0,
};

static int py_ue_fframe_number_init(ue_PyFFrameNumber *self, PyObject * args)
{
	int value = 0;
	if (!PyArg_ParseTuple(args, "i", &value))
		return -1;
	new(&self->frame_number) FFrameNumber(value);
	return 0;
}

void ue_python_init_fframe_number(PyObject *ue_module)
{
	ue_PyFFrameNumberType.tp_new = PyType_GenericNew;
	ue_PyFFrameNumberType.tp_init = (initproc)py_ue_fframe_number_init;

	if (PyType_Ready(&ue_PyFFrameNumberType) < 0)
		return;

	Py_INCREF(&ue_PyFFrameNumberType);
	PyModule_AddObject(ue_module, "FFrameNumber", (PyObject *)&ue_PyFFrameNumberType);
}

ue_PyFFrameNumber *py_ue_is_fframe_number(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFFrameNumberType))
		return nullptr;
	return (ue_PyFFrameNumber *)obj;
}

PyObject *py_ue_new_fframe_number(FFrameNumber frame_number)
{
	ue_PyFFrameNumber *ret = (ue_PyFFrameNumber *)PyObject_New(ue_PyFFrameNumber, &ue_PyFFrameNumberType);
	new(&ret->frame_number) FFrameNumber(frame_number);
	return (PyObject *)ret;
}

#endif
