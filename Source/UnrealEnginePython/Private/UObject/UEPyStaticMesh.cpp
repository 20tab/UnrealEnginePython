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

PyObject * py_ue_static_mesh_get_num_triangles(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "|i:get_num_tris", &lod_index))
		return nullptr;

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	if (lod_index < 0 || lod_index >= mesh->GetNumLODs())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", mesh->GetNumLODs() - 1);

	return PyLong_FromLong(mesh->RenderData ? mesh->RenderData->LODResources[lod_index].GetNumTriangles() : 0);
}

PyObject *py_ue_static_mesh_create_body_setup(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	mesh->CreateBodySetup();

	Py_RETURN_NONE;
}

PyObject *py_ue_static_mesh_can_lods_share_static_lighting(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	//if (mesh->CanLODsShareStaticLighting())
	//	Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

PyObject *py_ue_static_mesh_get_raw_mesh(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "|i:get_raw_mesh", &lod_index))
		return nullptr;

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	FRawMesh raw_mesh;

	if (lod_index < 0 || lod_index >= mesh->SourceModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index");

	mesh->SourceModels[lod_index].RawMeshBulkData->LoadRawMesh(raw_mesh);

	return py_ue_new_fraw_mesh(raw_mesh);
}

#endif