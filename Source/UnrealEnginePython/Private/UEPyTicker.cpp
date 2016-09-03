#include "UnrealEnginePythonPrivatePCH.h"

PyObject *py_ue_add_ticker(PyObject * self, PyObject * args) {

	float delay;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "Of:add_ticker", &py_callable, &delay)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "object is not a callable");

	TBaseDelegate<bool, float> lambda = FTickerDelegate::CreateLambda([py_callable](int x){
		PyGILState_STATE gstate;
		gstate = PyGILState_Ensure();

		PyObject *py_args = PyTuple_New(1);
		PyObject *xValue = PyInt_FromLong(x);
		PyTuple_SetItem(py_args, 0, xValue);
		
		PyObject *ret = PyObject_CallObject(py_callable, py_args);
		
		Py_XDECREF(py_args);
		Py_XDECREF(xValue);
		if (!ret) {
			unreal_engine_py_log_error();
			return false;
		}
		
		bool result = false;
		if (ret == Py_True) result = true;
		Py_DECREF(ret);
		PyGILState_Release(gstate);
		return result;
	});

	FDelegateHandle ticker_delegate = FTicker::GetCoreTicker().AddTicker(lambda, delay);
	ue_PyFDelegateHandle *ret = (ue_PyFDelegateHandle *)PyObject_New(ue_PyFDelegateHandle, &ue_PyFDelegateHandleType);
	if (!ret) {
		FTicker::GetCoreTicker().RemoveTicker(ticker_delegate);
		return PyErr_Format(PyExc_Exception, "unable to allocate FDelegateHandle python object");
	}
	ret->dhandle = ticker_delegate;
	ret->py_callable = py_callable;
	// will be decref'ed on clear
	Py_INCREF(ret->py_callable);

	return (PyObject *)ret;
}