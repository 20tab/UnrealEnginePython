
#include "UEPyProperty.h"

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)

#include "PythonDelegate.h"
#include "PythonFunction.h"
#include "Components/ActorComponent.h"
#include "Engine/UserDefinedEnum.h"

#if WITH_EDITOR
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "ObjectTools.h"
#include "UnrealEd.h"
#include "Runtime/Core/Public/HAL/FeedbackContextAnsi.h"

#include "Wrappers/UEPyFObjectThumbnail.h"
#endif

#include "Runtime/Core/Public/Misc/OutputDeviceNull.h"
#include "Runtime/CoreUObject/Public/Serialization/ObjectWriter.h"
#include "Runtime/CoreUObject/Public/Serialization/ObjectReader.h"



PyObject *get_fproperty_uclass(FProperty *property)
{
	UClass *u_class = nullptr;
	//ue_py_check(self);

	// why was I thinking this - not clear what I was trying to do here
	// return the UClass for an FProperty
	// or return UClass for a python wrapped FProperty??
	// obviously the return seems to be a python wrapped UClass object

	// not clear how to implement this
	// we may just have to check each FProperty class
	// and load the StaticClass of the appropriate UObject

	//u_class = (UClass *)property->ue_fproperty

	Py_RETURN_UOBJECT(u_class);
}


FProperty* FProperty_New(FFieldVariant* scope, FFieldClass* f_class, FName name, EObjectFlags o_flags)
{
	//FArrayProperty *f_array = new FArrayProperty(self->ue_object, UTF8_TO_TCHAR(name), o_flags);

	// for the moment lets just do explicit ifs
	if (f_class == FBoolProperty::StaticClass())
	{
		FBoolProperty *f_prop = new FBoolProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FIntProperty::StaticClass())
	{
		FIntProperty *f_prop = new FIntProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FUInt32Property::StaticClass())
	{
		FUInt32Property *f_prop = new FUInt32Property(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FInt64Property::StaticClass())
	{
		FInt64Property *f_prop = new FInt64Property(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FUInt64Property::StaticClass())
	{
		FUInt64Property *f_prop = new FUInt64Property(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FFloatProperty::StaticClass())
	{
		FFloatProperty *f_prop = new FFloatProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FByteProperty::StaticClass())
	{
		FByteProperty *f_prop = new FByteProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FEnumProperty::StaticClass())
	{
		FEnumProperty *f_prop = new FEnumProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FStrProperty::StaticClass())
	{
		FStrProperty *f_prop = new FStrProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FNameProperty::StaticClass())
	{
		FNameProperty *f_prop = new FNameProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FTextProperty::StaticClass())
	{
		FTextProperty *f_prop = new FTextProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FArrayProperty::StaticClass())
	{
		FArrayProperty *f_prop = new FArrayProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FMapProperty::StaticClass())
	{
		FMapProperty *f_prop = new FMapProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FStructProperty::StaticClass())
	{
		FStructProperty *f_prop = new FStructProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FClassProperty::StaticClass())
	{
		FClassProperty *f_prop = new FClassProperty(*scope, name, o_flags);
		return f_prop;
	}
	else if (f_class == FObjectPropertyBase::StaticClass())
	{
		FObjectPropertyBase *f_prop = new FObjectPropertyBase(*scope, name, o_flags);
		return f_prop;
	}

	UE_LOG(LogPython, Error, TEXT("new property unimplemented property class %s for %s"), TCHAR_TO_UTF8(*f_class->GetName()), *name.ToString());

	return nullptr;
}

#if WITH_EDITOR
// strictly WITH_EDITORONLY_DATA
PyObject *py_ue_fproperty_set_metadata(ue_PyFProperty * self, PyObject * args)
{

	// not clear what to do here - we can have a valid python uobject wrapper
	// with invalid uobject - which is what ue_py_check is testing for
	// can we have a valid python fproperty wrapper with invalid fproperty??
	//ue_py_check(self);
	//if (!ue_is_pyfproperty(self))
	//	return PyErr_Format(PyExc_Exception, "fproperty_set_metadata: self is not FProperty");
	if (self->ue_fproperty == nullptr)
		return PyErr_Format(PyExc_Exception, "PyFProperty is in invalid state");

	char *metadata_key;
	char *metadata_value;
	if (!PyArg_ParseTuple(args, "ss:set_metadata", &metadata_key, &metadata_value))
	{
		return NULL;
	}

	FProperty *f_property = (FProperty *)self->ue_fproperty;
	f_property->SetMetaData(FName(UTF8_TO_TCHAR(metadata_key)), UTF8_TO_TCHAR(metadata_value));

	Py_RETURN_NONE;
}

PyObject *py_ue_fproperty_get_metadata(ue_PyFProperty * self, PyObject * args)
{

	// not clear what to do here - we can have a valid python uobject wrapper
	// with invalid uobject - which is what ue_py_check is testing for
	// can we have a valid python fproperty wrapper with invalid fproperty??
	//ue_py_check(self);
	//if (!ue_is_pyfproperty(self))
	//	return PyErr_Format(PyExc_Exception, "fproperty_get_metadata: self is not FProperty");
	if (self->ue_fproperty == nullptr)
		return PyErr_Format(PyExc_Exception, "PyFProperty is in invalid state");

	char *metadata_key;
	if (!PyArg_ParseTuple(args, "s:get_metadata", &metadata_key))
	{
		return NULL;
	}

	char *metadata_value = nullptr;

	FProperty *f_property = (FProperty *)self->ue_fproperty;
	FString value = f_property->GetMetaData(FName(UTF8_TO_TCHAR(metadata_key)));
	return PyUnicode_FromString(TCHAR_TO_UTF8(*value));

}

PyObject *py_ue_fproperty_has_metadata(ue_PyFProperty * self, PyObject * args)
{

	// not clear what to do here - we can have a valid python uobject wrapper
	// with invalid uobject - which is what ue_py_check is testing for
	// can we have a valid python fproperty wrapper with invalid fproperty??
	//ue_py_check(self);
	//if (!ue_is_pyfproperty(self))
	//	return PyErr_Format(PyExc_Exception, "fproperty_has_metadata: self is not FProperty");
	if (self->ue_fproperty == nullptr)
		return PyErr_Format(PyExc_Exception, "PyFProperty is in invalid state");

	char *metadata_key;
	if (!PyArg_ParseTuple(args, "s:has_metadata", &metadata_key))
	{
		return NULL;
	}

	FProperty *f_property = (FProperty *)self->ue_fproperty;
	if (f_property->HasMetaData(FName(UTF8_TO_TCHAR(metadata_key))))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}
	Py_INCREF(Py_False);
	return Py_False;
}
#endif

#endif
