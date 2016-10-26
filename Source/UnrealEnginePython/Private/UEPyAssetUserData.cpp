#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

PyObject *py_ue_asset_import_data(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UStruct *u_struct = (UStruct *)self->ue_object->GetClass();
	UClassProperty *u_property = (UClassProperty *)u_struct->FindPropertyByName(TEXT("AssetImportData"));
	if (!u_property) {
		return PyErr_Format(PyExc_Exception, "UObject does not have asset import data.");
		
	}
	UAssetImportData *import_data = (UAssetImportData *)u_property->GetPropertyValue_InContainer(self->ue_object);
	FAssetImportInfo *import_info = &import_data->SourceData;
	PyObject *ret = PyList_New(import_info->SourceFiles.Num());
	for (int i=0; i < import_info->SourceFiles.Num(); i++) {
		PyObject *py_source_file = PyDict_New();
		PyDict_SetItemString(py_source_file, "absolute_filepath", PyUnicode_FromString(TCHAR_TO_UTF8(*import_data->ResolveImportFilename(import_info->SourceFiles[i].RelativeFilename, NULL))));
		PyDict_SetItemString(py_source_file, "relative_filepath", PyUnicode_FromString(TCHAR_TO_UTF8(*import_info->SourceFiles[i].RelativeFilename)));
		PyDict_SetItemString(py_source_file, "timestamp", PyLong_FromLong(import_info->SourceFiles[i].Timestamp.ToUnixTimestamp()));
		PyDict_SetItemString(py_source_file, "filehash", PyUnicode_FromString(TCHAR_TO_UTF8(*LexicalConversion::ToString(import_info->SourceFiles[i].FileHash))));
		PyList_SetItem(ret, i, py_source_file);
	}
	return ret;
}
#endif
