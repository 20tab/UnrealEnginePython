#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

static PyObject *py_ue_ftab_spawner_entry_set_display_name(ue_PyFTabSpawnerEntry *self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:set_display_name", &name))
		return NULL;

	self->spawner_entry->SetDisplayName(FText::FromString(UTF8_TO_TCHAR(name)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyMethodDef ue_PyFTabSpawnerEntry_methods[] = {
	{ "set_display_name", (PyCFunction)py_ue_ftab_spawner_entry_set_display_name, METH_VARARGS, "" },
	//{ "set_group", (PyCFunction)py_ue_ftab_spawner_entry_set_group, METH_VARARGS, "" },
	//{ "set_tooltip_text", (PyCFunction)py_ue_ftab_spawner_entry_set_tooltip_text, METH_VARARGS, "" },
	//{ "set_icon", (PyCFunction)py_ue_ftab_spawner_entry_set_icon, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyFTabSpawnerEntry_str(ue_PyFTabSpawnerEntry *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FTabSpawnerEntry {'name': %s}>",
		PyUnicode_FromString(TCHAR_TO_UTF8(*self->spawner_entry->GetDisplayName().ToString())));
}

static PyTypeObject ue_PyFTabSpawnerEntryType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FTabSpawnerEntry", /* tp_name */
	sizeof(ue_PyFTabSpawnerEntry), /* tp_basicsize */
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
	(reprfunc)ue_PyFTabSpawnerEntry_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FTabSpawnerEntry",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFTabSpawnerEntry_methods,             /* tp_methods */
	0,
	0,
};




static int ue_py_ftab_spawner_entry_init(ue_PyFTabSpawnerEntry *self, PyObject *args, PyObject *kwargs) {
	char *name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO", &name, &py_callable))
		return -1;

	if (!PyCallable_Check(py_callable)) {
		PyErr_SetString(PyExc_ValueError, "argument is not callable");
		return -1;
	}

	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	self->spawner_entry = &FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UTF8_TO_TCHAR(name), FOnSpawnTab::CreateUObject(py_delegate, &UPythonSlateDelegate::SpawnPythonTab));

	return 0;
}

void ue_python_init_ftab_spawner_entry(PyObject *ue_module) {
	ue_PyFTabSpawnerEntryType.tp_new = PyType_GenericNew;

	ue_PyFTabSpawnerEntryType.tp_init = (initproc)ue_py_ftab_spawner_entry_init;

	if (PyType_Ready(&ue_PyFTabSpawnerEntryType) < 0)
		return;

	Py_INCREF(&ue_PyFTabSpawnerEntryType);
	PyModule_AddObject(ue_module, "FTabSpawnerEntry", (PyObject *)&ue_PyFTabSpawnerEntryType);
}

PyObject *py_ue_new_ftab_spawner_entry(FTabSpawnerEntry *spawner_entry) {
	ue_PyFTabSpawnerEntry *ret = (ue_PyFTabSpawnerEntry *)PyObject_New(ue_PyFTabSpawnerEntry, &ue_PyFTabSpawnerEntryType);
	ret->spawner_entry = spawner_entry;
	return (PyObject *)ret;
}

#endif