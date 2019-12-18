#include "UEPyFKeyEvent.h"

static PyObject *py_ue_fkey_event_get_key(ue_PyFKeyEvent *self, PyObject * args)
{
	FKey key = self->key_event.GetKey();
	return py_ue_new_owned_uscriptstruct(FKey::StaticStruct(), (uint8*)&key);
}

static PyObject *py_ue_fkey_event_get_key_name(ue_PyFKeyEvent *self, PyObject * args)
{
	FKey key = self->key_event.GetKey();
	return PyUnicode_FromString(TCHAR_TO_UTF8(*key.ToString()));
}

static PyMethodDef ue_PyFKeyEvent_methods[] = {
	{ "get_key", (PyCFunction)py_ue_fkey_event_get_key, METH_VARARGS, "" },
	{ "get_key_name", (PyCFunction)py_ue_fkey_event_get_key_name, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFKeyEvent_str(ue_PyFKeyEvent *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FKeyEvent '%s'>",
		TCHAR_TO_UTF8(*self->key_event.ToText().ToString()));
}

static PyTypeObject ue_PyFKeyEventType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FKeyEvent", /* tp_name */
	sizeof(ue_PyFKeyEvent), /* tp_basicsize */
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
	(reprfunc)ue_PyFKeyEvent_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine FKeyEvent",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFKeyEvent_methods,             /* tp_methods */
};

static int ue_py_fkey_event_init(ue_PyFKeyEvent *self, PyObject *args, PyObject *kwargs)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s", &key))
	{
		return -1;
	}

	FKey InKey(key);

	// TODO make it configurable
	FModifierKeysState modifier;

	// TODO configure repeat
	FKeyEvent Event(InKey, modifier, 0, false, 0, 0);

	new(&self->key_event) FKeyEvent(Event);
	new(&self->f_input.input) FInputEvent(Event);

	return 0;
}


void ue_python_init_fkey_event(PyObject *ue_module)
{

	ue_PyFKeyEventType.tp_base = &ue_PyFInputEventType;
	ue_PyFKeyEventType.tp_init = (initproc)ue_py_fkey_event_init;

	if (PyType_Ready(&ue_PyFKeyEventType) < 0)
		return;

	Py_INCREF(&ue_PyFKeyEventType);
	PyModule_AddObject(ue_module, "FKeyEvent", (PyObject *)&ue_PyFKeyEventType);
}

PyObject *py_ue_new_fkey_event(FKeyEvent key_event)
{
	ue_PyFKeyEvent *ret = (ue_PyFKeyEvent *)PyObject_New(ue_PyFKeyEvent, &ue_PyFKeyEventType);
	new(&ret->key_event) FKeyEvent(key_event);
	new(&ret->f_input.input) FInputEvent(key_event);
	return (PyObject *)ret;
}

ue_PyFKeyEvent *py_ue_is_fkey_event(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFKeyEventType))
		return nullptr;
	return (ue_PyFKeyEvent *)obj;
}