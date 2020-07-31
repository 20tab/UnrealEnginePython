#include "UEPyFMeshDescription.h"

#if WITH_EDITOR

#if ENGINE_MINOR_VERSION > 21

//#include "Engine/StaticMesh.h"


// so as far I can see ExportToRawMesh is primarily used to read the mesh data
// - and havent found a simple example in which we create a mesh using FMeshDescription
// so for the moment lets ignore creating a mesh functionality

#if 0

static PyObject *py_ue_fraw_mesh_save_to_static_mesh_source_model(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *model;
	if (!PyArg_ParseTuple(args, "O:save_to_static_mesh_source_model", &model))
	{
		return nullptr;
	}

       FStaticMeshSourceModel *source_model = ue_py_check_struct<FStaticMeshSourceModel>(model);
        if (!source_model)
                return PyErr_Format(PyExc_Exception, "argument is not a FStaticMeshSourceModel");

        if (self->raw_mesh.WedgeIndices.Num() >= 3)
        {
                // set default sane values (read: 0) to face materials and smoothing groups
                if (self->raw_mesh.FaceSmoothingMasks.Num() == 0)
                        self->raw_mesh.FaceSmoothingMasks.AddZeroed(self->raw_mesh.WedgeIndices.Num() / 3);
                if (self->raw_mesh.FaceMaterialIndices.Num() == 0)
                        self->raw_mesh.FaceMaterialIndices.AddZeroed(self->raw_mesh.WedgeIndices.Num() / 3);
        }

        if (!self->raw_mesh.IsValidOrFixable())
                return PyErr_Format(PyExc_Exception, "FRawMesh is not valid or fixable");

        source_model->RawMeshBulkData->SaveRawMesh(self->raw_mesh);

	Py_RETURN_NONE;
}
#endif

// I think we need a completely new function naming scheme here as
// not clear what the direct equivalent of Wedge is

// so I think the older FRawMesh only allowed triangles (Wedge)
// it seems FMeshDescription can have various forms of Polygon - explicitly Triangles but now can have Quads
// - ah but a Polygon is defined only in Triangles
// this complicates things

// ah - now think we dont have to set anything into the FMeshDescription
// - we create a new instance, then run ExportToRawMesh which fills the FMeshDescription with all the mesh data
// NOTE - there could be many types of FMeshDescription depending on the attributes set
//        NOT HANDLED - the assumption is this is from the Landscape ExportToRawMesh function
//        using FStaticMeshAttributes



PyObject *py_ue_new_fmeshvertex(FMeshVertex mesh_vertex);

PyObject *py_ue_new_fmeshvertexinstance(FMeshVertexInstance mesh_vertex_instance);


static PyObject *py_ue_fmesh_description_get_vertices_num(ue_PyFMeshDescription *self, PyObject * args)
{
	return PyLong_FromUnsignedLong(self->mesh_description.Vertices().Num());
}

static PyObject *py_ue_fmesh_description_get_vertex_positions(ue_PyFMeshDescription *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	// needs CHECKING - Vertices does not appear to contain positions but describes what the vertex is connected to
	// Im assuming the number of Vertices is same as number of Vertex Positions

#if ENGINE_MINOR_VERSION > 24
	// so this is how to get GetVertexPositions to work
	FStaticMeshAttributes MeshAttributes(self->mesh_description);
	TVertexAttributesConstRef<FVector> VertexPositions = MeshAttributes.GetVertexPositions();
#else
	// this also works
	// and is needed for 4.22 as there is no StaticMeshAttributes defined
	TVertexAttributesConstRef<FVector> VertexPositions = self->mesh_description.VertexAttributes().GetAttributesRef<FVector>(MeshAttribute::Vertex::Position);
#endif

	for (const FVertexID VertexID : self->mesh_description.Vertices().GetElementIDs())
	{
		PyList_Append(py_list, py_ue_new_fvector(VertexPositions[VertexID]));
	}

	return py_list;
}

static PyObject *py_ue_fmesh_description_get_vertices(ue_PyFMeshDescription *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (const FVertexID VertexID : self->mesh_description.Vertices().GetElementIDs())
	{
		PyObject *py_sub_list = PyList_New(0);

		TArray<FEdgeID> edge_ids;
		PyObject *py_edge_list = PyList_New(0);
		for (const FEdgeID EdgeID : self->mesh_description.GetVertexConnectedEdges(VertexID))
		{
			PyObject *idvalue = PyLong_FromUnsignedLong(EdgeID.GetValue());
			PyList_Append(py_edge_list, idvalue);
		}
		PyList_Append(py_sub_list, py_edge_list);

		TArray<FVertexInstanceID> vrtx_ids;
		PyObject *py_vrtx_list = PyList_New(0);
		for (const FVertexInstanceID VertexInstanceID : self->mesh_description.GetVertexVertexInstances(VertexID))
		{
			PyObject *idvalue = PyLong_FromUnsignedLong(VertexInstanceID.GetValue());
			PyList_Append(py_vrtx_list, idvalue);
		}
		PyList_Append(py_sub_list, py_vrtx_list);


		PyList_Append(py_list, py_sub_list);
	}

	return py_list;
}

static PyObject *py_ue_fmesh_description_get_edges(ue_PyFMeshDescription *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (const FVertexID VertexID : self->mesh_description.Vertices().GetElementIDs())
	{
		TArray<FEdgeID> edge_ids;
		PyObject *py_edge_list = PyList_New(0);
		for (const FEdgeID EdgeID : self->mesh_description.GetVertexConnectedEdges(VertexID))
		{
			PyObject *idvalue = PyLong_FromUnsignedLong(EdgeID.GetValue());
			PyList_Append(py_edge_list, idvalue);
		}
		PyList_Append(py_list, py_edge_list);
	}

	return py_list;
}

static PyObject *py_ue_fmesh_description_get_vertex_instances(ue_PyFMeshDescription *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (const FVertexID VertexID : self->mesh_description.Vertices().GetElementIDs())
	{
		TArray<FVertexInstanceID> vrtx_ids;
		PyObject *py_vrtx_list = PyList_New(0);
		for (const FVertexInstanceID VertexInstanceID : self->mesh_description.GetVertexVertexInstances(VertexID))
		{
			PyObject *idvalue = PyLong_FromUnsignedLong(VertexInstanceID.GetValue());
			PyList_Append(py_vrtx_list, idvalue);
		}
		PyList_Append(py_list, py_vrtx_list);
	}

	return py_list;
}

static PyObject *py_ue_fmesh_description_get_triangles_num(ue_PyFMeshDescription *self, PyObject * args)
{
	// these are labelled polygons but in ALandscapeProxy::ExportToRawMesh its clear they are triangles
	// this is number of faces
	return PyLong_FromUnsignedLong(self->mesh_description.Polygons().Num());
}

static PyObject *py_ue_fmesh_description_get_triangles(ue_PyFMeshDescription *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (const FPolygonID PolygonID : self->mesh_description.Polygons().GetElementIDs())
	{
		TArray<FPolygonID> poly_ids;
		PyObject *py_vrtx_list = PyList_New(0);
#if ENGINE_MINOR_VERSION > 24
		// not defined in 4.22 - just skip for now
		for (const FVertexInstanceID VertexInstanceID : self->mesh_description.GetPolygonVertexInstances(PolygonID))
		{
			PyObject *idvalue = PyLong_FromUnsignedLong(VertexInstanceID.GetValue());
			PyList_Append(py_vrtx_list, idvalue);
		}
		PyList_Append(py_list, py_vrtx_list);
#endif
	}

	return py_list;
}

static PyObject *py_ue_fmesh_description_get_polygons_num(ue_PyFMeshDescription *self, PyObject * args)
{
	// these are labelled polygons but in ALandscapeProxy::ExportToRawMesh its clear they are triangles
	// this is number of faces
	return PyLong_FromUnsignedLong(self->mesh_description.Polygons().Num());
}

static PyObject *py_ue_fmesh_description_get_polygons(ue_PyFMeshDescription *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (const FPolygonID PolygonID : self->mesh_description.Polygons().GetElementIDs())
	{
		PyObject *py_sub_list = PyList_New(0);

		TArray<FPolygonID> poly_ids;
		PyObject *py_vrtx_list = PyList_New(0);
#if ENGINE_MINOR_VERSION > 24
		// not defined in 4.22 - just skip for now
		for (const FVertexInstanceID VertexInstanceID : self->mesh_description.GetPolygonVertexInstances(PolygonID))
		{
			PyObject *idvalue = PyLong_FromUnsignedLong(VertexInstanceID.GetValue());
			PyList_Append(py_vrtx_list, idvalue);
		}
		PyList_Append(py_sub_list, py_vrtx_list);
#endif

#if ENGINE_MINOR_VERSION > 24
		// not defined in 4.22 - just skip for now
		TArray<FTriangleID> tri_ids;
		PyObject *py_tri_list = PyList_New(0);
		for (const FTriangleID TriangleID : self->mesh_description.GetPolygonTriangleIDs(PolygonID))
		{
			PyObject *idvalue = PyLong_FromUnsignedLong(TriangleID.GetValue());
			PyList_Append(py_tri_list, idvalue);
		}
		PyList_Append(py_sub_list, py_vrtx_list);
#else
		PyObject *py_tri_list = PyList_New(0);
#endif

		FPolygonGroupID PolygonGroupID = self->mesh_description.GetPolygonPolygonGroup(PolygonID);
		PyObject *idvalue = PyLong_FromUnsignedLong(PolygonGroupID.GetValue());
		PyList_Append(py_sub_list, idvalue);

		PyList_Append(py_list, py_sub_list);
	}

	return py_list;
}



static PyMethodDef ue_PyFMeshDescription_methods[] = {
#if 0
	{ "set_vertex_positions", (PyCFunction)py_ue_fraw_mesh_set_vertex_positions, METH_VARARGS, "" },
	{ "set_wedge_indices", (PyCFunction)py_ue_fraw_mesh_set_wedge_indices, METH_VARARGS, "" },
	{ "set_face_material_indices", (PyCFunction)py_ue_fraw_mesh_set_face_material_indices, METH_VARARGS, "" },
	{ "set_wedge_tex_coords", (PyCFunction)py_ue_fraw_mesh_set_wedge_tex_coords, METH_VARARGS, "" },
	{ "set_wedge_tangent_x", (PyCFunction)py_ue_fraw_mesh_set_wedge_tangent_x, METH_VARARGS, "" },
	{ "set_wedge_tangent_y", (PyCFunction)py_ue_fraw_mesh_set_wedge_tangent_y, METH_VARARGS, "" },
	{ "set_wedge_tangent_z", (PyCFunction)py_ue_fraw_mesh_set_wedge_tangent_z, METH_VARARGS, "" },
	{ "set_wedge_colors", (PyCFunction)py_ue_fraw_mesh_set_wedge_colors, METH_VARARGS, "" },
#endif
#if 0
	{ "get_wedge_position", (PyCFunction)py_ue_fmesh_description_get_wedge_position, METH_VARARGS, "" },
	{ "get_wedge_indices", (PyCFunction)py_ue_fmesh_description_get_wedge_indices, METH_VARARGS, "" },
	{ "get_wedge_tex_coords", (PyCFunction)py_ue_fmesh_description_get_wedge_tex_coords, METH_VARARGS, "" },
	{ "get_wedge_tangent_x", (PyCFunction)py_ue_fmesh_description_get_wedge_tangent_x, METH_VARARGS, "" },
	{ "get_wedge_tangent_y", (PyCFunction)py_ue_fmesh_description_get_wedge_tangent_y, METH_VARARGS, "" },
	{ "get_wedge_tangent_z", (PyCFunction)py_ue_fmesh_description_get_wedge_tangent_z, METH_VARARGS, "" },
	{ "get_wedge_colors", (PyCFunction)py_ue_fmesh_description_get_wedge_colors, METH_VARARGS, "" },
	{ "get_face_material_indices", (PyCFunction)py_ue_fmesh_description_get_face_material_indices, METH_VARARGS, "" },
	{ "save_to_static_mesh_source_model", (PyCFunction)py_ue_fmesh_description_save_to_static_mesh_source_model, METH_VARARGS, "" },
	{ "get_wedges_num", (PyCFunction)py_ue_fmesh_description_get_wedges_num, METH_VARARGS, "" },
#endif
	{ "get_vertices_num", (PyCFunction)py_ue_fmesh_description_get_vertices_num, METH_VARARGS, "" },
	{ "get_vertices", (PyCFunction)py_ue_fmesh_description_get_vertices, METH_VARARGS, "" },
	{ "get_vertex_instances", (PyCFunction)py_ue_fmesh_description_get_vertex_instances, METH_VARARGS, "" },
	{ "get_vertex_positions", (PyCFunction)py_ue_fmesh_description_get_vertex_positions, METH_VARARGS, "" },
	{ "get_triangles_num", (PyCFunction)py_ue_fmesh_description_get_triangles_num, METH_VARARGS, "" },
	{ "get_triangles", (PyCFunction)py_ue_fmesh_description_get_triangles, METH_VARARGS, "" },
	{ "get_polygons_num", (PyCFunction)py_ue_fmesh_description_get_polygons_num, METH_VARARGS, "" },
	{ "get_polygons", (PyCFunction)py_ue_fmesh_description_get_polygons, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static int ue_py_fmesh_description_init(ue_PyFMeshDescription *self, PyObject *args, PyObject *kwargs)
{
	new(&self->mesh_description) FMeshDescription();
	return 0;
}

static PyTypeObject ue_PyFMeshDescriptionType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FMeshDescription", /* tp_name */
	sizeof(ue_PyFMeshDescription),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,   /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FMeshDesription", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFMeshDescription_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

PyObject *py_ue_new_fmesh_description(FMeshDescription mesh_description)
{
	// so this creates a new python wrapper around a copy of the passed raw FMeshDescription object
	ue_PyFMeshDescription *ret = (ue_PyFMeshDescription *)PyObject_New(ue_PyFMeshDescription, &ue_PyFMeshDescriptionType);

	new(&ret->mesh_description) FMeshDescription(mesh_description);

	return (PyObject *)ret;
}

void ue_python_init_fmesh_description(PyObject *ue_module)
{
	ue_PyFMeshDescriptionType.tp_new = PyType_GenericNew;;
	ue_PyFMeshDescriptionType.tp_init = (initproc)ue_py_fmesh_description_init;
	if (PyType_Ready(&ue_PyFMeshDescriptionType) < 0)
		return;

	Py_INCREF(&ue_PyFMeshDescriptionType);
	PyModule_AddObject(ue_module, "FMeshDescription", (PyObject *)&ue_PyFMeshDescriptionType);
}



// wrap the subcomponents of mesh descriptions




#if 0


// FMeshVertex

static PyObject *py_ue_fmesh_vertex_get_vertex_instance_ids(ue_PyFMeshVertex *self, PyObject * args)
{
	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->mesh_vertex.VertexInstanceIDs.Num(); i++)
	{
		PyObject* id_value = PyLong_FromUnsignedLong(self->mesh_vertex.VertexInstanceIDs[i].GetValue());
		PyList_Append(py_list, id_value);
	}

	return py_list;
}

static PyObject *py_ue_fmesh_vertex_get_edge_ids(ue_PyFMeshVertex *self, PyObject * args)
{
	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->mesh_vertex.ConnectedEdgeIDs.Num(); i++)
	{
		PyObject* id_value = PyLong_FromUnsignedLong(self->mesh_vertex.ConnectedEdgeIDs[i].GetValue());
		PyList_Append(py_list, id_value);
	}

	return py_list;
}


static PyMethodDef ue_PyFMeshVertex_methods[] = {
	{ "get_vertex_instance_ids", (PyCFunction)py_ue_fmesh_vertex_get_vertex_instance_ids, METH_VARARGS, "" },
	{ "get_edge_ids", (PyCFunction)py_ue_fmesh_vertex_get_edge_ids, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static int ue_py_fmeshvertex_init(ue_PyFMeshVertex *self, PyObject *args, PyObject *kwargs)
{
	new(&self->mesh_vertex) FMeshVertex();
	return 0;
}

static PyTypeObject ue_PyFMeshVertexType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FMeshVertex", /* tp_name */
	sizeof(ue_PyFMeshVertex),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,   /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FMeshVertex", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFMeshVertex_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

PyObject *py_ue_new_fmeshvertex(FMeshVertex mesh_vertex)
{
	// so this creates a new python wrapper around a copy of the passed raw FMeshDescription object
	ue_PyFMeshVertex *ret = (ue_PyFMeshVertex *)PyObject_New(ue_PyFMeshVertex, &ue_PyFMeshVertexType);

	new(&ret->mesh_vertex) FMeshVertex(mesh_vertex);
	return (PyObject *)ret;
}

void ue_python_init_fmeshvertex(PyObject *ue_module)
{
	ue_PyFMeshVertexType.tp_new = PyType_GenericNew;;
	ue_PyFMeshVertexType.tp_init = (initproc)ue_py_fmeshvertex_init;
	if (PyType_Ready(&ue_PyFMeshVertexType) < 0)
		return;

	Py_INCREF(&ue_PyFMeshVertexType);
	PyModule_AddObject(ue_module, "FMeshVertex", (PyObject *)&ue_PyFMeshVertexType);
}



// FMeshVertexInstance

static PyObject *py_ue_fmesh_vertex_instance_get_vertex_id(ue_PyFMeshVertexInstance *self, PyObject * args)
{
	return PyLong_FromUnsignedLong(self->mesh_vertex_instance.VertexID.GetValue());
}

static PyObject *py_ue_fmesh_vertex_instance_get_triangles(ue_PyFMeshVertexInstance *self, PyObject * args)
{
	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->mesh_vertex_instance.ConnectedTriangles.Num(); i++)
	{
		PyObject* id_value = PyLong_FromUnsignedLong(self->mesh_vertex_instance.ConnectedTriangles[i].GetValue());
		PyList_Append(py_list, id_value);
	}

	return py_list;
}


static PyMethodDef ue_PyFMeshVertexInstance_methods[] = {
	{ "get_vertex_id", (PyCFunction)py_ue_fmesh_vertex_instance_get_vertex_id, METH_VARARGS, "" },
	{ "get_triangles", (PyCFunction)py_ue_fmesh_vertex_instance_get_triangles, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static int ue_py_fmeshvertexinstance_init(ue_PyFMeshVertexInstance *self, PyObject *args, PyObject *kwargs)
{
	new(&self->mesh_vertex_instance) FMeshVertexInstance();
	return 0;
}

static PyTypeObject ue_PyFMeshVertexInstanceType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FMeshVertexInstance", /* tp_name */
	sizeof(ue_PyFMeshVertexInstance),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,   /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FMeshVertexInstance", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFMeshVertexInstance_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

PyObject *py_ue_new_fmeshvertexinstance(FMeshVertexInstance mesh_vertex_instance)
{
	// so this creates a new python wrapper around a copy of the passed raw FMeshDescription object
	ue_PyFMeshVertexInstance *ret = (ue_PyFMeshVertexInstance *)PyObject_New(ue_PyFMeshVertexInstance, &ue_PyFMeshVertexInstanceType);

	new(&ret->mesh_vertex_instance) FMeshVertexInstance(mesh_vertex_instance);
	return (PyObject *)ret;
}

void ue_python_init_fmeshvertexinstance(PyObject *ue_module)
{
	ue_PyFMeshVertexInstanceType.tp_new = PyType_GenericNew;;
	ue_PyFMeshVertexInstanceType.tp_init = (initproc)ue_py_fmeshvertexinstance_init;
	if (PyType_Ready(&ue_PyFMeshVertexInstanceType) < 0)
		return;

	Py_INCREF(&ue_PyFMeshVertexInstanceType);
	PyModule_AddObject(ue_module, "FMeshVertexInstance", (PyObject *)&ue_PyFMeshVertexInstanceType);
}



// FMeshPolygon

static PyObject *py_ue_fmesh_polygon_get_vertex_instance_ids(ue_PyFMeshPolygon *self, PyObject * args)
{
	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->mesh_polygon.VertexInstanceIDs.Num(); i++)
	{
		PyObject* id_value = PyLong_FromUnsignedLong(self->mesh_polygon.VertexInstanceIDs[i].GetValue());
		PyList_Append(py_list, id_value);
	}

	return py_list;
}

static PyObject *py_ue_fmesh_polygon_get_triangle_instance_ids(ue_PyFMeshPolygon *self, PyObject * args)
{
	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->mesh_polygon.TriangleIDs.Num(); i++)
	{
		PyObject* id_value = PyLong_FromUnsignedLong(self->mesh_polygon.TriangleIDs[i].GetValue());
		PyList_Append(py_list, id_value);
	}

	return py_list;
}

static PyObject *py_ue_fmesh_polygon_get_polygon_group_id(ue_PyFMeshPolygon *self, PyObject * args)
{
	return PyLong_FromUnsignedLong(self->mesh_polygon.PolygonGroupID.GetValue())
}


static PyMethodDef ue_PyFMeshPolygon_methods[] = {
	{ "get_vertex_instance_ids", (PyCFunction)py_ue_fmesh_polygon_get_vertex_instance_ids, METH_VARARGS, "" },
	{ "get_triangle_ids", (PyCFunction)py_ue_fmesh_polygon_get_triangle_ids, METH_VARARGS, "" },
	{ "get_polygon_group_id", (PyCFunction)py_ue_fmesh_polygon_get_polygon_group_id, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static int ue_py_fmeshpolygon_init(ue_PyFMeshPolygon *self, PyObject *args, PyObject *kwargs)
{
	new(&self->mesh_polygon) FMeshPolygon();
	return 0;
}

static PyTypeObject ue_PyFMeshPolygonType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FMeshPolygon", /* tp_name */
	sizeof(ue_PyFMeshPolygon),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,   /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FMeshPolygon", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFMeshPolygon_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

PyObject *py_ue_new_fmeshpolygon(FMeshPolygon mesh_polygon)
{
	// so this creates a new python wrapper around a copy of the passed raw FMeshDescription object
	ue_PyFMeshPolygon *ret = (ue_PyFMeshPolygon *)PyObject_New(ue_PyFMeshPolygon, &ue_PyFMeshPolygonType);

	new(&ret->mesh_polygon) FMeshPolygon(mesh_polygon);
	return (PyObject *)ret;
}

void ue_python_init_fmeshpolygon(PyObject *ue_module)
{
	ue_PyFMeshPolygonType.tp_new = PyType_GenericNew;;
	ue_PyFMeshPolygonType.tp_init = (initproc)ue_py_fmeshpolygon_init;
	if (PyType_Ready(&ue_PyFMeshPolygonType) < 0)
		return;

	Py_INCREF(&ue_PyFMeshPolygonType);
	PyModule_AddObject(ue_module, "FMeshPolygon", (PyObject *)&ue_PyFMeshPolygonType);
}

#endif


#endif
#endif
