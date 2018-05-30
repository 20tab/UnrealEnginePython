#include "UEPyFSlowTask.h"

#include "Misc/FeedbackContext.h"

#if WITH_EDITOR

static PyObject *py_ue_fslowtask_initialize(ue_PyFSlowTask *self, PyObject * args)
{
	self->slowtask.Initialize();

	Py_RETURN_NONE;
}

static PyObject *py_ue_fslowtask_destroy(ue_PyFSlowTask *self, PyObject * args)
{
	self->slowtask.Destroy();

	Py_RETURN_NONE;
}

static PyObject *py_ue_fslowtask_make_dialog_delayed(ue_PyFSlowTask *self, PyObject * args)
{
	float threshold;
	PyObject *py_show_cancel_button = nullptr;
	PyObject *py_allow_in_pie = nullptr;
	if(!PyArg_ParseTuple(args, "f|OO:make_dialog_delayed", &threshold, &py_show_cancel_button, &py_allow_in_pie))
	{
		return nullptr;
	}

	bool show_cancel_button = false;
	if (nullptr != py_show_cancel_button && PyObject_IsTrue(py_show_cancel_button))
	{
		show_cancel_button = true;
	}

	bool allow_in_pie = false;
	if (nullptr != py_allow_in_pie && PyObject_IsTrue(py_allow_in_pie))
	{
		allow_in_pie = true;
	}

	self->slowtask.MakeDialogDelayed(threshold, show_cancel_button, allow_in_pie);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fslowtask_make_dialog(ue_PyFSlowTask *self, PyObject * args)
{
	PyObject *py_show_cancel_button = nullptr;
	PyObject *py_allow_in_pie = nullptr;
	if(!PyArg_ParseTuple(args, "|OO:make_dialog", &py_show_cancel_button, &py_allow_in_pie))
	{
		return nullptr;
	}

	bool show_cancel_button = false;
	if (nullptr != py_show_cancel_button && PyObject_IsTrue(py_show_cancel_button))
	{
		show_cancel_button = true;
	}

	bool allow_in_pie = false;
	if (nullptr != py_allow_in_pie && PyObject_IsTrue(py_allow_in_pie))
	{
		allow_in_pie = true;
	}

	self->slowtask.MakeDialog(show_cancel_button, allow_in_pie);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fslowtask_enter_progress_frame(ue_PyFSlowTask *self, PyObject * args)
{
	float expected_work_this_frame = 1.0;
	char *text = (char *)"";
	if(!PyArg_ParseTuple(args, "|fs:enter_progress_frame", &expected_work_this_frame, &text))
	{
		return nullptr;
	}

	self->slowtask.EnterProgressFrame(expected_work_this_frame, FText::FromString(UTF8_TO_TCHAR(text)));

	Py_RETURN_NONE;
}

static PyObject *py_ue_fslowtask_get_current_message(ue_PyFSlowTask *self, PyObject * args)
{
	FText current_message = self->slowtask.GetCurrentMessage();
	return PyUnicode_FromString(TCHAR_TO_UTF8(*current_message.ToString()));
}

static PyObject *py_ue_fslowtask_received_user_cancel(ue_PyFSlowTask *self, PyObject * args )
{
	if(GWarn->ReceivedUserCancel())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}
	Py_INCREF(Py_False);
	return Py_False;
}

static PyMethodDef ue_PyFSlowTask_methods[] = {
	{ "initialize", (PyCFunction)py_ue_fslowtask_initialize, METH_VARARGS, "" },
	{ "destroy", (PyCFunction)py_ue_fslowtask_destroy, METH_VARARGS, "" },
	{ "make_dialog_delayed", (PyCFunction)py_ue_fslowtask_make_dialog_delayed, METH_VARARGS, "" },
	{ "make_dialog", (PyCFunction)py_ue_fslowtask_make_dialog, METH_VARARGS, "" },
	{ "enter_progress_frame", (PyCFunction)py_ue_fslowtask_enter_progress_frame, METH_VARARGS, "" },
	{ "get_current_message", (PyCFunction)py_ue_fslowtask_get_current_message, METH_VARARGS, "" },
	{ "received_user_cancel", (PyCFunction)py_ue_fslowtask_received_user_cancel, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFSlowTaskType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FSlowTask", /* tp_name */
	sizeof(ue_PyFSlowTask), /* tp_basicsize */
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
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FSlowTask",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFSlowTask_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_fslowtask_init(ue_PyFSlowTask *self, PyObject * args)
{
	float amount_of_work;
	char *default_message = (char *)"";
	PyObject *py_enabled = nullptr;
	if(!PyArg_ParseTuple(args, "f|sO:__init__", &amount_of_work, &default_message, &py_enabled))
	{
		return -1;
	}

	bool enabled = true;
	if (nullptr != py_enabled && !PyObject_IsTrue(py_enabled))
	{
		enabled = false;
	}

	new(&self->slowtask) FSlowTask(amount_of_work, FText::FromString(UTF8_TO_TCHAR(default_message)), enabled);
	return 0;
}

void ue_python_init_fslowtask(PyObject *ue_module)
{
	ue_PyFSlowTaskType.tp_new = PyType_GenericNew;
	ue_PyFSlowTaskType.tp_init = (initproc)py_ue_fslowtask_init;

	if (PyType_Ready(&ue_PyFSlowTaskType) < 0)
		return;

	Py_INCREF(&ue_PyFSlowTaskType);
	PyModule_AddObject(ue_module, "FSlowTask", (PyObject *)&ue_PyFSlowTaskType);
}

ue_PyFSlowTask *py_ue_is_fslowtask(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFSlowTaskType))
		return nullptr;
	return (ue_PyFSlowTask *)obj;
}

#endif
