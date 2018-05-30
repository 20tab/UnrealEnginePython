#include "UEPyFInputEvent.h"

static PyObject *py_ue_finput_event_to_text(ue_PyFInputEvent *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->input.ToText().ToString()));
}

static PyObject *py_ue_finput_event_get_user_index(ue_PyFInputEvent *self, PyObject * args)
{
	return PyLong_FromUnsignedLong(self->input.GetUserIndex());
}

static PyObject *py_ue_finput_event_is_alt_down(ue_PyFInputEvent *self, PyObject * args)
{
	if (self->input.IsAltDown())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_finput_event_is_command_down(ue_PyFInputEvent *self, PyObject * args)
{
	if (self->input.IsCommandDown())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_finput_event_is_control_down(ue_PyFInputEvent *self, PyObject * args)
{
	if (self->input.IsControlDown())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyMethodDef ue_PyFInputEvent_methods[] = {
	{ "to_text", (PyCFunction)py_ue_finput_event_to_text, METH_VARARGS, "" },
	{ "get_user_index", (PyCFunction)py_ue_finput_event_get_user_index, METH_VARARGS, "" },
	{ "is_alt_down", (PyCFunction)py_ue_finput_event_is_alt_down, METH_VARARGS, "" },
	{ "is_command_down", (PyCFunction)py_ue_finput_event_is_command_down, METH_VARARGS, "" },
	{ "is_control_down", (PyCFunction)py_ue_finput_event_is_control_down, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFInputEvent_str(ue_PyFInputEvent *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FInputEvent '%s'>",
		TCHAR_TO_UTF8(*self->input.ToText().ToString()));
}

PyTypeObject ue_PyFInputEventType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FInputEvent", /* tp_name */
	sizeof(ue_PyFInputEvent), /* tp_basicsize */
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
	(reprfunc)ue_PyFInputEvent_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine FInputEvent",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFInputEvent_methods,             /* tp_methods */
};

void ue_python_init_finput_event(PyObject *ue_module)
{
	ue_PyFInputEventType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFInputEventType) < 0)
		return;

	Py_INCREF(&ue_PyFInputEventType);
	PyModule_AddObject(ue_module, "FInputEvent", (PyObject *)&ue_PyFInputEventType);
}

PyObject *py_ue_new_finput_event(FInputEvent input)
{
	ue_PyFInputEvent *ret = (ue_PyFInputEvent *)PyObject_New(ue_PyFInputEvent, &ue_PyFInputEventType);
	new(&ret->input) FInputEvent(input);
	return (PyObject *)ret;
}
