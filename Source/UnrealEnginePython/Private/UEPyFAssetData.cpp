#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

static PyObject *py_ue_fassetdata_get_asset(ue_PyFAssetData *self, PyObject * args) {
	return (PyObject *)ue_get_python_wrapper(self->asset_data->GetAsset());
}

static PyObject *py_ue_fassetdata_is_asset_loaded(ue_PyFAssetData *self, PyObject * args) {
	if (self->asset_data->IsAssetLoaded())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyMethodDef ue_PyFAssetData_methods[] = {
	{ "get_asset", (PyCFunction)py_ue_fassetdata_get_asset, METH_VARARGS, "" },
	{ "is_asset_loaded", (PyCFunction)py_ue_fassetdata_is_asset_loaded, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyMemberDef ue_PyFAssetData_members[] = {
	{ (char *)"asset_class", T_OBJECT, offsetof(ue_PyFAssetData, asset_class), READONLY, (char *)"asset_class" },
	{ (char *)"asset_name", T_OBJECT, offsetof(ue_PyFAssetData, asset_name), READONLY, (char *)"asset_name" },
	{ (char *)"group_names", T_OBJECT, offsetof(ue_PyFAssetData, group_names), READONLY, (char *)"group_names" },
	{ (char *)"object_path", T_OBJECT, offsetof(ue_PyFAssetData, object_path), READONLY, (char *)"object_path" },
	{ (char *)"package_flags", T_OBJECT, offsetof(ue_PyFAssetData, package_flags), READONLY, (char *)"package_flags" },
	{ (char *)"package_name", T_OBJECT, offsetof(ue_PyFAssetData, package_name), READONLY, (char *)"package_name" },
	{ (char *)"package_path", T_OBJECT, offsetof(ue_PyFAssetData, package_path), READONLY, (char *)"package_path" },
	{ (char *)"tags_and_values", T_OBJECT, offsetof(ue_PyFAssetData, tags_and_values), READONLY, (char *)"tags_and_values" },
	{ NULL }  /* Sentinel */
};

static int ue_py_fassetdata_init(ue_PyFAssetData *self, PyObject *args, PyObject *kwargs) {
	return 0;
}

static void ue_py_fassetdata_dealloc(ue_PyFAssetData *self) {
	Py_XDECREF(self->asset_class);
	Py_XDECREF(self->asset_name);
	Py_XDECREF(self->group_names);
	Py_XDECREF(self->object_path);
	Py_XDECREF(self->package_flags);
	Py_XDECREF(self->package_name);
	Py_XDECREF(self->package_path);
	Py_XDECREF(self->tags_and_values);

#if PY_MAJOR_VERSION < 3
	self->ob_type->tp_free((PyObject*)self);
#else
	Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

static PyTypeObject ue_PyFAssetDataType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FAssetData", /* tp_name */
	sizeof(ue_PyFAssetData),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_py_fassetdata_dealloc,   /* tp_dealloc */
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
	"Unreal Engine FAssetData", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFAssetData_methods,    /* tp_methods */
	ue_PyFAssetData_members,   /* tp_members */
	0,                         /* tp_getset */
};

void ue_python_init_fassetdata(PyObject *ue_module) {
	ue_PyFAssetDataType.tp_new = PyType_GenericNew;;
	ue_PyFAssetDataType.tp_init = (initproc)ue_py_fassetdata_init;
	if (PyType_Ready(&ue_PyFAssetDataType) < 0)
		return;

	Py_INCREF(&ue_PyFAssetDataType);
	PyModule_AddObject(ue_module, "FAssetData", (PyObject *)&ue_PyFAssetDataType);
}

PyObject *py_ue_new_fassetdata(FAssetData *asset_data) {
	ue_PyFAssetData *ret = (ue_PyFAssetData *)PyObject_New(ue_PyFAssetData, &ue_PyFAssetDataType);
		
	ret->asset_class = PyUnicode_FromString(TCHAR_TO_UTF8(*asset_data->AssetClass.ToString()));
	ret->asset_name = PyUnicode_FromString(TCHAR_TO_UTF8(*asset_data->AssetName.ToString()));
	ret->group_names = PyUnicode_FromString(TCHAR_TO_UTF8(*asset_data->GroupNames.ToString()));
	ret->object_path = PyUnicode_FromString(TCHAR_TO_UTF8(*asset_data->ObjectPath.ToString()));
	ret->package_flags = PyLong_FromLong(asset_data->PackageFlags);
	ret->package_name = PyUnicode_FromString(TCHAR_TO_UTF8(*asset_data->PackageName.ToString()));
	ret->package_path = PyUnicode_FromString(TCHAR_TO_UTF8(*asset_data->PackagePath.ToString()));
	ret->tags_and_values = PyDict_New();
	for (auto It = asset_data->TagsAndValues.CreateConstIterator(); It; ++It)
	{
		PyDict_SetItem(ret->tags_and_values, 
			           PyUnicode_FromString(TCHAR_TO_UTF8(*It->Key.ToString())), 
					   PyUnicode_FromString(TCHAR_TO_UTF8(*It->Value)));
	}

	ret->asset_data = asset_data;
	return (PyObject *)ret;
}

ue_PyFAssetData *py_ue_is_fassetdata(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFAssetDataType))
		return nullptr;
	return (ue_PyFAssetData *)obj;
}

#endif
