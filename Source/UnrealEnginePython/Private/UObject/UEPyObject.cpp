#include "UEPyObject.h"

#include "PythonDelegate.h"
#include "PythonFunction.h"

#if WITH_EDITOR
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "ObjectTools.h"
#include "UnrealEd.h"
#include "Runtime/Core/Public/HAL/FeedbackContextAnsi.h"

#include "Wrappers/UEPyFObjectThumbnail.h"
#endif
#include <ObjectMacros.h>

PyObject *py_ue_get_class(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	Py_RETURN_UOBJECT(self->ue_object->GetClass());
}

PyObject *py_ue_class_generated_by(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UClass *u_class = ue_py_check_type<UClass>(self);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "uobject is a not a UClass");

	UObject *u_object = u_class->ClassGeneratedBy;
	if (!u_object)
		Py_RETURN_NONE;

	Py_RETURN_UOBJECT(u_object);
}

PyObject *py_ue_class_get_flags(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UClass *u_class = ue_py_check_type<UClass>(self);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "uobject is a not a UClass");

	return PyLong_FromUnsignedLongLong((uint64)u_class->GetClassFlags());
}

PyObject *py_ue_class_set_flags(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	uint64 flags;
	if (!PyArg_ParseTuple(args, "K:class_set_flags", &flags))
	{
		return nullptr;
	}

	UClass *u_class = ue_py_check_type<UClass>(self);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "uobject is a not a UClass");

	u_class->ClassFlags = (EClassFlags)flags;

	Py_RETURN_NONE;
}

PyObject *py_ue_class_has_any_flags(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	uint64 flags;
	if (!PyArg_ParseTuple(args, "K:class_has_any_flags", &flags))
	{
		return nullptr;
	}

	UClass *u_class = ue_py_check_type<UClass>(self);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "uobject is a not a UClass");

	if (u_class->HasAnyClassFlags((EClassFlags)flags))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

PyObject *py_ue_get_obj_flags(ue_PyUObject * self, PyObject * args)
{
	ue_py_check(self);

	return PyLong_FromUnsignedLongLong((uint64)self->ue_object->GetFlags());
}

PyObject *py_ue_set_obj_flags(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	uint64 flags;
	if (!PyArg_ParseTuple(args, "K:set_obj_flags", &flags))
	{
		return nullptr;
	}

	self->ue_object->SetFlags((EObjectFlags)flags);

	Py_RETURN_NONE;
}


#if WITH_EDITOR

#if ENGINE_MINOR_VERSION >= 19
PyObject *py_ue_is_data_valid(ue_PyUObject * self, PyObject * args)
{
    ue_py_check(self);

    PyObject* py_validErrors = nullptr;
    if (!PyArg_ParseTuple(args, "O:is_data_valid", &py_validErrors))
    {
        return NULL;
    }

    UObject* uobj = ue_py_check_type<UObject>(self);
    if (!uobj)
    { return PyErr_Format(PyExc_Exception, "object is not a valid UObject"); }

    if (!PyList_Check(py_validErrors))
    {
        return PyErr_Format(PyExc_Exception, "validation errors must be an array ");
    }

    TArray<FText> newValidErrors;
    EDataValidationResult validationResult = uobj->IsDataValid(newValidErrors);
    for (const FText& validError : newValidErrors)
    {
        PyList_Append(py_validErrors, PyUnicode_FromString(TCHAR_TO_UTF8(*validError.ToString())));
    }

    return PyLong_FromLong((uint8)validationResult);
}
#endif

PyObject *py_ue_class_set_config_name(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *config_name;
	if (!PyArg_ParseTuple(args, "s:class_set_config_name", &config_name))
	{
		return nullptr;
	}

	UClass *u_class = ue_py_check_type<UClass>(self);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "uobject is a not a UClass");

	u_class->ClassConfigName = UTF8_TO_TCHAR(config_name);

	Py_RETURN_NONE;
}

PyObject *py_ue_class_get_config_name(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);


	UClass *u_class = ue_py_check_type<UClass>(self);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "uobject is a not a UClass");

	return PyUnicode_FromString(TCHAR_TO_UTF8(*u_class->ClassConfigName.ToString()));
}
#endif

PyObject *py_ue_get_property_struct(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:get_property_struct", &property_name))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	UStructProperty *prop = Cast<UStructProperty>(u_property);
	if (!prop)
		return PyErr_Format(PyExc_Exception, "object is not a StructProperty");
	return py_ue_new_uscriptstruct(prop->Struct, prop->ContainerPtrToValuePtr<uint8>(self->ue_object));
}

PyObject *py_ue_output_referencers(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	FStringOutputDevice outputDevice;

	// Get referencer information
	FReferencerInformationList outputList;
	self->ue_object->OutputReferencers((FOutputDevice &)outputDevice);

	// Get actual list of objects referencing this actor
	TArray<FReferencerInformation> outInternalRefs, outExternalRefs;
	self->ue_object->RetrieveReferencers(&outInternalRefs, &outExternalRefs);

	PyObject *retList = PyList_New(0);

	// Pack objects into list
	for (FReferencerInformation & info : outExternalRefs)
	{
		ue_PyUObject *py_obj = ue_get_python_uobject(info.Referencer);
		if (!py_obj)
			continue;
		Py_INCREF(py_obj);
		PyList_Append(retList, (PyObject *)py_obj);
	}
	// Pack into tuple of type(String, List[UObject]) and return
	PyObject * retTuple = PyTuple_New(2);
	PyTuple_SetItem(retTuple, 0, PyUnicode_FromString(TCHAR_TO_UTF8(*outputDevice)));
	PyTuple_SetItem(retTuple, 1, retList);
	return retTuple;
}

PyObject *py_ue_get_super_class(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UClass *u_class = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		u_class = (UClass *)self->ue_object;
	}
	else
	{
		u_class = self->ue_object->GetClass();
	}

	Py_RETURN_UOBJECT(u_class->GetSuperClass());
}

PyObject *py_ue_get_outer(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UObject *outer = self->ue_object->GetOuter();
	if (!outer)
		Py_RETURN_NONE;

	Py_RETURN_UOBJECT(outer);
}

PyObject *py_ue_get_outermost(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UObject *outermost = self->ue_object->GetOutermost();
	if (!outermost)
		Py_RETURN_NONE;

	Py_RETURN_UOBJECT(outermost);
}

PyObject *py_ue_conditional_begin_destroy(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	self->ue_object->ConditionalBeginDestroy();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_is_valid(ue_PyUObject * self, PyObject * args)
{
	if (!self->ue_object || !self->ue_object->IsValidLowLevel() || self->ue_object->IsPendingKillOrUnreachable())
	{
		Py_RETURN_FALSE;
	}

	Py_RETURN_TRUE;
}

PyObject *py_ue_is_a(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:is_a", &obj))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(obj))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (self->ue_object->IsA((UClass *)py_obj->ue_object))
	{
		Py_RETURN_TRUE;
	}


	Py_RETURN_FALSE;
}

PyObject *py_ue_is_child_of(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:is_child_of", &obj))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "object is not a UClass");

	if (!ue_is_pyuobject(obj))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	UClass *parent = (UClass *)py_obj->ue_object;
	UClass *child = (UClass *)self->ue_object;

	if (child->IsChildOf(parent))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_post_edit_change(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

#if WITH_EDITOR
	self->ue_object->PostEditChange();
#endif
	Py_RETURN_NONE;
}

PyObject *py_ue_post_edit_change_property(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	char *prop_name = nullptr;
	int change_type = 0;

	if (!PyArg_ParseTuple(args, "si:post_edit_change_property", &prop_name, &change_type))
	{
		return nullptr;
	}


	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *prop = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(prop_name)));
	if (!prop)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", prop_name);

#if WITH_EDITOR
	FPropertyChangedEvent changed(prop, change_type);
	self->ue_object->PostEditChangeProperty(changed);
#endif
	Py_RETURN_NONE;
}

PyObject *py_ue_modify(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

#if WITH_EDITOR
	self->ue_object->Modify();
#endif
	Py_RETURN_NONE;
}

PyObject *py_ue_pre_edit_change(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	UProperty *prop = nullptr;
	char *prop_name = nullptr;

	if (!PyArg_ParseTuple(args, "|s:pre_edit_change", &prop_name))
	{
		return nullptr;
	}

	if (prop_name)
	{
		UStruct *u_struct = nullptr;

		if (self->ue_object->IsA<UStruct>())
		{
			u_struct = (UStruct *)self->ue_object;
		}
		else
		{
			u_struct = (UStruct *)self->ue_object->GetClass();
		}

		prop = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(prop_name)));
		if (!prop)
			return PyErr_Format(PyExc_Exception, "unable to find property %s", prop_name);
	}

#if WITH_EDITOR
	self->ue_object->PreEditChange(prop);
#endif
	Py_RETURN_NONE;
}


#if WITH_EDITOR
PyObject *py_ue_set_metadata(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *metadata_key;
	char *metadata_value;
	if (!PyArg_ParseTuple(args, "ss:set_metadata", &metadata_key, &metadata_value))
	{
		return NULL;
	}

	if (self->ue_object->IsA<UClass>())
	{
		UClass *u_class = (UClass *)self->ue_object;
		u_class->SetMetaData(FName(UTF8_TO_TCHAR(metadata_key)), UTF8_TO_TCHAR(metadata_value));
	}
	else if (self->ue_object->IsA<UField>())
	{
		UField *u_field = (UField *)self->ue_object;
		u_field->SetMetaData(FName(UTF8_TO_TCHAR(metadata_key)), UTF8_TO_TCHAR(metadata_value));
	}
	else
	{
		return PyErr_Format(PyExc_TypeError, "the object does not support MetaData");
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_set_metadata_on_property(ue_PyUObject * self, PyObject * args)
{
	ue_py_check(self);

	char *metadata_key;
	char *metadata_value;
	char *property_name;

	if (!PyArg_ParseTuple(args, "sss:set_metadata_on_property", &property_name, &metadata_key, &metadata_value))
	{
		return NULL;
	}

	if (self->ue_object->IsA<UClass>())
	{
		UClass *u_class = (UClass *)self->ue_object;
		UProperty * u_property = u_class->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
		u_property->SetMetaData(FName(UTF8_TO_TCHAR(metadata_key)), UTF8_TO_TCHAR(metadata_value));
	}
	else
	{
		return PyErr_Format(PyExc_TypeError, "the object does not support MetaData");
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_get_metadata(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *metadata_key;
	if (!PyArg_ParseTuple(args, "s:get_metadata", &metadata_key))
	{
		return NULL;
	}

	char *metadata_value = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		UClass *u_class = (UClass *)self->ue_object;
		FString value = u_class->GetMetaData(FName(UTF8_TO_TCHAR(metadata_key)));
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value));
	}

	if (self->ue_object->IsA<UField>())
	{
		UField *u_field = (UField *)self->ue_object;
		FString value = u_field->GetMetaData(FName(UTF8_TO_TCHAR(metadata_key)));
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value));
	}

	return PyErr_Format(PyExc_TypeError, "the object does not support MetaData");
}

PyObject *py_ue_has_metadata(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *metadata_key;
	if (!PyArg_ParseTuple(args, "s:has_metadata", &metadata_key))
	{
		return NULL;
	}

	if (self->ue_object->IsA<UClass>())
	{
		UClass *u_class = (UClass *)self->ue_object;
		if (u_class->HasMetaData(FName(UTF8_TO_TCHAR(metadata_key))))
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
		Py_INCREF(Py_False);
		return Py_False;
	}

	if (self->ue_object->IsA<UField>())
	{
		UField *u_field = (UField *)self->ue_object;
		if (u_field->HasMetaData(FName(UTF8_TO_TCHAR(metadata_key))))
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
		Py_INCREF(Py_False);
		return Py_False;
	}

	return PyErr_Format(PyExc_TypeError, "the object does not support MetaData");
}
#endif

PyObject *py_ue_call_function(ue_PyUObject * self, PyObject * args, PyObject *kwargs)
{

	ue_py_check(self);

	UFunction *function = nullptr;

	if (PyTuple_Size(args) < 1)
	{
		return PyErr_Format(PyExc_TypeError, "this function requires at least an argument");
	}

	PyObject *func_id = PyTuple_GetItem(args, 0);

	if (PyUnicodeOrString_Check(func_id))
	{
		function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(PyUnicode_AsUTF8(func_id))));
	}

	if (!function)
		return PyErr_Format(PyExc_Exception, "unable to find function");

	return py_ue_ufunction_call(function, self->ue_object, args, 1, kwargs);

}

PyObject *py_ue_find_function(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_function", &name))
	{
		return NULL;
	}

	UFunction *function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(name)));
	if (!function)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	Py_RETURN_UOBJECT((UObject *)function);
}

#if ENGINE_MINOR_VERSION >= 15
PyObject *py_ue_can_modify(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (self->ue_object->CanModify())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}
#endif

PyObject *py_ue_set_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:set_name", &name))
	{
		return NULL;
	}

	if (!self->ue_object->Rename(UTF8_TO_TCHAR(name), self->ue_object->GetOutermost(), REN_Test))
	{
		return PyErr_Format(PyExc_Exception, "cannot set name %s", name);
	}

	if (self->ue_object->Rename(UTF8_TO_TCHAR(name)))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_set_outer(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_outer;
	if (!PyArg_ParseTuple(args, "O:set_outer", &py_outer))
	{
		return nullptr;
	}

	UPackage *package = ue_py_check_type<UPackage>(py_outer);
	if (!package)
		return PyErr_Format(PyExc_Exception, "argument is not a UPackage");

	if (!self->ue_object->Rename(nullptr, package, REN_Test))
	{
		return PyErr_Format(PyExc_Exception, "cannot move to package %s", TCHAR_TO_UTF8(*package->GetPathName()));
	}

	if (self->ue_object->Rename(nullptr, package))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_get_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);


	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetName())));
}

PyObject *py_ue_get_display_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

#if WITH_EDITOR
	if (UClass *uclass = ue_py_check_type<UClass>(self))
	{
		return PyUnicode_FromString(TCHAR_TO_UTF8(*uclass->GetDisplayNameText().ToString()));
	}

	if (AActor *actor = ue_py_check_type<AActor>(self))
	{
		return PyUnicode_FromString(TCHAR_TO_UTF8(*actor->GetActorLabel()));
	}
#endif

	if (UActorComponent *component = ue_py_check_type<UActorComponent>(self))
	{
		return PyUnicode_FromString(TCHAR_TO_UTF8(*component->GetReadableName()));
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetName())));
}


PyObject *py_ue_get_full_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);


	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetFullName())));
}

PyObject *py_ue_get_path_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetPathName())));
}

PyObject *py_ue_save_config(ue_PyUObject *self, PyObject * args, PyObject *kwargs)
{
	ue_py_check(self);

	uint64 flags = CPF_Config;
	char *file_name = nullptr;

	char *kwlist[] = {
		(char *)"flags",
		(char *)"filename",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|Ks:save_config", (char**)kwlist, &flags, &file_name))
	{
		PyErr_Format(PyExc_Exception, "\nPossible causes of error:\n1. Arguments provided in wrong order\n2. Arguments provided with wrong keywords");
		Py_RETURN_NONE;
	}

	self->ue_object->SaveConfig(flags, UTF8_TO_TCHAR(file_name));

	Py_RETURN_NONE;
}

//TODO: ikrimae: #ThirdParty-Python: Codereview's sai addition
PyObject *py_ue_save_config_to_section(ue_PyUObject *self, PyObject * args, PyObject *kwargs)
{
	ue_py_check(self);

	uint64 flags = CPF_Config;
	char *file_name = nullptr;
	char *section_name = nullptr;

	char *kwlist[] = {
		(char *)"section_name",
		(char *)"flags",
		(char *)"filename",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|Ks:save_config_to_section", (char**)kwlist, &section_name, &flags, &file_name))
	{
		PyErr_Format(PyExc_Exception, "\nPossible causes of error:\n1. Arguments provided in wrong order\n2. Arguments provided with wrong keywords");
		Py_RETURN_NONE;
	}


	if (!self->ue_object->GetClass()->HasAnyClassFlags(CLASS_Config))
	{
		Py_RETURN_NONE;
	}

	TCHAR * InFileName = UTF8_TO_TCHAR(file_name);
	uint32 PropagationFlags = UE4::LCPF_None;

	const FString Filename
		// if a filename was specified, always load from that file
		= InFileName
		? InFileName
		: GetConfigFilename(self->ue_object);

	// Determine whether the file we are writing is a default file config.
	const bool bIsADefaultIniWrite = Filename == self->ue_object->GetDefaultConfigFilename() || Filename == self->ue_object->GetGlobalUserConfigFilename();

	const bool bPerObject = UsesPerObjectConfig(self->ue_object);

	FString Section = UTF8_TO_TCHAR(section_name);

	UObject* CDO = self->ue_object->GetClass()->GetDefaultObject();

	// only copy the values to the CDO if this is GConfig and we're not saving the CDO
	const bool bCopyValues = (self->ue_object != CDO);

	for (UProperty* Property = self->ue_object->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		if (!Property->HasAnyPropertyFlags(CPF_Config))
		{
			continue;
		}

		if ((Property->PropertyFlags & flags) == flags)
		{
			UClass* BaseClass = self->ue_object->GetClass();

			if (Property->PropertyFlags & CPF_GlobalConfig)
			{
				// call LoadConfig() on child classes if any of the properties were global config
				PropagationFlags |= UE4::LCPF_PropagateToChildDefaultObjects;
				BaseClass = Property->GetOwnerClass();
				if (BaseClass != self->ue_object->GetClass())
				{
					// call LoadConfig() on parent classes only if the global config property was declared in a parent class
					PropagationFlags |= UE4::LCPF_ReadParentSections;
				}
			}

			FString Key = Property->GetName();
			int32 PortFlags = 0;

#if WITH_EDITOR
			static FName ConsoleVariableFName(TEXT("ConsoleVariable"));
			const FString& CVarName = Property->GetMetaData(ConsoleVariableFName);
			if (!CVarName.IsEmpty())
			{
				Key = CVarName;
				PortFlags |= PPF_ConsoleVariable;
			}
#endif // #if WITH_EDITOR

			// globalconfig properties should always use the owning class's config file
			// specifying a value for InFilename will override this behavior (as it does with normal properties)
			const FString& PropFileName = ((Property->PropertyFlags & CPF_GlobalConfig) && InFileName == NULL) ? Property->GetOwnerClass()->GetConfigName() : Filename;

			// Properties that are the same as the parent class' defaults should not be saved to ini
			// Before modifying any key in the section, first check to see if it is different from the parent.
			const bool bIsPropertyInherited = Property->GetOwnerClass() != self->ue_object->GetClass();
			const bool bShouldCheckIfIdenticalBeforeAdding = !self->ue_object->GetClass()->HasAnyClassFlags(CLASS_ConfigDoNotCheckDefaults) && !bPerObject && bIsPropertyInherited;
			UObject* SuperClassDefaultObject = self->ue_object->GetClass()->GetSuperClass()->GetDefaultObject();

			UArrayProperty* Array = dynamic_cast<UArrayProperty*>(Property);
			if (Array)
			{
				if (!bShouldCheckIfIdenticalBeforeAdding || !Property->Identical_InContainer(self->ue_object, SuperClassDefaultObject))
				{
					FConfigSection* Sec = GConfig->GetSectionPrivate(*Section, 1, 0, *PropFileName);
					check(Sec);
					Sec->Remove(*Key);

					// Default ini's require the array syntax to be applied to the property name
					FString CompleteKey = FString::Printf(TEXT("%s%s"), bIsADefaultIniWrite ? TEXT("+") : TEXT(""), *Key);

					FScriptArrayHelper_InContainer ArrayHelper(Array, self->ue_object);
					for (int32 i = 0; i < ArrayHelper.Num(); i++)
					{
						FString	Buffer;
						Array->Inner->ExportTextItem(Buffer, ArrayHelper.GetRawPtr(i), ArrayHelper.GetRawPtr(i), self->ue_object, PortFlags);
						Sec->Add(*CompleteKey, *Buffer);
					}
				}
				else if (Property->Identical_InContainer(self->ue_object, SuperClassDefaultObject))
				{
					// If we are not writing it to config above, we should make sure that this property isn't stagnant in the cache.
					FConfigSection* Sec = GConfig->GetSectionPrivate(*Section, 1, 0, *PropFileName);
					if (Sec)
					{
						Sec->Remove(*Key);
					}

				}
			}
			else
			{
				TCHAR TempKey[MAX_SPRINTF] = TEXT("");
				for (int32 Index = 0; Index < Property->ArrayDim; Index++)
				{
					if (Property->ArrayDim != 1)
					{
						FCString::Sprintf(TempKey, TEXT("%s[%i]"), *Property->GetName(), Index);
						Key = TempKey;
					}

					if (!bShouldCheckIfIdenticalBeforeAdding || !Property->Identical_InContainer(self->ue_object, SuperClassDefaultObject, Index))
					{
						FString	Value;
						Property->ExportText_InContainer(Index, Value, self->ue_object, self->ue_object, self->ue_object, PortFlags);
						GConfig->SetString(*Section, *Key, *Value, *PropFileName);
					}
					else if (Property->Identical_InContainer(self->ue_object, SuperClassDefaultObject, Index))
					{
						// If we are not writing it to config above, we should make sure that this property isn't stagnant in the cache.
						FConfigSection* Sec = GConfig->GetSectionPrivate(*Section, 1, 0, *PropFileName);
						if (Sec)
						{
							Sec->Remove(*Key);
						}
					}
				}
			}

			if (bCopyValues)
			{
				void* ThisPropertyAddress = Property->ContainerPtrToValuePtr<void>(self->ue_object);
				void* CDOPropertyAddr = Property->ContainerPtrToValuePtr<void>(CDO);

				Property->CopyCompleteValue(CDOPropertyAddr, ThisPropertyAddress);
			}
		}
	}

	GConfig->Flush(0);
	Py_RETURN_NONE;
}

//TODO: ikrimae: #ThirdParty-Python: Codereview's sai addition
PyObject *py_ue_load_config(ue_PyUObject *self, PyObject * args, PyObject *kwargs)
{
	ue_py_check(self);

	PyObject * py_uclass = nullptr;
	PyObject * py_uproperty = nullptr;
	char *file_name = nullptr;
	int propagation_flags = UE4::ELoadConfigPropagationFlags::LCPF_None;

	char *kwlist[] = {
		(char *)"config_class",
		(char *)"filename",
		(char *)"propagation_flags",
		(char *)"property_to_load",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OsiO:load_config", (char**)kwlist, &py_uclass, &file_name, &propagation_flags, &py_uproperty))
	{
		PyErr_Format(PyExc_Exception, "\nPossible causes of error:\n1. Arguments provided in wrong order\n2. Arguments provided with wrong keywords");
		Py_RETURN_NONE;
	}

	// Validate UClass argument
	ue_PyUObject *py_uclass_obj = nullptr;
	UClass * config_uclass = nullptr;
	if (py_uclass != nullptr)
	{
		if (!ue_is_pyuobject(py_uclass))
		{
			return PyErr_Format(PyExc_Exception, "ConfigClass argument is not a UObject");
		}
		py_uclass_obj = (ue_PyUObject *)py_uclass;

		if (py_uclass_obj->ue_object->IsA<UClass>())
		{
			config_uclass = (UClass *)py_uclass_obj->ue_object;
		}
		else
		{
			PyErr_Format(PyExc_Exception, "ConfigClass Argument is not a UClass");
			Py_RETURN_NONE;
		}
	}

	// Validate UProperty argument
	ue_PyUObject *py_uproperty_obj = nullptr;
	UProperty * uproperty_to_load = nullptr;
	if (py_uproperty != nullptr)
	{
		if (!ue_is_pyuobject(py_uproperty))
		{
			return PyErr_Format(PyExc_Exception, "PropertyToLoad argument is not a UObject");
		}
		py_uproperty_obj = (ue_PyUObject *)py_uproperty;

		if (py_uproperty_obj->ue_object->IsA<UProperty>())
		{
			uproperty_to_load = (UProperty *)py_uproperty_obj->ue_object;
		}
		else
		{
			PyErr_Format(PyExc_Exception, "PropertyToLoad argument is not a UProperty");
			Py_RETURN_NONE;
		}
	}
	
	self->ue_object->LoadConfig(config_uclass, UTF8_TO_TCHAR(file_name), propagation_flags, uproperty_to_load);

	Py_RETURN_NONE;
}

/** Checks if a section specified as a long package name can be found as short name in ini. - Used below */
#if !UE_BUILD_SHIPPING
namespace {
    void CheckMissingSection(const FString& SectionName, const FString& IniFilename)
    {
	    static TSet<FString> MissingSections;
	    FConfigSection* Sec = GConfig->GetSectionPrivate(*SectionName, false, true, *IniFilename);
	    if (!Sec && MissingSections.Contains(SectionName) == false)
	    {
		    FString ShortSectionName = FPackageName::GetShortName(SectionName);
		    if (ShortSectionName != SectionName)
		    {
			    Sec = GConfig->GetSectionPrivate(*ShortSectionName, false, true, *IniFilename);
			    if (Sec != NULL)
			    {
				    UE_LOG(LogConfig, Fatal, TEXT("Short class section names (%s) are not supported, please use long name: %s"), *ShortSectionName, *SectionName);
			    }
		    }
		    MissingSections.Add(SectionName);
	    }
    }
}
#endif

//TODO: ikrimae: #ThirdParty-Python: Codereview's sai addition
PyObject *py_ue_load_config_from_section(ue_PyUObject *self, PyObject * args, PyObject *kwargs)
{
	ue_py_check(self);

	char *section_name = nullptr;
	PyObject * py_uclass = nullptr;
	PyObject * py_uproperty = nullptr;
	char *file_name = nullptr;
	int propagation_flags = UE4::ELoadConfigPropagationFlags::LCPF_None;

	char *kwlist[] = {
		(char *)"section_name",
		(char *)"config_class",
		(char *)"filename",
		(char *)"propagation_flags",
		(char *)"property_to_load",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|OsiO:load_config_from_section", (char**)kwlist, &section_name, &py_uclass, &file_name, &propagation_flags, &py_uproperty))
	{
		PyErr_Format(PyExc_Exception, "\nPossible causes of error:\n1. Arguments provided in wrong order\n2. Arguments provided with wrong keywords");
		Py_RETURN_NONE;
	}

	// Validate UClass argument
	ue_PyUObject *py_uclass_obj = nullptr;
	UClass * config_uclass = nullptr;
	if (py_uclass != nullptr)
	{
		if (!ue_is_pyuobject(py_uclass))
		{
			return PyErr_Format(PyExc_Exception, "ConfigClass argument is not a UObject");
		}
		py_uclass_obj = (ue_PyUObject *)py_uclass;

		if (py_uclass_obj->ue_object->IsA<UClass>())
		{
			config_uclass = (UClass *)py_uclass_obj->ue_object;
		}
		else
		{
			PyErr_Format(PyExc_Exception, "ConfigClass Argument is not a UClass");
			Py_RETURN_NONE;
		}
	}

	// Validate UProperty argument
	ue_PyUObject *py_uproperty_obj = nullptr;
	UProperty * uproperty_to_load = nullptr;
	if (py_uproperty != nullptr)
	{
		if (!ue_is_pyuobject(py_uproperty))
		{
			return PyErr_Format(PyExc_Exception, "PropertyToLoad argument is not a UObject");
		}
		py_uproperty_obj = (ue_PyUObject *)py_uproperty;

		if (py_uproperty_obj->ue_object->IsA<UProperty>())
		{
			uproperty_to_load = (UProperty *)py_uproperty_obj->ue_object;
		}
		else
		{
			PyErr_Format(PyExc_Exception, "PropertyToLoad argument is not a UProperty");
			Py_RETURN_NONE;
		}
	}

	// OriginalClass is the class that LoadConfig() was originally called on
	static UClass* OriginalClass = NULL;

	if (!config_uclass)
	{
		// if no class was specified in the call, this is the OriginalClass
		config_uclass = self->ue_object->GetClass();
		OriginalClass = config_uclass;
	}

	if (!config_uclass->HasAnyClassFlags(CLASS_Config))
	{
		Py_RETURN_NONE;
	}

	UClass* ParentClass = config_uclass->GetSuperClass();
	if (ParentClass != NULL)
	{
		if (ParentClass->HasAnyClassFlags(CLASS_Config))
		{
			if ((propagation_flags&UE4::LCPF_ReadParentSections) != 0)
			{
				// call LoadConfig on the parent class
				self->ue_object->LoadConfig(ParentClass, NULL, propagation_flags, uproperty_to_load);

				// if we are also notifying child classes or instances, stop here as this object's properties will be imported as a result of notifying the others
				if ((propagation_flags & (UE4::LCPF_PropagateToChildDefaultObjects | UE4::LCPF_PropagateToInstances)) != 0)
				{
					Py_RETURN_NONE;
				}
			}
			else if ((propagation_flags&UE4::LCPF_PropagateToChildDefaultObjects) != 0)
			{
				// not propagating the call upwards, but we are propagating the call to all child classes
				for (TObjectIterator<UClass> It; It; ++It)
				{
					if (It->IsChildOf(config_uclass))
					{
						// mask out the PropgateToParent and PropagateToChildren values
						It->GetDefaultObject()->LoadConfig(*It, NULL, (propagation_flags&(UE4::LCPF_PersistentFlags | UE4::LCPF_PropagateToInstances)), uproperty_to_load);
					}
				}

				// LoadConfig() was called on this object during iteration, so stop here 
				Py_RETURN_NONE;
			}
			else if ((propagation_flags&UE4::LCPF_PropagateToInstances) != 0)
			{
				// call LoadConfig() on all instances of this class (except the CDO)
				// Do not propagate this call to parents, and do not propagate to children or instances (would be redundant) 
				for (TObjectIterator<UObject> It; It; ++It)
				{
					if (It->IsA(config_uclass))
					{
						if (!GIsEditor)
						{
							// make sure to pass in the class so that OriginalClass isn't reset
							It->LoadConfig(It->GetClass(), NULL, (propagation_flags&UE4::LCPF_PersistentFlags), uproperty_to_load);
						}
#if WITH_EDITOR
						else
						{
							It->PreEditChange(NULL);

							// make sure to pass in the class so that OriginalClass isn't reset
							It->LoadConfig(It->GetClass(), NULL, (propagation_flags&UE4::LCPF_PersistentFlags), uproperty_to_load);

							It->PostEditChange();
						}
#endif // WITH_EDITOR
					}
				}
			}
		}
		else if ((propagation_flags&UE4::LCPF_PropagateToChildDefaultObjects) != 0)
		{
			// we're at the base-most config class
			for (TObjectIterator<UClass> It; It; ++It)
			{
				if (It->IsChildOf(config_uclass))
				{
					if (!GIsEditor)
					{
						// make sure to pass in the class so that OriginalClass isn't reset
						It->GetDefaultObject()->LoadConfig(*It, NULL, (propagation_flags&(UE4::LCPF_PersistentFlags | UE4::LCPF_PropagateToInstances)), uproperty_to_load);
					}
#if WITH_EDITOR
					else
					{
						It->PreEditChange(NULL);

						// make sure to pass in the class so that OriginalClass isn't reset
						It->GetDefaultObject()->LoadConfig(*It, NULL, (propagation_flags&(UE4::LCPF_PersistentFlags | UE4::LCPF_PropagateToInstances)), uproperty_to_load);

						It->PostEditChange();
					}
#endif // WITH_EDITOR
				}
			}

			Py_RETURN_NONE;
		}
		else if ((propagation_flags&UE4::LCPF_PropagateToInstances) != 0)
		{
			for (TObjectIterator<UObject> It; It; ++It)
			{
				if (It->GetClass() == config_uclass)
				{
					if (!GIsEditor)
					{
						// make sure to pass in the class so that OriginalClass isn't reset
						It->LoadConfig(It->GetClass(), NULL, (propagation_flags&UE4::LCPF_PersistentFlags), uproperty_to_load);
					}
#if WITH_EDITOR
					else
					{
						It->PreEditChange(NULL);

						// make sure to pass in the class so that OriginalClass isn't reset
						It->LoadConfig(It->GetClass(), NULL, (propagation_flags&UE4::LCPF_PersistentFlags), uproperty_to_load);
						It->PostEditChange();
					}
#endif // WITH_EDITOR
				}
			}
		}
	}

	TCHAR *  InFilename = UTF8_TO_TCHAR(file_name);
	const FString Filename
		// if a filename was specified, always load from that file
		= InFilename
		? InFilename
		: GetConfigFilename(self->ue_object);

	const bool bPerObject = UsesPerObjectConfig(self->ue_object);

	// does the class want to override the platform hierarchy (ignored if we passd in a specific ini file),
	// and if the name isn't the current running platform (no need to load extra files if already in GConfig)
	bool bUseConfigOverride = InFilename == nullptr && self->ue_object->GetConfigOverridePlatform() != nullptr &&
		FCString::Stricmp(self->ue_object->GetConfigOverridePlatform(), ANSI_TO_TCHAR(FPlatformProperties::IniPlatformName())) != 0;
	FConfigFile OverrideConfig;
	if (bUseConfigOverride)
	{
		// load into a local ini file
		FConfigCacheIni::LoadLocalIniFile(OverrideConfig, *self->ue_object->GetClass()->ClassConfigName.ToString(), true, self->ue_object->GetConfigOverridePlatform());
	}


	FString ClassSection;
	ClassSection = UTF8_TO_TCHAR(section_name);

	// If any of my properties are class variables, then LoadConfig() would also be called for each one of those classes.
	// Since OrigClass is a static variable, if the value of a class variable is a class different from the current class, 
	// we'll lose our nice reference to the original class - and cause any variables which were declared after this class variable to fail 
	// the 'if (OriginalClass != Class)' check....better store it in a temporary place while we do the actual loading of our properties 
	UClass* MyOrigClass = OriginalClass;

	if (uproperty_to_load == NULL)
	{
		UE_LOG(LogConfig, Verbose, TEXT("(%s) '%s' loading configuration from %s"), *config_uclass->GetName(), *self->ue_object->GetName(), *Filename);
	}
	else
	{
		UE_LOG(LogConfig, Verbose, TEXT("(%s) '%s' loading configuration for property %s from %s"), *config_uclass->GetName(), *self->ue_object->GetName(), *uproperty_to_load->GetName(), *Filename);
	}

	for (UProperty* Property = config_uclass->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		if (!Property->HasAnyPropertyFlags(CPF_Config))
		{
			continue;
		}

		// if we're only supposed to load the value for a specific property, skip all others
		if (uproperty_to_load != NULL && uproperty_to_load != Property)
		{
			continue;
		}

		// Don't load config properties that are marked editoronly if not in the editor
		if ((Property->PropertyFlags & CPF_EditorOnly) && !GIsEditor)
		{
			continue;
		}

		const bool bGlobalConfig = (Property->PropertyFlags&CPF_GlobalConfig) != 0;
		UClass* OwnerClass = Property->GetOwnerClass();

		UClass* BaseClass = bGlobalConfig ? OwnerClass : config_uclass;

		// globalconfig properties should always use the owning class's config file
		// specifying a value for InFilename will override this behavior (as it does with normal properties)
		const FString& PropFileName = (bGlobalConfig && InFilename == NULL) ? OwnerClass->GetConfigName() : Filename;

		FString Key = Property->GetName();
		int32 PortFlags = 0;

#if WITH_EDITOR
		static FName ConsoleVariableFName(TEXT("ConsoleVariable"));
		const FString& CVarName = Property->GetMetaData(ConsoleVariableFName);
		if (!CVarName.IsEmpty())
		{
			Key = CVarName;
			PortFlags |= PPF_ConsoleVariable;
		}
#endif // #if WITH_EDITOR

		UE_LOG(LogConfig, Verbose, TEXT("   Loading value for %s from [%s]"), *Key, *ClassSection);
		UArrayProperty* Array = dynamic_cast<UArrayProperty*>(Property);
		if (Array == NULL)
		{
			for (int32 i = 0; i < Property->ArrayDim; i++)
			{
				if (Property->ArrayDim != 1)
				{
					Key = FString::Printf(TEXT("%s[%i]"), *Property->GetName(), i);
				}

				FString Value;
				bool bFoundValue;
				if (bUseConfigOverride)
				{
					bFoundValue = OverrideConfig.GetString(*ClassSection, *Key, Value);
				}
				else
				{
					bFoundValue = GConfig->GetString(*ClassSection, *Key, Value, *PropFileName);
				}

				if (bFoundValue)
				{
					if (Property->ImportText(*Value, Property->ContainerPtrToValuePtr<uint8>(self->ue_object, i), PortFlags, self->ue_object) == NULL)
					{
						// this should be an error as the properties from the .ini / .int file are not correctly being read in and probably are affecting things in subtle ways
						UE_LOG(LogConfig, Error, TEXT("LoadConfig (%s): import failed for %s in: %s"), *self->ue_object->GetPathName(), *Property->GetName(), *Value);
					}
				}

#if !UE_BUILD_SHIPPING
				if (!bFoundValue && !FPlatformProperties::RequiresCookedData())
				{
					CheckMissingSection(ClassSection, PropFileName);
				}
#endif
			}
		}
		else
		{
			FConfigSection* Sec;
			if (bUseConfigOverride)
			{
				Sec = OverrideConfig.Find(*ClassSection);
			}
			else
			{
				Sec = GConfig->GetSectionPrivate(*ClassSection, false, true, *PropFileName);
			}

			FConfigSection* AltSec = NULL;
			//@Package name transition
			if (Sec)
			{
				TArray<FConfigValue> List;
				const FName KeyName(*Key, FNAME_Find);
				Sec->MultiFind(KeyName, List);

				// If we didn't find anything in the first section, try the alternate
				if ((List.Num() == 0) && AltSec)
				{
					AltSec->MultiFind(KeyName, List);
				}

				FScriptArrayHelper_InContainer ArrayHelper(Array, self->ue_object);
				const int32 Size = Array->Inner->ElementSize;
				// Only override default properties if there is something to override them with.
				if (List.Num() > 0)
				{
					ArrayHelper.EmptyAndAddValues(List.Num());
					for (int32 i = List.Num() - 1, c = 0; i >= 0; i--, c++)
					{
						Array->Inner->ImportText(*List[i].GetValue(), ArrayHelper.GetRawPtr(c), PortFlags, self->ue_object);
					}
				}
				else
				{
					int32 Index = 0;
					const FConfigValue* ElementValue = nullptr;
					do
					{
						// Add array index number to end of key
						FString IndexedKey = FString::Printf(TEXT("%s[%i]"), *Key, Index);

						// Try to find value of key
						const FName IndexedName(*IndexedKey, FNAME_Find);
						if (IndexedName == NAME_None)
						{
							break;
						}
						ElementValue = Sec->Find(IndexedName);

						// If found, import the element
						if (ElementValue != nullptr)
						{
							// expand the array if necessary so that Index is a valid element
							ArrayHelper.ExpandForIndex(Index);
							Array->Inner->ImportText(*ElementValue->GetValue(), ArrayHelper.GetRawPtr(Index), PortFlags, self->ue_object);
						}

						Index++;
					} while (ElementValue || Index < ArrayHelper.Num());
				}
			}
#if !UE_BUILD_SHIPPING
			else if (!FPlatformProperties::RequiresCookedData())
			{
				CheckMissingSection(ClassSection, PropFileName);
			}
#endif
		}
	}

	// if we are reloading config data after the initial class load, fire the callback now
	if ((propagation_flags&UE4::LCPF_ReloadingConfigData) != 0)
	{
		self->ue_object->PostReloadConfig(uproperty_to_load);
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_set_property(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	PyObject *property_value;
	int index = 0;
	if (!PyArg_ParseTuple(args, "sO|i:set_property", &property_name, &property_value, &index))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);


	if (!ue_py_convert_pyobject(property_value, u_property, (uint8 *)self->ue_object, index))
	{
		return PyErr_Format(PyExc_Exception, "unable to set property %s", property_name);
	}

	Py_RETURN_NONE;

}

PyObject *py_ue_set_property_flags(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	uint64 flags;
	if (!PyArg_ParseTuple(args, "sK:set_property_flags", &property_name, &flags))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);


	u_property->SetPropertyFlags(flags);
	Py_RETURN_NONE;
}

PyObject *py_ue_add_property_flags(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	uint64 flags;
	if (!PyArg_ParseTuple(args, "sK:add_property_flags", &property_name, &flags))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);


	u_property->SetPropertyFlags(u_property->GetPropertyFlags() | flags);
	Py_RETURN_NONE;
}

PyObject *py_ue_get_property_flags(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:get_property_flags", &property_name))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	return PyLong_FromUnsignedLong(u_property->GetPropertyFlags());
}

PyObject *py_ue_enum_values(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (!self->ue_object->IsA<UEnum>())
		return PyErr_Format(PyExc_TypeError, "uobject is not a UEnum");

	UEnum *u_enum = (UEnum *)self->ue_object;
	uint8 max_enum_value = u_enum->GetMaxEnumValue();
	PyObject *ret = PyList_New(0);
	for (uint8 i = 0; i < max_enum_value; i++)
	{
		PyObject *py_long = PyLong_FromLong(i);
		PyList_Append(ret, py_long);
		Py_DECREF(py_long);
	}
	return ret;
}

PyObject *py_ue_enum_names(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (!self->ue_object->IsA<UEnum>())
		return PyErr_Format(PyExc_TypeError, "uobject is not a UEnum");

	UEnum *u_enum = (UEnum *)self->ue_object;
	uint8 max_enum_value = u_enum->GetMaxEnumValue();
	PyObject *ret = PyList_New(0);
	for (uint8 i = 0; i < max_enum_value; i++)
	{
#if ENGINE_MINOR_VERSION > 15
		PyObject *py_long = PyUnicode_FromString(TCHAR_TO_UTF8(*u_enum->GetNameStringByIndex(i)));
#else
		PyObject *py_long = PyUnicode_FromString(TCHAR_TO_UTF8(*u_enum->GetEnumName(i)));
#endif
		PyList_Append(ret, py_long);
		Py_DECREF(py_long);
	}
	return ret;
}

#if ENGINE_MINOR_VERSION >= 15
PyObject *py_ue_enum_user_defined_names(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (!self->ue_object->IsA<UUserDefinedEnum>())
		return PyErr_Format(PyExc_TypeError, "uobject is not a UEnum");

	UUserDefinedEnum *u_enum = (UUserDefinedEnum *)self->ue_object;
	TArray<FText> user_defined_names;
	u_enum->DisplayNameMap.GenerateValueArray(user_defined_names);
	PyObject *ret = PyList_New(0);
	for (FText text : user_defined_names)
	{
		PyObject *py_long = PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
		PyList_Append(ret, py_long);
		Py_DECREF(py_long);
	}
	return ret;
}
#endif

PyObject *py_ue_properties(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	PyObject *ret = PyList_New(0);

	for (TFieldIterator<UProperty> PropIt(u_struct); PropIt; ++PropIt)
	{
		UProperty* property = *PropIt;
		PyObject *property_name = PyUnicode_FromString(TCHAR_TO_UTF8(*property->GetName()));
		PyList_Append(ret, property_name);
		Py_DECREF(property_name);
	}

	return ret;
}

PyObject *py_ue_functions(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	PyObject *ret = PyList_New(0);

	for (TFieldIterator<UFunction> FuncIt(u_struct); FuncIt; ++FuncIt)
	{
		UFunction* func = *FuncIt;
		PyObject *func_name = PyUnicode_FromString(TCHAR_TO_UTF8(*func->GetFName().ToString()));
		PyList_Append(ret, func_name);
		Py_DECREF(func_name);
	}

	return ret;

}

PyObject *py_ue_call(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *call_args;
	if (!PyArg_ParseTuple(args, "s:call", &call_args))
	{
		return NULL;
	}

	FOutputDeviceNull od_null;
	if (!self->ue_object->CallFunctionByNameWithArguments(UTF8_TO_TCHAR(call_args), od_null, NULL, true))
	{
		return PyErr_Format(PyExc_Exception, "error while calling \"%s\"", call_args);
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_broadcast(ue_PyUObject *self, PyObject *args)
{

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:broadcast", &property_name))
	{
		return NULL;
	}

	UProperty *u_property = self->ue_object->GetClass()->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find event property %s", property_name);

	if (auto casted_prop = Cast<UMulticastDelegateProperty>(u_property))
	{
		FMulticastScriptDelegate* multiscript_delegate = casted_prop->GetPropertyValuePtr_InContainer(self->ue_object);
		uint8 *parms = (uint8 *)FMemory_Alloca(casted_prop->SignatureFunction->PropertiesSize);
		FMemory::Memzero(parms, casted_prop->SignatureFunction->PropertiesSize);
		multiscript_delegate->ProcessMulticastDelegate<UObject>(parms);
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "property is not a UMulticastDelegateProperty");
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_get_property(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	int index = 0;
	if (!PyArg_ParseTuple(args, "s|i:get_property", &property_name, &index))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	return ue_py_convert_property(u_property, (uint8 *)self->ue_object, index);
}

PyObject *py_ue_get_property_array_dim(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:get_property_array_dim", &property_name))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	return PyLong_FromLongLong(u_property->ArrayDim);
}

#if WITH_EDITOR
PyObject *py_ue_get_thumbnail(ue_PyUObject *self, PyObject * args)
{

	PyObject *py_generate = nullptr;
	if (!PyArg_ParseTuple(args, "|O:get_thumbnail", &py_generate))
	{
		return nullptr;
	}

	ue_py_check(self);

	TArray<FName> names;
	names.Add(FName(*self->ue_object->GetFullName()));

	FThumbnailMap thumb_map;

	FObjectThumbnail *object_thumbnail = nullptr;

	if (!ThumbnailTools::ConditionallyLoadThumbnailsForObjects(names, thumb_map))
	{
		if (py_generate && PyObject_IsTrue(py_generate))
		{
			object_thumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(self->ue_object);
		}
	}
	else
	{
		object_thumbnail = &thumb_map[names[0]];
	}

	if (!object_thumbnail)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve thumbnail");
	}

	return py_ue_new_fobject_thumbnail(*object_thumbnail);
}

PyObject *py_ue_render_thumbnail(ue_PyUObject *self, PyObject * args)
{

	int width = ThumbnailTools::DefaultThumbnailSize;
	int height = ThumbnailTools::DefaultThumbnailSize;
	PyObject *no_flush = nullptr;
	if (!PyArg_ParseTuple(args, "|iiO:render_thumbnail", &width, height, &no_flush))
	{
		return nullptr;
	}

	ue_py_check(self);
	FObjectThumbnail object_thumbnail;
	ThumbnailTools::RenderThumbnail(self->ue_object, width, height,
		(no_flush && PyObject_IsTrue(no_flush)) ? ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush : ThumbnailTools::EThumbnailTextureFlushMode::AlwaysFlush,
		nullptr, &object_thumbnail);

	return py_ue_new_fobject_thumbnail(object_thumbnail);
}
#endif

PyObject *py_ue_get_uproperty(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:get_uproperty", &property_name))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	Py_RETURN_UOBJECT(u_property);

}

PyObject *py_ue_has_property(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:has_property", &property_name))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		Py_RETURN_FALSE;
	Py_RETURN_TRUE;
}

PyObject *py_ue_get_property_class(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:get_property_class", &property_name))
	{
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UClass>())
	{
		u_struct = (UStruct *)self->ue_object;
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	Py_RETURN_UOBJECT(u_property->GetClass());

}

PyObject *py_ue_is_rooted(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (self->ue_object->IsRooted())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_is_selected(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	if (self->ue_object->IsSelected())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_add_to_root(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	self->ue_object->AddToRoot();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_auto_root(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	self->ue_object->AddToRoot();
	self->auto_rooted = 1;

	Py_RETURN_NONE;
}

PyObject *py_ue_remove_from_root(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	self->ue_object->RemoveFromRoot();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_clear_event(ue_PyUObject * self, PyObject * args)
{
	ue_py_check(self);

	char *event_name;
	if (!PyArg_ParseTuple(args, "s:clear_events", &event_name))
	{
		return NULL;
	}

	UProperty *u_property = self->ue_object->GetClass()->FindPropertyByName(FName(*FString(event_name)));
	if (!u_property)
	{
		return PyErr_Format(PyExc_Exception, "unable to find event property %s", TCHAR_TO_UTF8(*FString(event_name)));
	}

	if (UMulticastDelegateProperty* casted_prop = Cast<UMulticastDelegateProperty>(u_property))
	{
		FMulticastScriptDelegate* multiscript_delegate = casted_prop->GetPropertyValuePtr_InContainer(self->ue_object);
		multiscript_delegate->Clear();
        Py_RETURN_NONE;
	}

	return PyErr_Format(PyExc_Exception, "property %s is not an event", TCHAR_TO_UTF8(*FString(event_name)));
}

PyObject *py_ue_bind_event(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *event_name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:bind_event", &event_name, &py_callable))
	{
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "object is not callable");
	}

	return ue_bind_pyevent(self, FString(event_name), py_callable, true);
}

#if PY_MAJOR_VERSION >= 3
PyObject *py_ue_add_function(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:add_function", &name, &py_callable))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UClass>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
	}

	UClass *u_class = (UClass *)self->ue_object;

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "object is not callable");
	}

	if (!unreal_engine_add_function(u_class, name, py_callable, FUNC_Native | FUNC_BlueprintCallable | FUNC_Public))
	{
		return PyErr_Format(PyExc_Exception, "unable to add function");
	}

	Py_INCREF(Py_None);
	return Py_None;
}
#endif

PyObject *py_ue_add_property(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *obj;
	char *name;
	PyObject *property_class = nullptr;
	PyObject *property_class2 = nullptr;
	if (!PyArg_ParseTuple(args, "Os|OO:add_property", &obj, &name, &property_class, &property_class2))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UStruct>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UStruct");

	UObject *scope = nullptr;

	UProperty *u_property = nullptr;
	UClass *u_class = nullptr;
	UProperty *u_property2 = nullptr;
	UClass *u_class2 = nullptr;

	UClass *u_prop_class = nullptr;
	UScriptStruct *u_script_struct = nullptr;
	UEnum * u_enum_class = nullptr;

	UClass *u_prop_class2 = nullptr;
	UScriptStruct *u_script_struct2 = nullptr;
	bool is_array = false;
	bool is_map = false;

	if (property_class)
	{
		if (!ue_is_pyuobject(property_class))
		{
			return PyErr_Format(PyExc_Exception, "property class arg is not a uobject");
		}
		ue_PyUObject *py_prop_class = (ue_PyUObject *)property_class;
		if (py_prop_class->ue_object->IsA<UClass>())
		{
			u_prop_class = (UClass *)py_prop_class->ue_object;
		}
		else if (py_prop_class->ue_object->IsA<UScriptStruct>())
		{
			u_script_struct = (UScriptStruct *)py_prop_class->ue_object;
		}
		else if (py_prop_class->ue_object->IsA<UEnum>())
		{
			u_enum_class = (UEnum *)py_prop_class->ue_object;
		}
		else
		{
			return PyErr_Format(PyExc_Exception, "property class arg is not a UClass or a UScriptStruct or a UEnum");
		}
	}

	if (property_class2)
	{
		if (!ue_is_pyuobject(property_class2))
		{
			return PyErr_Format(PyExc_Exception, "property class arg is not a uobject");
		}
		ue_PyUObject *py_prop_class = (ue_PyUObject *)property_class2;
		if (py_prop_class->ue_object->IsA<UClass>())
		{
			u_prop_class2 = (UClass *)py_prop_class->ue_object;
		}
		else if (py_prop_class->ue_object->IsA<UScriptStruct>())
		{
			u_script_struct2 = (UScriptStruct *)py_prop_class->ue_object;
		}
		else
		{
			return PyErr_Format(PyExc_Exception, "property class arg is not a UClass or a UScriptStruct");
		}
	}

	EObjectFlags o_flags = RF_Public | RF_MarkAsNative;// | RF_Transient;

	if (ue_is_pyuobject(obj))
	{
		ue_PyUObject *py_obj = (ue_PyUObject *)obj;
	
		if (py_obj->ue_object->IsA<UEnum>())
		{
			scope = self->ue_object;
			u_class = UEnumProperty::StaticClass();
		}
		else
		{
			if (!py_obj->ue_object->IsA<UClass>())
			{
				return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
			}
			u_class = (UClass *)py_obj->ue_object;
			if (!u_class->IsChildOf<UProperty>())
				return PyErr_Format(PyExc_Exception, "uobject is not a UProperty");
			if (u_class == UArrayProperty::StaticClass())
				return PyErr_Format(PyExc_Exception, "please use a single-item list of property for arrays");
			scope = self->ue_object;
		}
	}
	else if (PyList_Check(obj))
	{
		if (PyList_Size(obj) == 1)
		{
			PyObject *py_item = PyList_GetItem(obj, 0);
			if (ue_is_pyuobject(py_item))
			{
				ue_PyUObject *py_obj = (ue_PyUObject *)py_item;
				if (!py_obj->ue_object->IsA<UClass>())
				{
					return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
				}
				u_class = (UClass *)py_obj->ue_object;
				if (!u_class->IsChildOf<UProperty>())
					return PyErr_Format(PyExc_Exception, "uobject is not a UProperty");
				if (u_class == UArrayProperty::StaticClass())
					return PyErr_Format(PyExc_Exception, "please use a single-item list of property for arrays");
				UArrayProperty *u_array = NewObject<UArrayProperty>(self->ue_object, UTF8_TO_TCHAR(name), o_flags);
				if (!u_array)
					return PyErr_Format(PyExc_Exception, "unable to allocate new UProperty");
				scope = u_array;
				is_array = true;
			}
			Py_DECREF(py_item);
		}
#if ENGINE_MINOR_VERSION >= 15
		else if (PyList_Size(obj) == 2)
		{
			PyObject *py_key = PyList_GetItem(obj, 0);
			PyObject *py_value = PyList_GetItem(obj, 1);
			if (ue_is_pyuobject(py_key) && ue_is_pyuobject(py_value))
			{
				// KEY
				ue_PyUObject *py_obj = (ue_PyUObject *)py_key;
				if (!py_obj->ue_object->IsA<UClass>())
				{
					return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
				}
				u_class = (UClass *)py_obj->ue_object;
				if (!u_class->IsChildOf<UProperty>())
					return PyErr_Format(PyExc_Exception, "uobject is not a UProperty");
				if (u_class == UArrayProperty::StaticClass())
					return PyErr_Format(PyExc_Exception, "please use a two-items list of properties for maps");

				// VALUE
				ue_PyUObject *py_obj2 = (ue_PyUObject *)py_value;
				if (!py_obj2->ue_object->IsA<UClass>())
				{
					return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
				}
				u_class2 = (UClass *)py_obj2->ue_object;
				if (!u_class2->IsChildOf<UProperty>())
					return PyErr_Format(PyExc_Exception, "uobject is not a UProperty");
				if (u_class2 == UArrayProperty::StaticClass())
					return PyErr_Format(PyExc_Exception, "please use a two-items list of properties for maps");


				UMapProperty *u_map = NewObject<UMapProperty>(self->ue_object, UTF8_TO_TCHAR(name), o_flags);
				if (!u_map)
					return PyErr_Format(PyExc_Exception, "unable to allocate new UProperty");
				scope = u_map;
				is_map = true;
			}
			Py_DECREF(py_key);
			Py_DECREF(py_value);
		}
#endif
	}


	if (!scope)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject or a single item list");
	}

	u_property = NewObject<UProperty>(scope, u_class, UTF8_TO_TCHAR(name), o_flags);
	if (!u_property)
	{
		if (is_array || is_map)
			scope->MarkPendingKill();
		return PyErr_Format(PyExc_Exception, "unable to allocate new UProperty");
	}

	// one day we may want to support transient properties...
	//uint64 flags = CPF_Edit | CPF_BlueprintVisible | CPF_Transient | CPF_ZeroConstructor;
	uint64 flags = CPF_Edit | CPF_BlueprintVisible | CPF_ZeroConstructor;

	// we assumed Actor Components to be non-editable
	if (u_prop_class && u_prop_class->IsChildOf<UActorComponent>())
	{
		flags |= ~CPF_Edit;
	}

	// TODO manage replication
	/*
	if (replicate && PyObject_IsTrue(replicate)) {
		flags |= CPF_Net;
	}*/

	if (is_array)
	{
		UArrayProperty *u_array = (UArrayProperty *)scope;
		u_array->AddCppProperty(u_property);
		u_property->SetPropertyFlags(flags);
		if (u_property->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty *obj_prop = (UObjectProperty *)u_property;
			if (u_prop_class)
			{
				obj_prop->SetPropertyClass(u_prop_class);
			}
		}
		if (u_property->GetClass() == UStructProperty::StaticClass())
		{
			UStructProperty *obj_prop = (UStructProperty *)u_property;
			if (u_script_struct)
			{
				obj_prop->Struct = u_script_struct;
			}
		}
		u_property = u_array;
	}

#if ENGINE_MINOR_VERSION >= 15
	if (is_map)
	{
		u_property2 = NewObject<UProperty>(scope, u_class2, NAME_None, o_flags);
		if (!u_property2)
		{
			if (is_array || is_map)
				scope->MarkPendingKill();
			return PyErr_Format(PyExc_Exception, "unable to allocate new UProperty");
		}
		UMapProperty *u_map = (UMapProperty *)scope;


		u_property->SetPropertyFlags(flags);
		u_property2->SetPropertyFlags(flags);

		if (u_property->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty *obj_prop = (UObjectProperty *)u_property;
			if (u_prop_class)
			{
				obj_prop->SetPropertyClass(u_prop_class);
			}
		}
		if (u_property->GetClass() == UStructProperty::StaticClass())
		{
			UStructProperty *obj_prop = (UStructProperty *)u_property;
			if (u_script_struct)
			{
				obj_prop->Struct = u_script_struct;
			}
		}

		if (u_property2->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty *obj_prop = (UObjectProperty *)u_property2;
			if (u_prop_class2)
			{
				obj_prop->SetPropertyClass(u_prop_class2);
			}
		}
		if (u_property2->GetClass() == UStructProperty::StaticClass())
		{
			UStructProperty *obj_prop = (UStructProperty *)u_property2;
			if (u_script_struct2)
			{
				obj_prop->Struct = u_script_struct2;
			}
		}

		u_map->KeyProp = u_property;
		u_map->ValueProp = u_property2;

		u_property = u_map;
	}
#endif

	if (u_class == UMulticastDelegateProperty::StaticClass())
	{
		UMulticastDelegateProperty *mcp = (UMulticastDelegateProperty *)u_property;
		mcp->SignatureFunction = NewObject<UFunction>(self->ue_object, NAME_None, RF_Public | RF_Transient | RF_MarkAsNative);
		mcp->SignatureFunction->FunctionFlags = FUNC_MulticastDelegate | FUNC_BlueprintCallable | FUNC_Native;
		flags |= CPF_BlueprintAssignable | CPF_BlueprintCallable;
		flags &= ~CPF_Edit;
	}

	else if (u_class == UDelegateProperty::StaticClass())
	{
		UDelegateProperty *udp = (UDelegateProperty *)u_property;
		udp->SignatureFunction = NewObject<UFunction>(self->ue_object, NAME_None, RF_Public | RF_Transient | RF_MarkAsNative);
		udp->SignatureFunction->FunctionFlags = FUNC_MulticastDelegate | FUNC_BlueprintCallable | FUNC_Native;
		flags |= CPF_BlueprintAssignable | CPF_BlueprintCallable;
		flags &= ~CPF_Edit;
	}

	else if (u_class == UObjectProperty::StaticClass())
	{
		// ensure it is not an arry as we have already managed it !
		if (!is_array && !is_map)
		{
			UObjectProperty *obj_prop = (UObjectProperty *)u_property;
			if (u_prop_class)
			{
				obj_prop->SetPropertyClass(u_prop_class);
			}
		}
	}

	else if (u_class == UStructProperty::StaticClass())
	{
		// ensure it is not an arry as we have already managed it !
		if (!is_array && !is_map)
		{
			UStructProperty *obj_prop = (UStructProperty *)u_property;
			if (u_script_struct)
			{
				obj_prop->Struct = u_script_struct;
			}
		}
	}

	else if (u_class == UEnumProperty::StaticClass())
	{
		// ensure it is not an arry as we have already managed it !
		if (!is_array && !is_map)
		{
			UEnumProperty *enum_prop = (UEnumProperty *)u_property;
			if(enum_prop)
			{
				//UEnum* EnumType = CastChecked<UEnum>(u_enum_class);
				enum_prop->SetEnum(u_enum_class);
				enum_prop->AddCppProperty(NewObject<UByteProperty>(enum_prop, TEXT("UnderlyingType")));

			}
		}
	}

	u_property->SetPropertyFlags(flags);
	u_property->ArrayDim = 1;

	UStruct *u_struct = (UStruct *)self->ue_object;
	u_struct->AddCppProperty(u_property);
	u_struct->StaticLink(true);


	if (u_struct->IsA<UClass>())
	{
		UClass *owner_class = (UClass *)u_struct;
		owner_class->GetDefaultObject()->RemoveFromRoot();
		owner_class->GetDefaultObject()->ConditionalBeginDestroy();
		owner_class->ClassDefaultObject = nullptr;
		owner_class->GetDefaultObject()->PostInitProperties();
	}

	// TODO add default value

	Py_RETURN_UOBJECT(u_property);
}

PyObject *py_ue_as_dict(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UStruct *u_struct = nullptr;
	UObject *u_object = self->ue_object;

	if (self->ue_object->IsA<UStruct>())
	{
		u_struct = (UStruct *)self->ue_object;
		if (self->ue_object->IsA<UClass>())
		{
			UClass *u_class = (UClass *)self->ue_object;
			u_object = u_class->GetDefaultObject();
		}
	}
	else
	{
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	PyObject *py_struct_dict = PyDict_New();
	TFieldIterator<UProperty> SArgs(u_struct);
	for (; SArgs; ++SArgs)
	{
		PyObject *struct_value = ue_py_convert_property(*SArgs, (uint8 *)u_object, 0);
		if (!struct_value)
		{
			Py_DECREF(py_struct_dict);
			return NULL;
		}
		PyDict_SetItemString(py_struct_dict, TCHAR_TO_UTF8(*SArgs->GetName()), struct_value);
	}
	return py_struct_dict;
}

PyObject *py_ue_get_cdo(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<UClass>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
	}

	UClass *u_class = (UClass *)self->ue_object;

	Py_RETURN_UOBJECT(u_class->GetDefaultObject());
}

PyObject *py_ue_get_archetype(ue_PyUObject * self, PyObject * args)
{
	ue_py_check(self);

	Py_RETURN_UOBJECT(self->ue_object->GetArchetype());
}

PyObject *py_ue_get_archetype_instances(ue_PyUObject * self, PyObject * args)
{
    ue_py_check(self);

    TArray<UObject*> ObjectArchetypeInstances;
    self->ue_object->GetArchetypeInstances(ObjectArchetypeInstances);

    PyObject *retArchInstances = PyList_New(0);
    for (UObject* ObjectArchetype : ObjectArchetypeInstances)
    {
        ue_PyUObject *archInstance = ue_get_python_uobject(ObjectArchetype);
        if (archInstance)
        {
            PyList_Append(retArchInstances, (PyObject *)archInstance);
        }
    }

    return retArchInstances;
}


#if WITH_EDITOR
PyObject *py_ue_save_package(ue_PyUObject * self, PyObject * args)
{

	/*

		Here we have the following cases to manage:

		calling on a UObject without an outer
		calling on a UObject with an outer
		calling on a UObject with an outer and a name arg

	*/

	ue_py_check(self);

	bool has_package = false;

	char *name = nullptr;
	UPackage *package = nullptr;
	if (!PyArg_ParseTuple(args, "|s:save_package", &name))
	{
		return NULL;
	}
	UObject *outer = self->ue_object->GetOutermost();
	UObject *u_object = self->ue_object;

	if (outer && outer->IsA<UPackage>() && outer != GetTransientPackage())
	{
		package = (UPackage *)outer;
		has_package = true;
	}
	else if (u_object && u_object->IsA<UPackage>() && u_object != GetTransientPackage())
	{
		package = (UPackage *)u_object;
		has_package = true;
	}

	if (!package || name)
	{
		if (!name)
		{
			return PyErr_Format(PyExc_Exception, "the object has no associated package, please specify a name");
		}
		if (!has_package)
		{
			// unmark transient object
			if (u_object->HasAnyFlags(RF_Transient))
			{
				u_object->ClearFlags(RF_Transient);
				u_object->SetFlags(RF_Public | RF_Standalone);
			}
		}
		package = (UPackage *)StaticFindObject(nullptr, ANY_PACKAGE, UTF8_TO_TCHAR(name), true);
		// create a new package if it does not exist
		if (!package)
		{
			package = CreatePackage(nullptr, UTF8_TO_TCHAR(name));
		}
		if (!package)
			return PyErr_Format(PyExc_Exception, "unable to create package");
		package->FileName = *FPackageName::LongPackageNameToFilename(UTF8_TO_TCHAR(name), FPackageName::GetAssetPackageExtension());
		if (has_package)
		{
			FString split_path;
			FString split_filename;
			FString split_extension;
			FString split_base(UTF8_TO_TCHAR(name));
			FPaths::Split(split_base, split_path, split_filename, split_extension);
			u_object = DuplicateObject(self->ue_object, package, FName(*split_filename));
		}
		else
		{
			// move to object into the new package
			if (!self->ue_object->Rename(*(self->ue_object->GetName()), package, REN_Test))
			{
				return PyErr_Format(PyExc_Exception, "unable to set object outer to package");
			}
			if (!self->ue_object->Rename(*(self->ue_object->GetName()), package))
			{
				return PyErr_Format(PyExc_Exception, "unable to set object outer to package");
			}
		}
	}

	package->FullyLoad();
	package->MarkPackageDirty();

	if (package->FileName.IsNone())
	{
		package->FileName = *FPackageName::LongPackageNameToFilename(*package->GetPathName(), FPackageName::GetAssetPackageExtension());
		UE_LOG(LogPython, Warning, TEXT("no file mapped to UPackage %s, setting its FileName to %s"), *package->GetPathName(), *package->FileName.ToString());
	}

	// NOTE: FileName may not be a fully qualified filepath
	if (FPackageName::IsValidLongPackageName(package->FileName.ToString()))
	{
		package->FileName = *FPackageName::LongPackageNameToFilename(package->GetPathName(), FPackageName::GetAssetPackageExtension());
	}

	if (UPackage::SavePackage(package, u_object, RF_Public | RF_Standalone, *package->FileName.ToString()))
	{
		FAssetRegistryModule::AssetCreated(u_object);
		Py_RETURN_UOBJECT(u_object);
	}

	return PyErr_Format(PyExc_Exception, "unable to save package");
}

PyObject *py_ue_import_custom_properties(ue_PyUObject * self, PyObject * args)
{
	ue_py_check(self);

	char *t3d;

	if (!PyArg_ParseTuple(args, "s:import_custom_properties", &t3d))
	{
		return nullptr;
	}

	FFeedbackContextAnsi context;
	self->ue_object->ImportCustomProperties(UTF8_TO_TCHAR(t3d), &context);

	TArray<FString> errors;
	context.GetErrors(errors);

	if (errors.Num() > 0)
	{
		return PyErr_Format(PyExc_Exception, "%s", TCHAR_TO_UTF8(*errors[0]));
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_asset_can_reimport(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	if (FReimportManager::Instance()->CanReimport(self->ue_object))
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject *py_ue_asset_reimport(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_ask_for_new_file = nullptr;
	PyObject *py_show_notification = nullptr;
	char *filename = nullptr;
	if (!PyArg_ParseTuple(args, "|OOs:asset_reimport", &py_ask_for_new_file, &py_show_notification, &filename))
	{
		return NULL;
	}

	bool ask_for_new_file = false;
	bool show_notification = false;
	FString f_filename = FString("");

	if (py_ask_for_new_file && PyObject_IsTrue(py_ask_for_new_file))
		ask_for_new_file = true;

	if (py_show_notification && PyObject_IsTrue(py_show_notification))
		show_notification = true;

	if (filename)
		f_filename = FString(UTF8_TO_TCHAR(filename));

	if (FReimportManager::Instance()->Reimport(self->ue_object, ask_for_new_file, show_notification, f_filename))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_duplicate(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	char *package_name;
	char *object_name;
	PyObject *py_overwrite = nullptr;

	if (!PyArg_ParseTuple(args, "ss|O:duplicate", &package_name, &object_name, &py_overwrite))
		return nullptr;

	ObjectTools::FPackageGroupName pgn;
	pgn.ObjectName = UTF8_TO_TCHAR(object_name);
	pgn.GroupName = FString("");
	pgn.PackageName = UTF8_TO_TCHAR(package_name);

	TSet<UPackage *> refused;

#if ENGINE_MINOR_VERSION < 14
	UObject *new_asset = ObjectTools::DuplicateSingleObject(self->ue_object, pgn, refused);
#else
	UObject *new_asset = ObjectTools::DuplicateSingleObject(self->ue_object, pgn, refused, (py_overwrite && PyObject_IsTrue(py_overwrite)));
#endif

	if (!new_asset)
		return PyErr_Format(PyExc_Exception, "unable to duplicate object");

	Py_RETURN_UOBJECT(new_asset);
}
#endif


PyObject *py_ue_to_bytes(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	TArray<uint8> Bytes;

	FObjectWriter(self->ue_object, Bytes);

	return PyBytes_FromStringAndSize((const char *)Bytes.GetData(), Bytes.Num());
}

PyObject *py_ue_to_bytearray(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	TArray<uint8> Bytes;

	FObjectWriter(self->ue_object, Bytes);

	return PyByteArray_FromStringAndSize((const char *)Bytes.GetData(), Bytes.Num());
}

PyObject *py_ue_from_bytes(ue_PyUObject * self, PyObject * args)
{

	Py_buffer py_buf;

	if (!PyArg_ParseTuple(args, "z*:from_bytes", &py_buf))
		return nullptr;

	ue_py_check(self);

	TArray<uint8> Bytes;
	Bytes.AddUninitialized(py_buf.len);
	FMemory::Memcpy(Bytes.GetData(), py_buf.buf, py_buf.len);

	FObjectReader(self->ue_object, Bytes);

	Py_RETURN_NONE;
}
