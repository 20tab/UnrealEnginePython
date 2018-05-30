

#include "UEPyFCharacterEvent.h"

#include "UEPyFModifierKeysState.h"

static PyObject *py_ue_fcharacter_event_get_character(ue_PyFCharacterEvent *self, PyObject * args)
{
	TCHAR c = self->character_event.GetCharacter();
	FString s = FString(1, &c);
	return PyUnicode_FromString(TCHAR_TO_UTF8(*s));
}

static PyMethodDef ue_PyFCharacterEvent_methods[] = {
	{ "get_character", (PyCFunction)py_ue_fcharacter_event_get_character, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFCharacterEvent_str(ue_PyFCharacterEvent *self)
{
	TCHAR c = self->character_event.GetCharacter();
	FString s = FString(1, &c);
	return PyUnicode_FromFormat("<unreal_engine.FCharacterEvent '%s'>",
		TCHAR_TO_UTF8(*s));
}

static PyTypeObject ue_PyFCharacterEventType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FCharacterEvent", /* tp_name */
	sizeof(ue_PyFCharacterEvent), /* tp_basicsize */
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
	(reprfunc)ue_PyFCharacterEvent_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine FCharacterEvent",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFCharacterEvent_methods,             /* tp_methods */
};

static int ue_py_fcharacter_event_init(ue_PyFCharacterEvent *self, PyObject *args, PyObject *kwargs)
{
	char *key;
	PyObject *py_repeat = nullptr;
	PyObject *py_modifier = nullptr;
	if (!PyArg_ParseTuple(args, "sO|O", &key, &py_repeat, &py_modifier))
	{
		return -1;
	}

	FModifierKeysState modifier;
	if (py_modifier)
	{
		ue_PyFModifierKeysState *f_modifier = py_ue_is_fmodifier_keys_state(py_modifier);
		if (!f_modifier)
		{
			PyErr_SetString(PyExc_Exception, "argument is not a FModifierKeysState");
			return -1;
		}
		modifier = f_modifier->modifier;
	}


	FCharacterEvent Event(*UTF8_TO_TCHAR(key), modifier, 0, (py_repeat && PyObject_IsTrue(py_repeat)));

	new(&self->character_event) FCharacterEvent(Event);
	new(&self->f_input.input) FInputEvent(Event);

	return 0;
}

void ue_python_init_fcharacter_event(PyObject *ue_module)
{

	ue_PyFCharacterEventType.tp_base = &ue_PyFInputEventType;
	ue_PyFCharacterEventType.tp_init = (initproc)ue_py_fcharacter_event_init;

	if (PyType_Ready(&ue_PyFCharacterEventType) < 0)
		return;

	Py_INCREF(&ue_PyFCharacterEventType);
	PyModule_AddObject(ue_module, "FCharacterEvent", (PyObject *)&ue_PyFCharacterEventType);
}

PyObject *py_ue_new_fcharacter_event(FCharacterEvent key_event)
{
	ue_PyFCharacterEvent *ret = (ue_PyFCharacterEvent *)PyObject_New(ue_PyFCharacterEvent, &ue_PyFCharacterEventType);
	new(&ret->character_event) FCharacterEvent(key_event);
	new(&ret->f_input.input) FInputEvent(key_event);
	return (PyObject *)ret;
}

ue_PyFCharacterEvent *py_ue_is_fcharacter_event(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFCharacterEventType))
		return nullptr;
	return (ue_PyFCharacterEvent *)obj;
}