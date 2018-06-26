#include "UEPyIAssetEditorInstance.h"

#if WITH_EDITOR



static PyObject *py_ue_iasset_editor_instance_close_window(ue_PyIAssetEditorInstance *self, PyObject * args)
{
	self->editor_instance->CloseWindow();
	Py_RETURN_NONE;
}

static PyObject *py_ue_iasset_editor_instance_focus_window(ue_PyIAssetEditorInstance *self, PyObject * args)
{
	self->editor_instance->FocusWindow();
	Py_RETURN_NONE;
}

static PyObject *py_ue_iasset_editor_instance_get_editor_name(ue_PyIAssetEditorInstance *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->editor_instance->GetEditorName().ToString()));
}

static PyObject *py_ue_iasset_editor_instance_get_last_activation_time(ue_PyIAssetEditorInstance *self, PyObject * args)
{
	return PyFloat_FromDouble(self->editor_instance->GetLastActivationTime());
}

static PyMethodDef ue_PyIAssetEditorInstance_methods[] = {
	{ "close_window", (PyCFunction)py_ue_iasset_editor_instance_close_window, METH_VARARGS, "" },
	{ "focus_window", (PyCFunction)py_ue_iasset_editor_instance_focus_window, METH_VARARGS, "" },
	{ "get_editor_name", (PyCFunction)py_ue_iasset_editor_instance_get_editor_name, METH_VARARGS, "" },
	{ "get_last_activation_time", (PyCFunction)py_ue_iasset_editor_instance_get_last_activation_time, METH_VARARGS, "" },
	{ nullptr }  /* Sentinel */
};

static PyObject *ue_PyIAssetEditorInstance_str(ue_PyIAssetEditorInstance *self)
{
	return PyUnicode_FromFormat("<unreal_engine.IAssetEditorInstance %p>",
		&self->editor_instance);
}

static PyTypeObject ue_PyIAssetEditorInstanceType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IAssetEditorInstance", /* tp_name */
	sizeof(ue_PyIAssetEditorInstance), /* tp_basicsize */
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
	(reprfunc)ue_PyIAssetEditorInstance_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
#endif
	"Unreal Engine IAssetEditorInstance",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIAssetEditorInstance_methods,             /* tp_methods */
	0,
};

void ue_python_init_iasset_editor_instance(PyObject *ue_module)
{
	ue_PyIAssetEditorInstanceType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyIAssetEditorInstanceType) < 0)
		return;

	Py_INCREF(&ue_PyIAssetEditorInstanceType);
	PyModule_AddObject(ue_module, "IAssetEditorInstance", (PyObject *)&ue_PyIAssetEditorInstanceType);
}

ue_PyIAssetEditorInstance *py_ue_is_iasset_editor_instance(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyIAssetEditorInstanceType))
		return nullptr;
	return (ue_PyIAssetEditorInstance *)obj;
}

PyObject *py_ue_new_iasset_editor_instance(IAssetEditorInstance *editor_instance)
{
	ue_PyIAssetEditorInstance *ret = (ue_PyIAssetEditorInstance *)PyObject_New(ue_PyIAssetEditorInstance, &ue_PyIAssetEditorInstanceType);
	ret->editor_instance = editor_instance;
	return (PyObject *)ret;
}
#endif