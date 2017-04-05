#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

static void ue_py_fassetimportinfo_dealloc(ue_PyFAssetImportInfo *self) {
	if (self->source_files)
		Py_XDECREF(self->source_files);

#if PY_MAJOR_VERSION < 3
	self->ob_type->tp_free((PyObject*)self);
#else
	Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

static PyObject *ue_py_fassetimportinfo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	ue_PyFAssetImportInfo *self;

	self = (ue_PyFAssetImportInfo *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->source_files = nullptr;
	}
	return (PyObject *)self;
}

static int ue_py_fassetimportinfo_init(ue_PyFAssetImportInfo *self, PyObject *args, PyObject *kwargs) {
	return 0;
}

static PyObject *py_ue_fassetimportinfo_get_source_files(ue_PyFAssetImportInfo *self, void *closure) {
	if (!self->source_files) {
		self->source_files = PyList_New(self->asset_import_info->SourceFiles.Num());
		for (int i = 0; i < self->asset_import_info->SourceFiles.Num(); i++)
			PyList_SET_ITEM(self->source_files, i, py_ue_new_fsourcefile(&self->asset_import_info->SourceFiles[i]));
	}
	return self->source_files;
}

static PyGetSetDef ue_PyFAssetImportInfo_getseters[] = {
	{ (char*)"source_files", (getter)py_ue_fassetimportinfo_get_source_files, NULL, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFAssetImportInfoType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FAssetImportInfo", /* tp_name */
	sizeof(ue_PyFAssetImportInfo),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_py_fassetimportinfo_dealloc, /* tp_dealloc */
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
	"Unreal Engine FAssetImportInfo", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,                         /* tp_members */
	ue_PyFAssetImportInfo_getseters, /* tp_getset */
};

PyObject *py_ue_new_fassetimportinfo(FAssetImportInfo *asset_import_info) {
	ue_PyFAssetImportInfo *ret = (ue_PyFAssetImportInfo *)PyObject_CallObject((PyObject *)&ue_PyFAssetImportInfoType, NULL);
	ret->asset_import_info = asset_import_info;
	return (PyObject *)ret;
}

ue_PyFAssetImportInfo *py_ue_is_fassetimportinfo(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFAssetImportInfoType))
		return nullptr;
	return (ue_PyFAssetImportInfo *)obj;
}

static PyObject *py_ue_fsourcefile_get_file_hash(ue_PyFSourceFile *self, void *closure) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*LexicalConversion::ToString(self->source_file->FileHash)));
}

static PyObject *py_ue_fsourcefile_get_relative_filename(ue_PyFSourceFile *self, void *closure) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->source_file->RelativeFilename));
}

static PyObject *py_ue_fsourcefile_get_timestamp(ue_PyFSourceFile *self, void *closure) {
	return PyLong_FromLong(self->source_file->Timestamp.ToUnixTimestamp());
}

static PyGetSetDef ue_PyFSourceFile_getseters[] = {
	{ (char*)"file_hash", (getter)py_ue_fsourcefile_get_file_hash, NULL, (char *)"", NULL },
	{ (char *)"relative_filename", (getter)py_ue_fsourcefile_get_relative_filename, NULL, (char *)"", NULL },
	{ (char *)"timestamp", (getter)py_ue_fsourcefile_get_timestamp, NULL, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static int ue_py_fsourcefile_init(ue_PyFSourceFile *self, PyObject *args, PyObject *kwargs) {
	return 0;
}

static PyTypeObject ue_PyFSourceFileType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FSourceFile", /* tp_name */
	sizeof(ue_PyFSourceFile),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,                         /* tp_dealloc */
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
	"Unreal Engine FSourceFile", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,                         /* tp_members */
	ue_PyFSourceFile_getseters, /* tp_getset */
};

PyObject *py_ue_new_fsourcefile(FAssetImportInfo::FSourceFile *source_file) {
	ue_PyFSourceFile *ret = (ue_PyFSourceFile *)PyObject_CallObject((PyObject *)&ue_PyFSourceFileType, NULL);
	ret->source_file = source_file;
	return (PyObject *)ret;
}

ue_PyFSourceFile *py_ue_is_fsourcefile(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFSourceFileType))
		return nullptr;
	return (ue_PyFSourceFile *)obj;
}

void ue_python_init_fassetimportinfo(PyObject *ue_module) {
	ue_PyFAssetImportInfoType.tp_new = ue_py_fassetimportinfo_new;;
	ue_PyFAssetImportInfoType.tp_init = (initproc)ue_py_fassetimportinfo_init;
	if (PyType_Ready(&ue_PyFAssetImportInfoType) < 0)
		return;

	ue_PyFSourceFileType.tp_new = PyType_GenericNew;;
	ue_PyFSourceFileType.tp_init = (initproc)ue_py_fsourcefile_init;
	if (PyType_Ready(&ue_PyFSourceFileType) < 0)
		return;

	Py_INCREF(&ue_PyFAssetImportInfoType);
	PyModule_AddObject(ue_module, "FAssetImportInfo", (PyObject *)&ue_PyFAssetImportInfoType);
	
	Py_INCREF(&ue_PyFSourceFileType);
	PyModule_AddObject(ue_module, "FSourceFile", (PyObject *)&ue_PyFSourceFileType);
}

#endif
