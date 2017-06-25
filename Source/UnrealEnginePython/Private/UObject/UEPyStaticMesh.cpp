#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

PyObject *py_ue_static_mesh_build(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");
	
#if ENGINE_MINOR_VERSION > 13
	mesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
#endif
	mesh->Build();

	Py_RETURN_NONE;
}

PyObject *py_ue_static_mesh_create_body_setup(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	mesh->CreateBodySetup();

	Py_RETURN_NONE;
}

#endif