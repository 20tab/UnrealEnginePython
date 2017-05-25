#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyTimer.h"

static PyObject *py_ue_ftimerhandle_clear(ue_PyFTimerHandle *self, PyObject * args) {
	self->world->GetTimerManager().ClearTimer(self->thandle);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ftimerhandle_pause(ue_PyFTimerHandle *self, PyObject * args) {
	self->world->GetTimerManager().PauseTimer(self->thandle);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ftimerhandle_unpause(ue_PyFTimerHandle *self, PyObject * args) {
	self->world->GetTimerManager().UnPauseTimer(self->thandle);
	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef ue_PyFTimerHandle_methods[] = {

	// Transform

	{ "clear", (PyCFunction)py_ue_ftimerhandle_clear, METH_VARARGS, "" },
	{ "pause", (PyCFunction)py_ue_ftimerhandle_pause, METH_VARARGS, "" },
	{ "unpause", (PyCFunction)py_ue_ftimerhandle_unpause, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

// destructor
static void ue_pyftimerhandle_dealloc(ue_PyFTimerHandle *self) {
	Py_DECREF(self->py_callable);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject ue_PyFTimerHandleType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FTimerHandle", /* tp_name */
	sizeof(ue_PyFTimerHandle), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_pyftimerhandle_dealloc,       /* tp_dealloc */
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
	"Unreal Engine FTimerHandle",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFTimerHandle_methods,             /* tp_methods */
};

void ue_python_init_ftimerhandle(PyObject *ue_module) {
	ue_PyFTimerHandleType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ue_PyFTimerHandleType) < 0)
		return;

	Py_INCREF(&ue_PyFTimerHandleType);
	PyModule_AddObject(ue_module, "FTimerHandle", (PyObject *)&ue_PyFTimerHandleType);
}

PyObject *py_ue_set_timer(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float rate;
	PyObject *py_callable;
	PyObject *py_loop = nullptr;
	float first_delay = -1;
	if (!PyArg_ParseTuple(args, "fO|Of:set_timer", &rate, &py_callable, &py_loop, &first_delay)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "object is not a callable");

	bool loop = false;
	if (py_loop && PyObject_IsTrue(py_loop))
		loop = true;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	FTimerDelegate timer_delegate;
	UPythonDelegate *py_delegate = NewObject<UPythonDelegate>();
	py_delegate->SetPyCallable(py_callable);
	// fake UFUNCTION for bypassing checks
	timer_delegate.BindUFunction(py_delegate, FName("PyFakeCallable"));

	// allow the delegate to not be destroyed
	py_delegate->AddToRoot();
	self->python_delegates_gc->push_back(py_delegate);
	
	FTimerHandle thandle;
	world->GetTimerManager().SetTimer(thandle, timer_delegate, rate, loop, first_delay);

	ue_PyFTimerHandle *ret = (ue_PyFTimerHandle *)PyObject_New(ue_PyFTimerHandle, &ue_PyFTimerHandleType);
	if (!ret) {
		world->GetTimerManager().ClearTimer(thandle);
		return PyErr_Format(PyExc_Exception, "unable to allocate FTimerHandle python object");
	}
	ret->thandle = thandle;
	ret->py_callable = py_callable;
	// will be decref'ed on clear
	Py_INCREF(ret->py_callable);
	ret->world = world;

	return (PyObject *)ret;
}

