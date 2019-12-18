#include "UEPyUserDefinedStruct.h"


#if WITH_EDITOR
#include "Runtime/Engine/Classes/Engine/UserDefinedStruct.h"
#include "Editor/UnrealEd/Classes/UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Editor/UnrealEd/Public/Kismet2/StructureEditorUtils.h"



PyObject *py_ue_struct_add_variable(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_var;

	if (!PyArg_ParseTuple(args, "O:struct_add_variable", &py_var))
	{
		return nullptr;
	}

	UUserDefinedStruct *u_struct = ue_py_check_type<UUserDefinedStruct>(self);
	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "uobject is not a UUserDefinedStruct");

	FStructVariableDescription *var = ue_py_check_struct<FStructVariableDescription>(py_var);
	if (!var)
		return PyErr_Format(PyExc_Exception, "argument is not a FStructVariableDescription");

	var->VarGuid = FGuid::NewGuid();

	FStructureEditorUtils::GetVarDesc(u_struct).Add(*var);

	FStructureEditorUtils::OnStructureChanged(u_struct, FStructureEditorUtils::EStructureEditorChangeInfo::AddedVariable);

	return py_ue_new_owned_uscriptstruct(FindObject<UScriptStruct>(ANY_PACKAGE, UTF8_TO_TCHAR((char *)"Guid")), (uint8 *)&var->VarGuid);
}

PyObject *py_ue_struct_get_variables(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UUserDefinedStruct *u_struct = ue_py_check_type<UUserDefinedStruct>(self);
	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "uobject is not a UUserDefinedStruct");

	TArray<FStructVariableDescription> variables = FStructureEditorUtils::GetVarDesc(u_struct);

	PyObject *py_list = PyList_New(0);
	for (FStructVariableDescription description : variables)
	{
		PyList_Append(py_list, py_ue_new_owned_uscriptstruct(FStructVariableDescription::StaticStruct(), (uint8*)&description));
	}
	return py_list;
}

PyObject *py_ue_struct_remove_variable(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_guid;

	if (!PyArg_ParseTuple(args, "O:struct_remove_variable", &py_guid))
	{
		return nullptr;
	}

	UUserDefinedStruct *u_struct = ue_py_check_type<UUserDefinedStruct>(self);
	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "uobject is not a UUserDefinedStruct");

	FGuid *guid = ue_py_check_fguid(py_guid);
	if (!guid)
		return PyErr_Format(PyExc_Exception, "object is not a FGuid");

	if (FStructureEditorUtils::RemoveVariable(u_struct, *guid))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_struct_move_variable_up(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_guid;

	if (!PyArg_ParseTuple(args, "O:struct_move_variable_up", &py_guid))
	{
		return nullptr;
	}

	UUserDefinedStruct *u_struct = ue_py_check_type<UUserDefinedStruct>(self);
	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "uobject is not a UUserDefinedStruct");

	FGuid *guid = ue_py_check_fguid(py_guid);
	if (!guid)
		return PyErr_Format(PyExc_Exception, "object is not a FGuid");

	if (FStructureEditorUtils::MoveVariable(u_struct, *guid, FStructureEditorUtils::EMoveDirection::MD_Up))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_struct_move_variable_down(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_guid;

	if (!PyArg_ParseTuple(args, "O:struct_move_variable_down", &py_guid))
	{
		return nullptr;
	}

	UUserDefinedStruct *u_struct = ue_py_check_type<UUserDefinedStruct>(self);
	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "uobject is not a UUserDefinedStruct");

	FGuid *guid = ue_py_check_fguid(py_guid);
	if (!guid)
		return PyErr_Format(PyExc_Exception, "object is not a FGuid");

	if (FStructureEditorUtils::MoveVariable(u_struct, *guid, FStructureEditorUtils::EMoveDirection::MD_Down))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

#endif
