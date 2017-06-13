
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyIConsoleManager.h"

static PyObject *py_ue_iconsole_manager_add_console_history_entry(PyObject *cls, PyObject * args) {

	char *entry;
	if (!PyArg_ParseTuple(args, "s:add_console_history_entry", &entry)) {
		return nullptr;
	}

	IConsoleManager::Get().AddConsoleHistoryEntry(UTF8_TO_TCHAR(entry));

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_get_console_history(PyObject *cls, PyObject * args) {
	TArray<FString> history;
	IConsoleManager::Get().GetConsoleHistory(history);
	PyObject *py_history = PyList_New(0);
	for (FString item : history) {
		PyObject *py_item = PyUnicode_FromString(TCHAR_TO_UTF8(*item));
		PyList_Append(py_history, py_item);
		Py_DECREF(py_item);
	}
	return py_history;
}

static PyMethodDef ue_PyIConsoleManager_methods[] = {
	{ "get_console_history", (PyCFunction)py_ue_iconsole_manager_get_console_history, METH_VARARGS | METH_CLASS, "" },
	{ "add_console_history_entry", (PyCFunction)py_ue_iconsole_manager_add_console_history_entry, METH_VARARGS | METH_CLASS, "" },
	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyIConsoleManagerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IConsoleManager", /* tp_name */
	sizeof(ue_PyIConsoleManager), /* tp_basicsize */
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
	"Unreal Engine ConsoleManager Interface",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIConsoleManager_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_iconsole_manager_init(ue_PyIConsoleManager *self, PyObject * args) {
	PyErr_SetString(PyExc_Exception, "IConsoleManage is a singleton");
	return -1;
}

void ue_python_init_iconsole_manager(PyObject *ue_module) {
	ue_PyIConsoleManagerType.tp_new = PyType_GenericNew;
	ue_PyIConsoleManagerType.tp_init = (initproc)py_ue_iconsole_manager_init;

	if (PyType_Ready(&ue_PyIConsoleManagerType) < 0)
		return;

	Py_INCREF(&ue_PyIConsoleManagerType);
	PyModule_AddObject(ue_module, "IConsoleManager", (PyObject *)&ue_PyIConsoleManagerType);
}
