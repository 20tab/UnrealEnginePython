
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyFSlateApplication.h"


static PyObject *py_ue_get_average_delta_time(PyObject *cls, PyObject * args)
{
	return PyFloat_FromDouble(FSlateApplication::Get().GetAverageDeltaTime());
}

static PyObject *py_ue_get_delta_time(PyObject *cls, PyObject * args)
{
	return PyFloat_FromDouble(FSlateApplication::Get().GetDeltaTime());
}

static PyObject *py_ue_goto_line_in_source(PyObject *cls, PyObject * args)
{
	char *filename;
	int line_number;
	if (!PyArg_ParseTuple(args, "si:goto_line_in_source", &filename, &line_number))
	{
		return nullptr;
	}

	FSlateApplication::Get().GotoLineInSource(FString(UTF8_TO_TCHAR(filename)), line_number);

	Py_RETURN_NONE;
}

static PyObject *py_ue_is_gamepad_attached(PyObject *cls, PyObject * args)
{
	bool bAttached = FSlateApplication::Get().IsGamepadAttached();
	if (bAttached)
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_is_mouse_attached(PyObject *cls, PyObject * args)
{
	bool bAttached = FSlateApplication::Get().IsMouseAttached();
	if (bAttached)
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_process_key_down_event(PyObject *cls, PyObject * args)
{
	PyObject *py_event;
	if (!PyArg_ParseTuple(args, "O:process_key_down_event", &py_event))
	{
		return nullptr;
	}

	FKeyEvent *InKeyEvent = ue_py_check_struct<FKeyEvent>(py_event);
	if (!InKeyEvent)
	{
		ue_PyFKeyEvent *py_fkey_event = py_ue_is_fkey_event(py_event);
		if (!py_fkey_event)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FKeyEvent");
		}
		InKeyEvent = &py_fkey_event->key_event;
	}

	if (FSlateApplication::Get().ProcessKeyDownEvent(*InKeyEvent))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyObject *py_ue_process_key_char_event(PyObject *cls, PyObject * args)
{
	PyObject *py_event;
	if (!PyArg_ParseTuple(args, "O:process_key_char_event", &py_event))
	{
		return nullptr;
	}

	FCharacterEvent *InCharEvent = ue_py_check_struct<FCharacterEvent>(py_event);
	if (!InCharEvent)
	{
		ue_PyFCharacterEvent *py_fchar_event = py_ue_is_fcharacter_event(py_event);
		if (!py_fchar_event)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FCharacterEvent");
		}
		InCharEvent = &py_fchar_event->character_event;
	}

	if (FSlateApplication::Get().ProcessKeyCharEvent(*InCharEvent))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyMethodDef ue_PyFSlateApplication_methods[] = {
	{ "get_average_delta_time", (PyCFunction)py_ue_get_average_delta_time, METH_VARARGS | METH_CLASS, "" },
	{ "get_delta_time", (PyCFunction)py_ue_get_delta_time, METH_VARARGS | METH_CLASS, "" },
	{ "goto_line_in_source", (PyCFunction)py_ue_goto_line_in_source, METH_VARARGS | METH_CLASS, "" },
	{ "is_gamepad_attached", (PyCFunction)py_ue_is_gamepad_attached, METH_VARARGS | METH_CLASS, "" },
	{ "is_mouse_attached", (PyCFunction)py_ue_is_mouse_attached, METH_VARARGS | METH_CLASS, "" },
	{ "process_key_down_event", (PyCFunction)py_ue_process_key_down_event, METH_VARARGS | METH_CLASS, "" },
	{ "process_key_char_event", (PyCFunction)py_ue_process_key_char_event, METH_VARARGS | METH_CLASS, "" },
	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyFSlateApplicationType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FSlateApplication", /* tp_name */
	sizeof(ue_PyFSlateApplication), /* tp_basicsize */
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
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine SlateApplication",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFSlateApplication_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_fslate_application_init(ue_PyFSlateApplication *self, PyObject * args)
{
	PyErr_SetString(PyExc_Exception, "FSlateApplication is a singleton");
	return -1;
}

void ue_python_init_fslate_application(PyObject *ue_module)
{
	ue_PyFSlateApplicationType.tp_new = PyType_GenericNew;
	ue_PyFSlateApplicationType.tp_init = (initproc)py_ue_fslate_application_init;

	if (PyType_Ready(&ue_PyFSlateApplicationType) < 0)
		return;

	Py_INCREF(&ue_PyFSlateApplicationType);
	PyModule_AddObject(ue_module, "FSlateApplication", (PyObject *)&ue_PyFSlateApplicationType);
}
