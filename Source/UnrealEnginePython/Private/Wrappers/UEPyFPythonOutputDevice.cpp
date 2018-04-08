#include "UEPyFPythonOutputDevice.h"


static PyMethodDef ue_PyFPythonOutputDevice_methods[] = {
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFPythonOutputDevice_str(ue_PyFPythonOutputDevice *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FPythonOutputDevice '%p'>",
		self->device);
}

static void ue_PyFPythonOutputDevice_dealloc(ue_PyFPythonOutputDevice *self)
{
	delete(self->device);
#if PY_MAJOR_VERSION < 3
	self->ob_type->tp_free((PyObject*)self);
#else
	Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

static PyTypeObject ue_PyFPythonOutputDeviceType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FPythonOutputDevice", /* tp_name */
	sizeof(ue_PyFPythonOutputDevice), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_PyFPythonOutputDevice_dealloc,       /* tp_dealloc */
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
	(reprfunc)ue_PyFPythonOutputDevice_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FPythonOutputDevice",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFPythonOutputDevice_methods,             /* tp_methods */
	0,
	0,
};

static int ue_py_fpython_output_device_init(ue_PyFPythonOutputDevice *self, PyObject *args, PyObject *kwargs)
{
	PyObject *py_serialize;
	if (!PyArg_ParseTuple(args, "O", &py_serialize))
	{
		return -1;
	}

	if (!PyCallable_Check(py_serialize))
	{
		PyErr_SetString(PyExc_TypeError, "argument is not a callable");
		return -1;
	}

	self->device = new FPythonOutputDevice();
	self->device->SetPySerialize(py_serialize);
	return 0;
}

void ue_python_init_fpython_output_device(PyObject *ue_module)
{
	ue_PyFPythonOutputDeviceType.tp_new = PyType_GenericNew;

	ue_PyFPythonOutputDeviceType.tp_init = (initproc)ue_py_fpython_output_device_init;

	if (PyType_Ready(&ue_PyFPythonOutputDeviceType) < 0)
		return;

	Py_INCREF(&ue_PyFPythonOutputDeviceType);
	PyModule_AddObject(ue_module, "FPythonOutputDevice", (PyObject *)&ue_PyFPythonOutputDeviceType);
}


