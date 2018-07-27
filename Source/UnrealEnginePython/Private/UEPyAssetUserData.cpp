#include "UEPyAssetUserData.h"

#if WITH_EDITOR

PyObject *py_ue_asset_import_data(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UStruct *u_struct = (UStruct *)self->ue_object->GetClass();
	UClassProperty *u_property = (UClassProperty *)u_struct->FindPropertyByName(TEXT("AssetImportData"));
	if (!u_property)
	{
		return PyErr_Format(PyExc_Exception, "UObject does not have asset import data.");

	}
	UAssetImportData *import_data = (UAssetImportData *)u_property->GetPropertyValue_InContainer(self->ue_object);
	FAssetImportInfo *import_info = &import_data->SourceData;
	PyObject *ret = PyList_New(import_info->SourceFiles.Num());
	for (int i = 0; i < import_info->SourceFiles.Num(); i++)
	{
		PyObject *py_source_file = PyDict_New();
		PyDict_SetItemString(py_source_file, "absolute_filepath", PyUnicode_FromString(TCHAR_TO_UTF8(*import_data->ResolveImportFilename(import_info->SourceFiles[i].RelativeFilename, NULL))));
		PyDict_SetItemString(py_source_file, "relative_filepath", PyUnicode_FromString(TCHAR_TO_UTF8(*import_info->SourceFiles[i].RelativeFilename)));
		PyDict_SetItemString(py_source_file, "timestamp", PyLong_FromLong(import_info->SourceFiles[i].Timestamp.ToUnixTimestamp()));
#if ENGINE_MINOR_VERSION > 19
		PyDict_SetItemString(py_source_file, "filehash", PyUnicode_FromString(TCHAR_TO_UTF8(*LexToString(import_info->SourceFiles[i].FileHash))));
#else
		PyDict_SetItemString(py_source_file, "filehash", PyUnicode_FromString(TCHAR_TO_UTF8(*LexicalConversion::ToString(import_info->SourceFiles[i].FileHash))));
#endif
		PyList_SetItem(ret, i, py_source_file);
	}
	return ret;
}

PyObject *py_ue_asset_import_data_set_sources(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_files;
	if (!PyArg_ParseTuple(args, "O:asset_import_data_set_sources", &py_files))
	{
		return nullptr;
	}

	TArray<FString> filenames;

	UStruct *u_struct = (UStruct *)self->ue_object->GetClass();
	UClassProperty *u_property = (UClassProperty *)u_struct->FindPropertyByName(TEXT("AssetImportData"));
	if (!u_property)
	{
		return PyErr_Format(PyExc_Exception, "UObject does not have asset import data.");
	}

	if (PyUnicodeOrString_Check(py_files))
	{
		filenames.Add(FString(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(py_files))));
	}
	else
	{
		PyObject *py_iter = PyObject_GetIter(py_files);
		if (!py_iter)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a string or an interable of strings");
		}

		while (PyObject *py_item = PyIter_Next(py_iter))
		{
			if (!PyUnicodeOrString_Check(py_item))
			{
				Py_DECREF(py_iter);
				return PyErr_Format(PyExc_Exception, "argument is not a string or an interable of strings");
			}
			filenames.Add(FString(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(py_item))));
		}

		Py_DECREF(py_iter);
	}


	UAssetImportData *import_data = (UAssetImportData *)u_property->GetPropertyValue_InContainer(self->ue_object);
	FAssetImportInfo *import_info = &import_data->SourceData;

	TArray<FAssetImportInfo::FSourceFile> sources;

	for (FString filename : filenames)
	{
		sources.Add(FAssetImportInfo::FSourceFile(filename));
	}

	import_info->SourceFiles = sources;

	Py_RETURN_NONE;
}
#endif
