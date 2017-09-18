#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyFCharacterEvent.h"

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

void ue_python_init_fcharacter_event(PyObject *ue_module)
{

	ue_PyFCharacterEventType.tp_base = &ue_PyFInputEventType;

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
