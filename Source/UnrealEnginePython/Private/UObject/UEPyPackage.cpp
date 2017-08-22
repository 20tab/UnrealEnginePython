#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_package_is_dirty(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);
	if (!self->ue_object->IsA<UPackage>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an UPackage");
	}
	UPackage *package = (UPackage *)self->ue_object;
	if (package->IsDirty())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyObject *py_ue_package_get_filename(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);
	if (!self->ue_object->IsA<UPackage>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an UPackage");
	}
	
	UPackage *package = (UPackage *)self->ue_object;
	return PyUnicode_FromString(TCHAR_TO_UTF8(*package->FileName.ToString()));
}

PyObject *py_ue_package_make_unique_object_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_class;
	char *prefix = nullptr;

	if (!PyArg_ParseTuple(args, "Os:make_unique_object_name", &py_class, &prefix))
		return nullptr;

	UPackage *package = ue_py_check_type<UPackage>(self);
	if (!package)
		return PyErr_Format(PyExc_Exception, "uobject is not an UPackage");

	UClass *u_class = ue_py_check_type<UClass>(py_class);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	FName name = NAME_None;
	if (prefix)
		name = FName(UTF8_TO_TCHAR(prefix));

	FName new_name = MakeUniqueObjectName(package, u_class, name);
	return PyUnicode_FromString(TCHAR_TO_UTF8(*new_name.ToString()));
}
