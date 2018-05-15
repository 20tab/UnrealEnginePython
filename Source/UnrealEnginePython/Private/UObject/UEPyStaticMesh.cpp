#include "UnrealEnginePythonPrivatePCH.h"
#include <limits>
#include "Runtime/Engine/Classes/PhysicsEngine/BodySetup.h"

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

// TODO: (sabhiram) - Move this to the MeshComponent class, and rename this to
//					  `component.get_screen_bounding_box()`
PyObject *py_ue_static_mesh_get_component_bounding_box(ue_PyUObject *self, PyObject *args) {

	ue_py_check(self);

	PyObject *py_obj_controller = nullptr;
	int lod_index = 0;

	if (!PyArg_ParseTuple(args, "O|i:get_component_bounding_box", &py_obj_controller, &lod_index))
		return nullptr;

	// Is this object a UStaticMeshComponent?
	UStaticMeshComponent *comp = ue_py_check_type<UStaticMeshComponent>(self);
	if (!comp)
		return PyErr_Format(PyExc_Exception, "uobject is not a UStaticMeshComponent");

	APlayerController *controller =  ue_py_check_type<APlayerController>(py_obj_controller);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "can't determine controller");

	UStaticMesh *StaticMesh = comp->GetStaticMesh();
	if(!StaticMesh)
		return PyErr_Format(PyExc_Exception, "empty static mesh");

	USceneComponent *component = ((USceneComponent *)self->ue_object);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	FTransform wt = component->GetComponentTransform();

	// Get the vertices
	float min_x = std::numeric_limits<float>::max();
	float min_y = std::numeric_limits<float>::max();
	float max_x = std::numeric_limits<float>::min();
	float max_y = std::numeric_limits<float>::min();

	for (auto convex_elem : StaticMesh->BodySetup->AggGeom.ConvexElems)
		for (auto vertex : convex_elem.VertexData)
		{
			FVector wp = wt.TransformPosition(vertex);
			FVector2D sp;
			if (!controller->ProjectWorldLocationToScreen(wp, sp, false))
				return PyErr_Format(PyExc_Exception, "unable to project coordinates");

			if(min_x > sp.X) min_x = sp.X;
			if(max_x < sp.X) max_x = sp.X;
			if(min_y > sp.Y) min_y = sp.Y;
			if(max_y < sp.Y) max_y = sp.Y;
		}

	// Create a min and max vector
	auto min_vector = py_ue_new_fvector(FVector(min_x, min_y, 0));
	auto max_vector = py_ue_new_fvector(FVector(max_x, max_y, 0));

	// Add them to a list
	PyObject *py_list = PyList_New(0);
	PyList_Append(py_list, min_vector);
	PyList_Append(py_list, max_vector);

	return py_list;
}

#endif
