#include "UEPyStaticMesh.h"
#include "Engine/StaticMesh.h"

PyObject *py_ue_static_mesh_get_bounds(ue_PyUObject *self, PyObject * args)
{
    ue_py_check(self);
    UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
    if (!mesh)
        return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

    FBoxSphereBounds bounds = mesh->GetBounds();
    UScriptStruct *u_struct = FindObject<UScriptStruct>(ANY_PACKAGE, UTF8_TO_TCHAR("BoxSphereBounds"));
    if (!u_struct)
    {
        return PyErr_Format(PyExc_Exception, "unable to get BoxSphereBounds struct");
    }
    return py_ue_new_owned_uscriptstruct(u_struct, (uint8 *)&bounds);
}

#if WITH_EDITOR

#include "Wrappers/UEPyFRawMesh.h"
#include "Editor/UnrealEd/Private/GeomFitUtils.h"
#include "FbxMeshUtils.h"

static PyObject *generate_kdop(ue_PyUObject *self, const FVector *directions, uint32 num_directions)
{
	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	TArray<FVector> DirArray;
	for (uint32 i = 0; i < num_directions; i++)
	{
		DirArray.Add(directions[i]);
	}

	if (GenerateKDopAsSimpleCollision(mesh, DirArray) == INDEX_NONE)
	{
		return PyErr_Format(PyExc_Exception, "unable to generate KDop vectors");
	}

	PyObject *py_list = PyList_New(0);
	for (FVector v : DirArray)
	{
		PyList_Append(py_list, py_ue_new_fvector(v));
	}
	return py_list;
}

PyObject *py_ue_static_mesh_generate_kdop10x(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	return generate_kdop(self, KDopDir10X, 10);
}

PyObject *py_ue_static_mesh_generate_kdop10y(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	return generate_kdop(self, KDopDir10Y, 10);
}

PyObject *py_ue_static_mesh_generate_kdop10z(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	return generate_kdop(self, KDopDir10Z, 10);
}

PyObject *py_ue_static_mesh_generate_kdop18(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	return generate_kdop(self, KDopDir18, 18);
}

PyObject *py_ue_static_mesh_generate_kdop26(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	return generate_kdop(self, KDopDir26, 26);
}

PyObject *py_ue_static_mesh_build(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_static_mesh_create_body_setup(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	mesh->CreateBodySetup();

	Py_RETURN_NONE;
}

PyObject *py_ue_static_mesh_get_raw_mesh(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_static_mesh_import_lod(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	char *filename;
	int lod_level;
	if (!PyArg_ParseTuple(args, "si:static_mesh_import_lod", &filename, &lod_level))
		return nullptr;

	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMesh");

	if (FbxMeshUtils::ImportStaticMeshLOD(mesh, FString(UTF8_TO_TCHAR(filename)), lod_level))
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

#endif
