#include "UEPyDataTable.h"


#if WITH_EDITOR

#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Editor/UnrealEd/Public/DataTableEditorUtils.h"

PyObject *py_ue_data_table_add_row(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	PyObject *py_row;

	if (!PyArg_ParseTuple(args, "sO:data_table_add_row", &name, &py_row))
	{
		return nullptr;
	}

	UDataTable *data_table = ue_py_check_type<UDataTable>(self);
	if (!data_table)
		return PyErr_Format(PyExc_Exception, "uobject is not a UDataTable");

	ue_PyUScriptStruct *u_struct = py_ue_is_uscriptstruct(py_row);
	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "argument is not a UScriptStruct");

	if (data_table->RowStruct != u_struct->u_struct)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a %s", TCHAR_TO_UTF8(*data_table->RowStruct->GetName()));
	}

	FName row_name = FName(UTF8_TO_TCHAR(name));

	uint8 *row = FDataTableEditorUtils::AddRow(data_table, row_name);
	if (!row)
		return PyErr_Format(PyExc_Exception, "unable to add row");
	data_table->RowStruct->InitializeStruct(row);
	data_table->RowStruct->CopyScriptStruct(row, u_struct->u_struct_ptr);

	Py_RETURN_NONE;

}

PyObject *py_ue_data_table_remove_row(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;

	if (!PyArg_ParseTuple(args, "s:data_table_remove_row", &name))
	{
		return nullptr;
	}

	UDataTable *data_table = ue_py_check_type<UDataTable>(self);
	if (!data_table)
		return PyErr_Format(PyExc_Exception, "uobject is not a UDataTable");

	FName row_name = FName(UTF8_TO_TCHAR(name));

	if (FDataTableEditorUtils::RemoveRow(data_table, row_name))
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject *py_ue_data_table_rename_row(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	char *new_name;

	if (!PyArg_ParseTuple(args, "ss:data_table_rename_row", &name, &new_name))
	{
		return nullptr;
	}

	UDataTable *data_table = ue_py_check_type<UDataTable>(self);
	if (!data_table)
		return PyErr_Format(PyExc_Exception, "uobject is not a UDataTable");

	FName row_name = FName(UTF8_TO_TCHAR(name));
	FName row_new_name = FName(UTF8_TO_TCHAR(new_name));


	if (FDataTableEditorUtils::RenameRow(data_table, row_name, row_new_name))
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject *py_ue_data_table_as_dict(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UDataTable *data_table = ue_py_check_type<UDataTable>(self);
	if (!data_table)
		return PyErr_Format(PyExc_Exception, "uobject is not a UDataTable");

	PyObject *py_dict = PyDict_New();

#if ENGINE_MINOR_VERSION > 20
	for (TMap<FName, uint8*>::TConstIterator RowMapIter(data_table->GetRowMap().CreateConstIterator()); RowMapIter; ++RowMapIter)
#else
	for (TMap<FName, uint8*>::TConstIterator RowMapIter(data_table->RowMap.CreateConstIterator()); RowMapIter; ++RowMapIter)
#endif
	{
		PyDict_SetItemString(py_dict, TCHAR_TO_UTF8(*RowMapIter->Key.ToString()), py_ue_new_owned_uscriptstruct(data_table->RowStruct, RowMapIter->Value));
	}

	return py_dict;
}

PyObject *py_ue_data_table_as_json(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	int flags = 0;

	if (!PyArg_ParseTuple(args, "|i:data_table_as_json", &flags))
	{
		return nullptr;
	}

	UDataTable *data_table = ue_py_check_type<UDataTable>(self);
	if (!data_table)
		return PyErr_Format(PyExc_Exception, "uobject is not a UDataTable");

	return PyUnicode_FromString(TCHAR_TO_UTF8(*data_table->GetTableAsJSON((EDataTableExportFlags)flags)));
}

PyObject *py_ue_data_table_find_row(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;

	if (!PyArg_ParseTuple(args, "s:data_table_find_row", &name))
	{
		return nullptr;
	}

	UDataTable *data_table = ue_py_check_type<UDataTable>(self);
	if (!data_table)
		return PyErr_Format(PyExc_Exception, "uobject is not a UDataTable");

	uint8 **data = nullptr;
#if ENGINE_MINOR_VERSION > 20
	data = (uint8 **)data_table->GetRowMap().Find(FName(UTF8_TO_TCHAR(name)));
#else
	data = data_table->RowMap.Find(FName(UTF8_TO_TCHAR(name)));
#endif
	if (!data)
	{
		return PyErr_Format(PyExc_Exception, "key not found in UDataTable");
	}

	return py_ue_new_owned_uscriptstruct(data_table->RowStruct, *data);
}

PyObject *py_ue_data_table_get_all_rows(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UDataTable *data_table = ue_py_check_type<UDataTable>(self);
	if (!data_table)
		return PyErr_Format(PyExc_Exception, "uobject is not a UDataTable");

	PyObject *py_list = PyList_New(0);

#if ENGINE_MINOR_VERSION > 20
	for (TMap<FName, uint8*>::TConstIterator RowMapIter(data_table->GetRowMap().CreateConstIterator()); RowMapIter; ++RowMapIter)
#else
	for (TMap<FName, uint8*>::TConstIterator RowMapIter(data_table->RowMap.CreateConstIterator()); RowMapIter; ++RowMapIter)
#endif
	{
		PyList_Append(py_list, py_ue_new_owned_uscriptstruct(data_table->RowStruct, RowMapIter->Value));
	}

	return py_list;
}

#endif
