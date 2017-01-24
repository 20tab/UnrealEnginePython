
#if WITH_EDITOR


#include "UnrealEnginePythonPrivatePCH.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"

static PyObject *py_ue_iplugin_get_name(ue_PyIPlugin *self, PyObject * args) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetName())));
}


static PyMethodDef ue_PyIPlugin_methods[] = {
	{ "get_name", (PyCFunction)py_ue_iplugin_get_name, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_iplugin_get_category(ue_PyIPlugin *self, void *closure) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().Category)));
}

static PyGetSetDef ue_PyIPlugin_getseters[] = {
	{ (char*)"category", (getter)py_ue_iplugin_get_category, NULL, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyIPlugin_str(ue_PyIPlugin *self)
{
	return PyUnicode_FromFormat("<unreal_engine.EdGraphPin {'name': '%s'}>",
		TCHAR_TO_UTF8(*self->plugin->GetName()));
}

static PyTypeObject ue_PyIPluginType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IPlugin", /* tp_name */
	sizeof(ue_PyIPlugin), /* tp_basicsize */
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
	(reprfunc)ue_PyIPlugin_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine Editor IPlugin",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIPlugin_methods,             /* tp_methods */
	0,
	ue_PyIPlugin_getseters,
};


void ue_python_init_iplugin(PyObject *ue_module) {
	ue_PyIPluginType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyIPluginType) < 0)
		return;

	Py_INCREF(&ue_PyIPluginType);
	PyModule_AddObject(ue_module, "IPlugin", (PyObject *)&ue_PyIPluginType);
}

PyObject *py_ue_new_iplugin(IPlugin *plugin) {
	ue_PyIPlugin *ret = (ue_PyIPlugin *)PyObject_New(ue_PyIPlugin, &ue_PyIPluginType);
	ret->plugin = plugin;
	return (PyObject *)ret;
}
#endif