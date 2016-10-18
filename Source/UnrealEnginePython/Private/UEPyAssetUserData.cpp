#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_asset_import_data(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UStruct *u_struct = (UStruct *)self->ue_object->GetClass();
	UClassProperty *u_property = (UClassProperty *)u_struct->FindPropertyByName(TEXT("AssetImportData"));
	if (!u_property) {
		return PyErr_Format(PyExc_Exception, "UObject does not have asset import data.");
		
	}
	UAssetImportData *import_data = (UAssetImportData *)u_property->GetPropertyValue_InContainer(self->ue_object);
	FString absolute_filepath = import_data->ResolveImportFilename(import_data->GetFirstFilename(), NULL);

	PyObject *ret = PyDict_New();
	PyDict_SetItemString(ret, "absolute_filepath", PyUnicode_FromString(TCHAR_TO_UTF8(*absolute_filepath)));
	PyDict_SetItemString(ret, "relative_filepath", PyUnicode_FromString(TCHAR_TO_UTF8(*import_data->GetFirstFilename())));

	return ret;
}
