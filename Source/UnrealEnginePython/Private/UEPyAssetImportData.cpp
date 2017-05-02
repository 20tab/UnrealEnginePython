#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

PyObject *py_ue_get_assetimportdata(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UStruct *u_struct = (UStruct *)self->ue_object->GetClass();
	UClassProperty *u_property = (UClassProperty *)u_struct->FindPropertyByName(TEXT("AssetImportData"));
	if (!u_property) {
		return PyErr_Format(PyExc_Exception, "UObject does not have asset import data.");

	}
	UAssetImportData *import_data = (UAssetImportData *)u_property->GetPropertyValue_InContainer(self->ue_object);
	PyObject *ret = py_ue_new_assetimportdata(import_data);
	return ret;
}

static PyObject *py_ue_assetimportdata_extract_filenames(ue_PyAssetImportData *self, PyObject * args) {
	TArray<FString> extracted_filenames = self->asset_import_data->ExtractFilenames();
	PyObject *filenames_list = PyList_New(extracted_filenames.Num());
	for (int i = 0; i < extracted_filenames.Num(); i++)
		PyList_SetItem(filenames_list, i, PyUnicode_FromString(TCHAR_TO_UTF8(*extracted_filenames[i])));
	return filenames_list;
}

static PyObject *py_ue_assetimportdata_get_first_filename(ue_PyAssetImportData *self, PyObject * args) {
	PyObject *ret = PyUnicode_FromString(TCHAR_TO_UTF8(*self->asset_import_data->GetFirstFilename()));
	Py_INCREF(ret);
	return ret;
}

static PyObject *py_ue_assetimportdata_resolve_import_filename(ue_PyAssetImportData *self, PyObject * args) {
	char *rel_path;
	PyObject *package = nullptr;
	if (!PyArg_ParseTuple(args, "s|O:resolve_import_filename", &rel_path, &package)) {
		return NULL;
	}
	
	UPackage *upackage = nullptr;
	if (package && package != Py_None) {
		ue_PyUObject *py_obj = (ue_PyUObject *)package;
		upackage = (UPackage *)py_obj->ue_object;
	}
	else if (package == nullptr) {
		upackage = self->asset_import_data->GetOutermost();
	}

	PyObject *ret = PyUnicode_FromString(TCHAR_TO_UTF8(*self->asset_import_data->ResolveImportFilename(FString(UTF8_TO_TCHAR(rel_path)), upackage)));
	Py_INCREF(ret);
	return ret;
}

static PyObject *py_ue_assetimportdata_update(ue_PyAssetImportData *self, PyObject * args) {
	char *abs_path;
	if (!PyArg_ParseTuple(args, "s|:update", &abs_path)) {
		return NULL;
	}
	self->asset_import_data->Update(FString(UTF8_TO_TCHAR(abs_path)));
	Py_RETURN_NONE;
}

static PyObject *py_ue_assetimportdata_update_filename_only(ue_PyAssetImportData *self, PyObject * args) {
	char *in_path;
	if (!PyArg_ParseTuple(args, "s|:update_filename_only", &in_path)) {
		return NULL;
	}
	self->asset_import_data->UpdateFilenameOnly(FString(UTF8_TO_TCHAR(in_path)));
	Py_RETURN_NONE;
}

static PyObject *py_ue_assetimportdata_mark_package_dirty(ue_PyAssetImportData *self, PyObject * args) {
	if (self->asset_import_data->MarkPackageDirty())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_assetimportdata_get_source_data(ue_PyAssetImportData *self, void *closure) {
	if (!self->source_data)
		self->source_data = py_ue_new_fassetimportinfo(&self->asset_import_data->SourceData);
		Py_INCREF(self->source_data);
	return self->source_data;
}

static PyGetSetDef ue_PyAssetImportData_getseters[] = {
	{ (char*)"source_data", (getter)py_ue_assetimportdata_get_source_data, NULL, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyMethodDef ue_PyAssetImportData_methods[] = {
	{ "extract_filenames", (PyCFunction)py_ue_assetimportdata_extract_filenames, METH_VARARGS, "" },
	{ "get_first_filename", (PyCFunction)py_ue_assetimportdata_get_first_filename, METH_VARARGS, "" },
	{ "resolve_import_filename", (PyCFunction)py_ue_assetimportdata_resolve_import_filename, METH_VARARGS, "" },
	{ "update", (PyCFunction)py_ue_assetimportdata_update, METH_VARARGS, "" },
	{ "update_filename_only", (PyCFunction)py_ue_assetimportdata_update_filename_only, METH_VARARGS, "" },
	{ "mark_package_dirty", (PyCFunction)py_ue_assetimportdata_mark_package_dirty, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static void ue_py_assetimportdata_dealloc(ue_PyAssetImportData *self) {
	if (self->source_data != nullptr)
		Py_XDECREF(self->source_data);

#if PY_MAJOR_VERSION < 3
	self->ob_type->tp_free((PyObject*)self);
#else
	Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

static int ue_py_assetimportdata_init(ue_PyAssetImportData *self, PyObject *args, PyObject *kwargs) {
	return 0;
}

static PyObject * ue_py_assetimportdata_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	ue_PyAssetImportData *self;

	self = (ue_PyAssetImportData *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->source_data = nullptr;
	}
	return (PyObject *)self;
}

static PyTypeObject ue_PyAssetImportDataType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UAssetImportData", /* tp_name */
	sizeof(ue_PyAssetImportData),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_py_assetimportdata_dealloc, /* tp_dealloc */
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
	"Unreal Engine UAssetImportData", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyAssetImportData_methods,   /* tp_methods */
	0,                         /* tp_members */
	ue_PyAssetImportData_getseters, /* tp_getset */
};

void ue_python_init_assetimportdata(PyObject *ue_module) {
	ue_PyAssetImportDataType.tp_new = ue_py_assetimportdata_new;
	ue_PyAssetImportDataType.tp_init = (initproc)ue_py_assetimportdata_init;
	if (PyType_Ready(&ue_PyAssetImportDataType) < 0)
		return;

	Py_INCREF(&ue_PyAssetImportDataType);
	PyModule_AddObject(ue_module, "UAssetImportData", (PyObject *)&ue_PyAssetImportDataType);
}

PyObject *py_ue_new_assetimportdata(UAssetImportData *asset_import_data) {
	ue_PyAssetImportData *ret = (ue_PyAssetImportData *)PyObject_CallObject((PyObject *)&ue_PyAssetImportDataType, NULL);
	ret->asset_import_data = asset_import_data;
	return (PyObject *)ret;
}

ue_PyAssetImportData *py_ue_is_assetimportdata(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyAssetImportDataType))
		return nullptr;
	return (ue_PyAssetImportData *)obj;
}

#endif
