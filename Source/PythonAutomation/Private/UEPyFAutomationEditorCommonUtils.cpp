
#include "UEPyFAutomationEditorCommonUtils.h"

#include "UnrealEnginePython.h"
#include "Engine/World.h"


static PyObject *py_ue_fautomation_editor_common_utils_run_pie(PyObject *cls, PyObject * args)
{
	Py_BEGIN_ALLOW_THREADS;
	FAutomationEditorCommonUtils::RunPIE();
	Py_END_ALLOW_THREADS;
	Py_RETURN_NONE;
}

static PyObject *py_ue_fautomation_editor_common_utils_load_map(PyObject *cls, PyObject * args)
{
	char *map_name;
	if (!PyArg_ParseTuple(args, "s:load_map", &map_name))
		return nullptr;
	Py_BEGIN_ALLOW_THREADS;
	FAutomationEditorCommonUtils::LoadMap(FString(UTF8_TO_TCHAR(map_name)));
	Py_END_ALLOW_THREADS;
	Py_RETURN_NONE;
}

static PyObject *py_ue_fautomation_editor_common_utils_create_new_map(PyObject *cls, PyObject * args)
{
	Py_RETURN_UOBJECT(FAutomationEditorCommonUtils::CreateNewMap());
}

static PyMethodDef ue_PyFAutomationEditorCommonUtils_methods[] = {
	{ "run_pie", (PyCFunction)py_ue_fautomation_editor_common_utils_run_pie, METH_VARARGS | METH_CLASS, "" },
	{ "load_map", (PyCFunction)py_ue_fautomation_editor_common_utils_load_map, METH_VARARGS | METH_CLASS, "" },
	{ "create_new_map", (PyCFunction)py_ue_fautomation_editor_common_utils_create_new_map, METH_VARARGS | METH_CLASS, "" },

	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyFAutomationEditorCommonUtilsType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FAutomationEditorCommonUtils", /* tp_name */
	sizeof(ue_PyFAutomationEditorCommonUtils), /* tp_basicsize */
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
	"Unreal Engine AutomationEditorCommonUtils",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFAutomationEditorCommonUtils_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_fautomation_editor_common_utils_init(ue_PyFAutomationEditorCommonUtils *self, PyObject * args)
{
	PyErr_SetString(PyExc_Exception, "FAutomationEditorCommonUtils is a singleton");
	return -1;
}

void ue_python_init_fautomation_editor_common_utils(PyObject *ue_module)
{
	ue_PyFAutomationEditorCommonUtilsType.tp_new = PyType_GenericNew;
	ue_PyFAutomationEditorCommonUtilsType.tp_init = (initproc)py_ue_fautomation_editor_common_utils_init;

	if (PyType_Ready(&ue_PyFAutomationEditorCommonUtilsType) < 0)
		return;

	Py_INCREF(&ue_PyFAutomationEditorCommonUtilsType);
	PyModule_AddObject(ue_module, "FAutomationEditorCommonUtils", (PyObject *)&ue_PyFAutomationEditorCommonUtilsType);
}
