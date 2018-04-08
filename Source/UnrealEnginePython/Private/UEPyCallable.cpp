#include "UEPyCallable.h"

// destructor
static void ue_pycallable_dealloc(ue_PyCallable *self)
{
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyCallable %p mapped to UFunction %p"), self, self->u_function);
#endif
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* ue_pycallable_call(ue_PyCallable *self, PyObject *args, PyObject *kw)
{
	if (!self->u_function ||
		!self->u_target ||
		!self->u_function->IsValidLowLevel() ||
		!self->u_target->IsValidLowLevel() ||
		self->u_function->IsPendingKillOrUnreachable() ||
		self->u_target->IsPendingKillOrUnreachable())
	{
		return PyErr_Format(PyExc_Exception, "UFunction/UObject is in invalid state for python callable");
	}

	return py_ue_ufunction_call(self->u_function, self->u_target, args, 0, kw);
}

static PyTypeObject ue_PyCallableType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.Callable", /* tp_name */
	sizeof(ue_PyCallable), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_pycallable_dealloc,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	(ternaryfunc)ue_pycallable_call,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine Python Callable",           /* tp_doc */
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

void ue_python_init_callable(PyObject *ue_module)
{
	ue_PyCallableType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyCallableType) < 0)
		return;

	Py_INCREF(&ue_PyCallableType);
	PyModule_AddObject(ue_module, "Callable", (PyObject *)&ue_PyCallableType);
}

PyObject *py_ue_new_callable(UFunction *u_function, UObject *u_target)
{
	ue_PyCallable *ret = (ue_PyCallable *)PyObject_New(ue_PyCallable, &ue_PyCallableType);
	ret->u_function = u_function;
	ret->u_target = u_target;
	return (PyObject *)ret;
}

ue_PyCallable *py_ue_is_callable(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyCallableType))
		return nullptr;
	return (ue_PyCallable *)obj;
}
