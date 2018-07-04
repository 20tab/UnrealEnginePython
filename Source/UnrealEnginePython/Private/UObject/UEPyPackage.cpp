#include "UEPyPackage.h"


PyObject *py_ue_package_is_dirty(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	UPackage *package = ue_py_check_type<UPackage>(self);
	if (!package)
		return PyErr_Format(PyExc_Exception, "uobject is not an UPackage");

	if (package->IsDirty())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyObject *py_ue_package_get_filename(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);
	UPackage *package = ue_py_check_type<UPackage>(self);
	if (!package)
		return PyErr_Format(PyExc_Exception, "uobject is not an UPackage");

	FString Filename;
	if (!FPackageName::DoesPackageExist(package->GetPathName(), nullptr, &Filename))
		return PyErr_Format(PyExc_Exception, "package does not exist");

	return PyUnicode_FromString(TCHAR_TO_UTF8(*Filename));
}

PyObject *py_ue_package_make_unique_object_name(ue_PyUObject *self, PyObject * args)
{

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
