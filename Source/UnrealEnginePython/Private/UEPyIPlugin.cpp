#include "UEPyIPlugin.h"

#if WITH_EDITOR


#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"

static PyObject *py_ue_iplugin_get_name(ue_PyIPlugin *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetName())));
}

static PyObject *py_ue_iplugin_get_base_dir(ue_PyIPlugin *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetBaseDir())));
}

static PyObject *py_ue_iplugin_get_content_dir(ue_PyIPlugin *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetContentDir())));
}

static PyObject *py_ue_iplugin_get_descriptor_file_name(ue_PyIPlugin *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptorFileName())));
}

static PyObject *py_ue_iplugin_get_mounted_asset_path(ue_PyIPlugin *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetMountedAssetPath())));
}

static PyObject *py_ue_iplugin_can_contain_content(ue_PyIPlugin *self, PyObject * args)
{
	if (self->plugin->CanContainContent())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_iplugin_is_enabled(ue_PyIPlugin *self, PyObject * args)
{
	if (self->plugin->IsEnabled())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_iplugin_to_json(ue_PyIPlugin *self, PyObject * args)
{


	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:to_json", &py_bool))
	{
		return NULL;
	}

	bool enabled_by_default = false;
	if (PyObject_IsTrue(py_bool))
	{
		enabled_by_default = true;
	}

	FPluginDescriptor descriptor = self->plugin->GetDescriptor();
	FString text;
#if ENGINE_MINOR_VERSION < 14
	text = descriptor.ToString();
#elif ENGINE_MINOR_VERSION <= 17
	descriptor.Write(text, enabled_by_default);
#else
	descriptor.Write(text);
#endif

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text));
}

static PyObject *py_ue_iplugin_from_json(ue_PyIPlugin *self, PyObject * args)
{

	char *json;
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "sO:from_json", &json, &py_bool))
	{
		return NULL;
	}

	bool enabled_by_default = false;
	if (PyObject_IsTrue(py_bool))
	{
		enabled_by_default = true;
	}

	FText error;
	FString text = FString(UTF8_TO_TCHAR(json));
	FPluginDescriptor descriptor = self->plugin->GetDescriptor();
#if ENGINE_MINOR_VERSION < 14
	if (!descriptor.Read(text, error))
#elif ENGINE_MINOR_VERSION <= 17
	if (!descriptor.Read(text, enabled_by_default, error))
#else
	if (!descriptor.Read(text, error))
#endif
	{
		return PyErr_Format(PyExc_Exception, "unable to update descriptor from json");
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef ue_PyIPlugin_methods[] = {
	{ "get_name", (PyCFunction)py_ue_iplugin_get_name, METH_VARARGS, "" },
	{ "get_base_dir", (PyCFunction)py_ue_iplugin_get_base_dir, METH_VARARGS, "" },
	{ "get_content_dir", (PyCFunction)py_ue_iplugin_get_content_dir, METH_VARARGS, "" },
	{ "get_descriptor_file_name", (PyCFunction)py_ue_iplugin_get_descriptor_file_name, METH_VARARGS, "" },
	{ "get_mounted_asset_path", (PyCFunction)py_ue_iplugin_get_mounted_asset_path, METH_VARARGS, "" },
	{ "can_contain_content", (PyCFunction)py_ue_iplugin_can_contain_content, METH_VARARGS, "" },
	{ "is_enabled", (PyCFunction)py_ue_iplugin_is_enabled, METH_VARARGS, "" },
	{ "to_json", (PyCFunction)py_ue_iplugin_to_json, METH_VARARGS, "" },
	{ "from_json", (PyCFunction)py_ue_iplugin_from_json, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_iplugin_get_category(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().Category)));
}

static PyObject *py_ue_iplugin_get_can_contain_content(ue_PyIPlugin *self, void *closure)
{
	if (self->plugin->GetDescriptor().bCanContainContent)
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_iplugin_get_enabled_by_default(ue_PyIPlugin *self, void *closure)
{
#if ENGINE_MINOR_VERSION < 18
	if (self->plugin->GetDescriptor().bEnabledByDefault)
#else
	if (self->plugin->GetDescriptor().EnabledByDefault == EPluginEnabledByDefault::Enabled)
#endif
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_iplugin_get_installed(ue_PyIPlugin *self, void *closure)
{
	if (self->plugin->GetDescriptor().bInstalled)
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_iplugin_get_is_beta_version(ue_PyIPlugin *self, void *closure)
{
	if (self->plugin->GetDescriptor().bIsBetaVersion)
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_iplugin_created_by(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().CreatedBy)));
}

static PyObject *py_ue_iplugin_created_by_url(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().CreatedByURL)));
}

static PyObject *py_ue_iplugin_description(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().Description)));
}

static PyObject *py_ue_iplugin_docs_url(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().DocsURL)));
}

static PyObject *py_ue_iplugin_friendly_name(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().FriendlyName)));
}

static PyObject *py_ue_iplugin_marketplace_url(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().MarketplaceURL)));
}

static PyObject *py_ue_iplugin_support_url(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().SupportURL)));
}

static PyObject *py_ue_iplugin_version_name(ue_PyIPlugin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->plugin->GetDescriptor().VersionName)));
}

static PyObject *py_ue_iplugin_file_version(ue_PyIPlugin *self, void *closure)
{
#if ENGINE_MINOR_VERSION < 18
	return PyLong_FromLong(self->plugin->GetDescriptor().FileVersion);
#else
	return PyLong_FromLong(self->plugin->GetDescriptor().Version);
#endif
}

static PyObject *py_ue_iplugin_version(ue_PyIPlugin *self, void *closure)
{
	return PyLong_FromLong(self->plugin->GetDescriptor().Version);
}

static PyObject *py_ue_iplugin_get_requires_build_platform(ue_PyIPlugin *self, void *closure)
{
	if (self->plugin->GetDescriptor().bRequiresBuildPlatform)
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyGetSetDef ue_PyIPlugin_getseters[] = {
	{ (char*)"category", (getter)py_ue_iplugin_get_category, NULL, (char *)"", NULL },
	{ (char*)"can_contain_content", (getter)py_ue_iplugin_get_can_contain_content, NULL, (char *)"", NULL },
	{ (char*)"enabled_by_default", (getter)py_ue_iplugin_get_enabled_by_default, NULL, (char *)"", NULL },
	{ (char*)"installed", (getter)py_ue_iplugin_get_installed, NULL, (char *)"", NULL },
	{ (char*)"is_beta_version", (getter)py_ue_iplugin_get_is_beta_version, NULL, (char *)"", NULL },
	{ (char*)"requires_build_platform", (getter)py_ue_iplugin_get_requires_build_platform, NULL, (char *)"", NULL },
	{ (char*)"created_by", (getter)py_ue_iplugin_created_by, NULL, (char *)"", NULL },
	{ (char*)"created_by_url", (getter)py_ue_iplugin_created_by_url, NULL, (char *)"", NULL },
	{ (char*)"description", (getter)py_ue_iplugin_description, NULL, (char *)"", NULL },
	{ (char*)"docs_url", (getter)py_ue_iplugin_docs_url, NULL, (char *)"", NULL },
	{ (char*)"file_version", (getter)py_ue_iplugin_file_version, NULL, (char *)"", NULL },
	{ (char*)"friendly_name", (getter)py_ue_iplugin_friendly_name, NULL, (char *)"", NULL },
	{ (char*)"marketplace_url", (getter)py_ue_iplugin_marketplace_url, NULL, (char *)"", NULL },
	{ (char*)"support_url", (getter)py_ue_iplugin_support_url, NULL, (char *)"", NULL },
	{ (char*)"version", (getter)py_ue_iplugin_version, NULL, (char *)"", NULL },
	{ (char*)"version_name", (getter)py_ue_iplugin_version_name, NULL, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyIPlugin_str(ue_PyIPlugin *self)
{
	return PyUnicode_FromFormat("<unreal_engine.IPlugin {'name': '%s'}>",
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


void ue_python_init_iplugin(PyObject *ue_module)
{
	ue_PyIPluginType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyIPluginType) < 0)
		return;

	Py_INCREF(&ue_PyIPluginType);
	PyModule_AddObject(ue_module, "IPlugin", (PyObject *)&ue_PyIPluginType);
}

PyObject *py_ue_new_iplugin(IPlugin *plugin)
{
	ue_PyIPlugin *ret = (ue_PyIPlugin *)PyObject_New(ue_PyIPlugin, &ue_PyIPluginType);
	ret->plugin = plugin;
	return (PyObject *)ret;
}
#endif