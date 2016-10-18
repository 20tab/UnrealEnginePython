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
