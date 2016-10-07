#include "UnrealEnginePythonPrivatePCH.h"

#include "PythonDelegate.h"
#include "PythonFunction.h"

#if WITH_EDITOR
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "UnrealEd.h"
#endif

PyObject *py_ue_get_class(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	ue_PyUObject *ret = ue_get_python_wrapper(self->ue_object->GetClass());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_get_outer(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	ue_PyUObject *ret = ue_get_python_wrapper(self->ue_object->GetOuter());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_get_outermost(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	ue_PyUObject *ret = ue_get_python_wrapper(self->ue_object->GetOutermost());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_conditional_begin_destroy(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	self->ue_object->ConditionalBeginDestroy();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_is_a(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:is_a", &obj)) {
		return NULL;
	}

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (self->ue_object->IsA((UClass *)py_obj->ue_object)) {
		Py_INCREF(Py_True);
		return Py_True;
	}


	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_call_function(ue_PyUObject * self, PyObject * args, PyObject *kwargs) {

	ue_py_check(self);

	UFunction *function = nullptr;

	if (PyTuple_Size(args) < 1) {
		return PyErr_Format(PyExc_TypeError, "this function requires at least an argument");
	}

	PyObject *func_id = PyTuple_GetItem(args, 0);

	if (PyUnicode_Check(func_id)) {
		function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(PyUnicode_AsUTF8(func_id))));
	}

	if (!function)
		return PyErr_Format(PyExc_Exception, "unable to find function");

	return py_ue_ufunction_call(function, self->ue_object, args, 1, kwargs);

}

PyObject *py_ue_find_function(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_function", &name)) {
		return NULL;
	}

	UFunction *function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(name)));
	if (!function) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	UE_LOG(LogPython, Warning, TEXT("Func %d %d"), function->NumParms, function->ReturnValueOffset);

	ue_PyUObject *ret = ue_get_python_wrapper((UObject *)function);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_ue_set_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:set_name", &name)) {
		return NULL;
	}

	if (self->ue_object->Rename(UTF8_TO_TCHAR(name))) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_get_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);


	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetName())));
}

PyObject *py_ue_get_full_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);


	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetFullName())));
}

PyObject *py_ue_set_property(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *property_name;
	PyObject *property_value;
	if (!PyArg_ParseTuple(args, "sO:set_property", &property_name, &property_value)) {
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>()) {
		u_struct = (UStruct *)self->ue_object;
	}
	else {
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);


	if (!ue_py_convert_pyobject(property_value, u_property, (uint8 *)self->ue_object)) {
		return PyErr_Format(PyExc_Exception, "unable to set property %s", property_name);
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_enum_values(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (!self->ue_object->IsA<UEnum>())
		return PyErr_Format(PyExc_TypeError, "uobject is not a UEnum");

	UEnum *u_enum = (UEnum *)self->ue_object;
	uint8 max_enum_value = u_enum->GetMaxEnumValue();
	PyObject *ret = PyList_New(0);
	for (uint8 i = 0; i < max_enum_value; i++) {
		PyObject *py_long = PyUnicode_FromString(TCHAR_TO_UTF8(*u_enum->GetEnumName(i)));
		PyList_Append(ret, py_long);
		Py_DECREF(py_long);
	}
	return ret;
}

PyObject *py_ue_properties(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>()) {
		u_struct = (UStruct *)self->ue_object;
	}
	else {
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

PyObject *py_ue_call(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *call_args;
	if (!PyArg_ParseTuple(args, "s:call", &call_args)) {
		return NULL;
	}

	FOutputDeviceNull od_null;
	if (!self->ue_object->CallFunctionByNameWithArguments(UTF8_TO_TCHAR(call_args), od_null, NULL, true)) {
		return PyErr_Format(PyExc_Exception, "error while calling \"%s\"", call_args);
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_get_property(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:get_property", &property_name)) {
		return NULL;
	}

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>()) {
		u_struct = (UStruct *)self->ue_object;
	}
	else {
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	return ue_py_convert_property(u_property, (uint8 *)self->ue_object);
}

PyObject *py_ue_is_rooted(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (self->ue_object->IsRooted()) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}


PyObject *py_ue_add_to_root(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	self->ue_object->AddToRoot();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_remove_from_root(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	self->ue_object->RemoveFromRoot();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_bind_event(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *event_name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:bind_event", &event_name, &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "object is not callable");
	}

	return ue_bind_pyevent(self, FString(event_name), py_callable, true);
}

#if PY_MAJOR_VERSION >= 3
PyObject *py_ue_add_function(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:add_function", &name, &py_callable)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
	}

	UClass *u_class = (UClass *)self->ue_object;

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "object is not callable");
	}

	if (!unreal_engine_add_function(u_class, name, py_callable, FUNC_Native | FUNC_BlueprintCallable | FUNC_Public)) {
		return PyErr_Format(PyExc_Exception, "unable to add function");
	}

	Py_INCREF(Py_None);
	return Py_None;
}
#endif

PyObject *py_ue_add_property(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	char *name;
	PyObject *replicate = nullptr;
	if (!PyArg_ParseTuple(args, "Os|O:add_property", &obj, &name, &replicate)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UStruct>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UStruct");

	UObject *scope = nullptr;
	UProperty *u_property = nullptr;
	UClass *u_class = nullptr;
	bool is_array = false;

	EObjectFlags o_flags = RF_Public | RF_MarkAsNative | RF_Transient;

	if (ue_is_pyuobject(obj)) {
		ue_PyUObject *py_obj = (ue_PyUObject *)obj;
		if (!py_obj->ue_object->IsA<UClass>()) {
			return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
		}
		u_class = (UClass *)py_obj->ue_object;
		if (!u_class->IsChildOf<UProperty>())
			return PyErr_Format(PyExc_Exception, "uobject is not a UProperty");
		if (u_class == UArrayProperty::StaticClass())
			return PyErr_Format(PyExc_Exception, "please use a single-item list of property for arrays");
		scope = self->ue_object;
	}
	else if (PyList_Check(obj)) {
		if (PyList_Size(obj) == 1) {
			PyObject *py_item = PyList_GetItem(obj, 0);
			if (ue_is_pyuobject(py_item)) {
				ue_PyUObject *py_obj = (ue_PyUObject *)py_item;
				if (!py_obj->ue_object->IsA<UClass>()) {
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
	}

	if (!scope) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject or a single item list");
	}

	u_property = NewObject<UProperty>(scope, u_class, UTF8_TO_TCHAR(name), o_flags);
	if (!u_property) {
		if (is_array)
			scope->MarkPendingKill();
		return PyErr_Format(PyExc_Exception, "unable to allocate new UProperty");
	}

	uint64 flags = CPF_Edit | CPF_BlueprintVisible | CPF_Transient | CPF_ZeroConstructor;
	if (replicate && PyObject_IsTrue(replicate)) {
		flags |= CPF_Net;
	}

	if (is_array) {
		UArrayProperty *u_array = (UArrayProperty *)scope;
		u_array->AddCppProperty(u_property);
		u_property->SetPropertyFlags(flags);
		u_property = u_array;
	}

	if (u_class == UMulticastDelegateProperty::StaticClass()) {
		UMulticastDelegateProperty *mcp = (UMulticastDelegateProperty *)u_property;
		mcp->SignatureFunction = NewObject<UFunction>(self->ue_object);
		flags |= CPF_BlueprintAssignable | CPF_BlueprintCallable;
	}

	u_property->SetPropertyFlags(flags);
	u_property->ArrayDim = 1;

	UStruct *u_struct = (UStruct *)self->ue_object;
	u_struct->AddCppProperty(u_property);
	u_struct->StaticLink(true);


	if (u_struct->IsA<UClass>()) {
		UClass *owner_class = (UClass *)u_struct;
		owner_class->GetDefaultObject()->RemoveFromRoot();
		owner_class->GetDefaultObject()->MarkPendingKill();
		owner_class->ClassDefaultObject = nullptr;
		owner_class->GetDefaultObject();
		//u_property->InitializeValue_InContainer(owner_class->GetDefaultObject());
	}

	// TODO add default value

	ue_PyUObject *ret = ue_get_python_wrapper(u_property);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_as_dict(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UStruct *u_struct = nullptr;

	if (self->ue_object->IsA<UStruct>()) {
		u_struct = (UStruct *)self->ue_object;
	}
	else {
		u_struct = (UStruct *)self->ue_object->GetClass();
	}

	PyObject *py_struct_dict = PyDict_New();
	TFieldIterator<UProperty> SArgs(u_struct);
	for (; SArgs; ++SArgs) {
		PyObject *struct_value = ue_py_convert_property(*SArgs, (uint8 *)self->ue_object);
		if (!struct_value) {
			Py_DECREF(py_struct_dict);
			return NULL;
		}
		PyDict_SetItemString(py_struct_dict, TCHAR_TO_UTF8(*SArgs->GetName()), struct_value);
	}
	return py_struct_dict;
}

PyObject *py_ue_get_cdo(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
	}

	UClass *u_class = (UClass *)self->ue_object;

	ue_PyUObject *ret = ue_get_python_wrapper(u_class->GetDefaultObject());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

#if WITH_EDITOR
PyObject *py_ue_save_package(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:save_package", &name)) {
		return NULL;
	}

	UPackage *package = CreatePackage(nullptr, UTF8_TO_TCHAR(name));
	if (!package)
		return PyErr_Format(PyExc_Exception, "unable to create package");

	FString filename = FPackageName::LongPackageNameToFilename(UTF8_TO_TCHAR(name), FPackageName::GetAssetPackageExtension());

	if (!self->ue_object->Rename(*(self->ue_object->GetName()), package)) {
		return PyErr_Format(PyExc_Exception, "unable to set object outer to package");
	}

	package->FullyLoad();
	package->MarkPackageDirty();

	if (UPackage::SavePackage(package, self->ue_object, RF_Public | RF_Standalone, *filename)) {
		FAssetRegistryModule::AssetCreated(self->ue_object);
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_asset_reimport(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_ask_for_new_file = nullptr;
	PyObject *py_show_notification = nullptr;
	char *filename;
	if (!PyArg_ParseTuple(args, "|OOs:asset_reimport", &py_ask_for_new_file, &py_show_notification, &filename)) {
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

	if (FReimportManager::Instance()->Reimport(self->ue_object, ask_for_new_file, show_notification, f_filename)) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}
#endif

