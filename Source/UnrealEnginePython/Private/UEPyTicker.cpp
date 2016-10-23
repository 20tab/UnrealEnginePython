#include "UnrealEnginePythonPrivatePCH.h"

// destructor
static void ue_pyfdelegatehandle_dealloc(ue_PyFDelegateHandle *self) {
	Py_DECREF(self->py_callable);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject ue_PyFDelegateHandleType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FDelegateHandle", /* tp_name */
	sizeof(ue_PyFDelegateHandle), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_pyfdelegatehandle_dealloc,       /* tp_dealloc */
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
	"Unreal Engine FDelegateHandle",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,						   /* tp_methods */
};

void ue_python_init_fdelegatehandle(PyObject *ue_module) {
	ue_PyFDelegateHandleType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ue_PyFDelegateHandleType) < 0)
		return;

	Py_INCREF(&ue_PyFDelegateHandleType);
	PyModule_AddObject(ue_module, "FDelegateHandle", (PyObject *)&ue_PyFDelegateHandleType);
}

PyObject *py_unreal_engine_add_ticker(PyObject * self, PyObject * args) {

	PyObject *py_callable;
	float delay = 0;
	if (!PyArg_ParseTuple(args, "O|f:add_ticker", &py_callable, &delay)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "object is not a callable");

	FTickerDelegate ticker_delegate;
	UPythonDelegate *py_delegate = NewObject<UPythonDelegate>();
	py_delegate->SetPyCallable(py_callable);

	ticker_delegate.BindUObject(py_delegate, &UPythonDelegate::Tick);

	// avoid the delegate to be GC'ed
	py_delegate->AddToRoot();

	FDelegateHandle dhandle = FTicker::GetCoreTicker().AddTicker(ticker_delegate, delay);
	ue_PyFDelegateHandle *ret = (ue_PyFDelegateHandle *)PyObject_New(ue_PyFDelegateHandle, &ue_PyFDelegateHandleType);
	if (!ret) {
		FTicker::GetCoreTicker().RemoveTicker(dhandle);
		return PyErr_Format(PyExc_Exception, "unable to allocate FDelegateHandle python object");
	}
	ret->dhandle = dhandle;
	ret->py_callable = py_callable;

	return (PyObject *)ret;
}

PyObject *py_unreal_engine_remove_ticker(PyObject * self, PyObject * args) {

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:remove_ticker", &py_obj)) {
		return NULL;
	}

	if (!PyObject_IsInstance(py_obj, (PyObject *)&ue_PyFDelegateHandleType))
		return PyErr_Format(PyExc_Exception, "object is not a PyFDelegateHandle");

	ue_PyFDelegateHandle *py_handle = (ue_PyFDelegateHandle *)py_obj;
	FTicker::GetCoreTicker().RemoveTicker(py_handle->dhandle);

	Py_INCREF(Py_None);
	return Py_None;
}