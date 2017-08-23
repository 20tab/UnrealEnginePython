#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyFPointerEvent.h"

static PyObject *py_ue_fpointer_event_get_effecting_button(ue_PyFPointerEvent *self, PyObject * args) {
	FKey key = self->pointer.GetEffectingButton();
	return py_ue_new_uscriptstruct(FKey::StaticStruct(), (uint8*)&key);
}

static PyObject *py_ue_fpointer_event_get_effecting_button_name(ue_PyFPointerEvent *self, PyObject * args) {
	FKey key = self->pointer.GetEffectingButton();
	return PyUnicode_FromString(TCHAR_TO_UTF8(*key.ToString()));
}

static PyObject *py_ue_fpointer_event_get_wheel_delta(ue_PyFPointerEvent *self, PyObject * args) {
	float delta = self->pointer.GetWheelDelta();
	return PyFloat_FromDouble(delta);
}

static PyObject *py_ue_fpointer_event_get_cursor_delta(ue_PyFPointerEvent *self, PyObject * args) {
	FVector2D delta = self->pointer.GetCursorDelta();
	return Py_BuildValue((char *)"(ff)", delta.X, delta.Y);
}

static PyObject *py_ue_fpointer_event_get_screen_space_position(ue_PyFPointerEvent *self, PyObject * args) {
	FVector2D pos = self->pointer.GetScreenSpacePosition();
	return Py_BuildValue((char *)"(ff)", pos.X, pos.Y);
}

static PyMethodDef ue_PyFPointerEvent_methods[] = {
	{ "get_effecting_button", (PyCFunction)py_ue_fpointer_event_get_effecting_button, METH_VARARGS, "" },
	{ "get_effecting_button_name", (PyCFunction)py_ue_fpointer_event_get_effecting_button_name, METH_VARARGS, "" },
	{ "get_wheel_delta", (PyCFunction)py_ue_fpointer_event_get_wheel_delta, METH_VARARGS, "" },
	{ "get_cursor_delta", (PyCFunction)py_ue_fpointer_event_get_cursor_delta, METH_VARARGS, "" },
	{ "get_screen_space_position", (PyCFunction)py_ue_fpointer_event_get_screen_space_position, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFPointerEvent_str(ue_PyFPointerEvent *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FPointerEvent '%s'>",
		TCHAR_TO_UTF8(*self->pointer.ToText().ToString()));
}

static PyTypeObject ue_PyFPointerEventType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FPointerEvent", /* tp_name */
	sizeof(ue_PyFPointerEvent), /* tp_basicsize */
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
	(reprfunc)ue_PyFPointerEvent_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine FPointerEvent",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFPointerEvent_methods,             /* tp_methods */
};

void ue_python_init_fpointer_event(PyObject *ue_module) {

	ue_PyFPointerEventType.tp_base = &ue_PyFInputEventType;

	if (PyType_Ready(&ue_PyFPointerEventType) < 0)
		return;

	Py_INCREF(&ue_PyFPointerEventType);
	PyModule_AddObject(ue_module, "FPointerEvent", (PyObject *)&ue_PyFPointerEventType);
}

PyObject *py_ue_new_fpointer_event(FPointerEvent pointer) {
	ue_PyFPointerEvent *ret = (ue_PyFPointerEvent *)PyObject_New(ue_PyFPointerEvent, &ue_PyFPointerEventType);
	new(&ret->pointer) FPointerEvent(pointer);
	new(&ret->f_input.input) FInputEvent(pointer);
	return (PyObject *)ret;
}
