
#include "UEPyFbxMesh.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 12

#include "UEPyFbx.h"

static PyObject *py_ue_fbx_mesh_get_polygon_count(ue_PyFbxMesh *self, PyObject *args)
{
	return PyLong_FromLong(self->fbx_mesh->GetPolygonCount());
}

static PyObject *py_ue_fbx_mesh_get_control_points_count(ue_PyFbxMesh *self, PyObject *args)
{
	return PyLong_FromLong(self->fbx_mesh->GetControlPointsCount());
}

static PyObject *py_ue_fbx_mesh_get_polygon_vertex_count(ue_PyFbxMesh *self, PyObject *args)
{
	return PyLong_FromLong(self->fbx_mesh->GetPolygonVertexCount());
}

static PyObject *py_ue_fbx_mesh_get_name(ue_PyFbxMesh *self, PyObject *args)
{
	return PyUnicode_FromString(self->fbx_mesh->GetName());
}

static PyObject *py_ue_fbx_mesh_get_polygon_vertices(ue_PyFbxMesh *self, PyObject *args)
{
	PyObject *py_list = PyList_New(0);
	int *indices = self->fbx_mesh->GetPolygonVertices();
	for (int i = 0; i < self->fbx_mesh->GetPolygonVertexCount(); i++)
	{
		PyList_Append(py_list, PyLong_FromLong(indices[i]));
	}
	return py_list;
}

static PyObject *py_ue_fbx_mesh_get_control_points(ue_PyFbxMesh *self, PyObject *args)
{
	PyObject *py_list = PyList_New(0);
	FbxVector4* control_points = self->fbx_mesh->GetControlPoints();
	for (int i = 0; i < self->fbx_mesh->GetControlPointsCount(); i++)
	{
		FbxVector4 vec4 = control_points[i];
		PyList_Append(py_list, py_ue_new_fvector(FVector(vec4[0], vec4[1], vec4[2])));
	}
	return py_list;
}

static PyObject *py_ue_fbx_mesh_get_polygon_vertex_normals(ue_PyFbxMesh *self, PyObject *args)
{

	FbxArray<FbxVector4> normals;

	if (!self->fbx_mesh->GetPolygonVertexNormals(normals))
	{
		Py_RETURN_NONE;
	}

	PyObject *py_list = PyList_New(0);

	for (int i = 0; i < normals.Size(); i++)
	{
		FbxVector4 vec = normals.GetAt(i);
		PyList_Append(py_list, py_ue_new_fvector(FVector(vec[0], vec[1], vec[2])));
	}

	return py_list;
}

static PyObject *py_ue_fbx_mesh_get_uv_set_names(ue_PyFbxMesh *self, PyObject *args)
{
	PyObject *py_list = PyList_New(0);
	FbxStringList name_list;
	self->fbx_mesh->GetUVSetNames(name_list);

	for (int i = 0; i < name_list.GetCount(); i++)
	{
		PyList_Append(py_list, PyUnicode_FromString(name_list.GetStringAt(i)));
	}

	return py_list;
}

static PyObject *py_ue_fbx_mesh_get_polygon_vertex_uvs(ue_PyFbxMesh *self, PyObject *args)
{
	char *uv_set;
	if (!PyArg_ParseTuple(args, "s", &uv_set))
		return nullptr;

	FbxArray<FbxVector2> uvs;

	if (!self->fbx_mesh->GetPolygonVertexUVs(uv_set, uvs))
	{
		Py_RETURN_NONE;
	}

	PyObject *py_list = PyList_New(0);

	for (int i = 0; i < uvs.Size(); i++)
	{
		FbxVector2 vec = uvs.GetAt(i);
		PyList_Append(py_list, Py_BuildValue((char *)"(ff)", vec[0], vec[1]));
	}

	return py_list;
}

static PyObject *py_ue_fbx_mesh_remove_bad_polygons(ue_PyFbxMesh *self, PyObject *args)
{
	self->fbx_mesh->RemoveBadPolygons();
	Py_RETURN_NONE;
}

static PyMethodDef ue_PyFbxMesh_methods[] = {
	{ "remove_bad_polygons", (PyCFunction)py_ue_fbx_mesh_remove_bad_polygons, METH_VARARGS, "" },
	{ "get_polygon_count", (PyCFunction)py_ue_fbx_mesh_get_polygon_count, METH_VARARGS, "" },
	{ "get_polygon_vertices", (PyCFunction)py_ue_fbx_mesh_get_polygon_vertices, METH_VARARGS, "" },
	{ "get_polygon_vertex_count", (PyCFunction)py_ue_fbx_mesh_get_polygon_vertex_count, METH_VARARGS, "" },
	{ "get_control_points_count", (PyCFunction)py_ue_fbx_mesh_get_control_points_count, METH_VARARGS, "" },
	{ "get_control_points", (PyCFunction)py_ue_fbx_mesh_get_control_points, METH_VARARGS, "" },
	{ "get_polygon_vertex_uvs", (PyCFunction)py_ue_fbx_mesh_get_polygon_vertex_uvs, METH_VARARGS, "" },
	{ "get_uv_set_names", (PyCFunction)py_ue_fbx_mesh_get_uv_set_names, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_fbx_mesh_get_name, METH_VARARGS, "" },
	{ "get_polygon_vertex_normals", (PyCFunction)py_ue_fbx_mesh_get_polygon_vertex_normals, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxMeshType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxMesh", /* tp_name */
	sizeof(ue_PyFbxMesh),    /* tp_basicsize */
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
	"Unreal Engine FbxMesh", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxMesh_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_mesh_init(ue_PyFbxMesh *self, PyObject * args)
{
	PyObject *py_object;
	char *name;
	if (!PyArg_ParseTuple(args, "Os", &py_object, &name))
	{
		return -1;
	}

	ue_PyFbxManager *py_fbx_manager = py_ue_is_fbx_manager(py_object);
	if (!py_fbx_manager)
	{
		PyErr_SetString(PyExc_Exception, "argument is not a FbxManager");
		return -1;
	}

	self->fbx_mesh = FbxMesh::Create(py_fbx_manager->fbx_manager, name);
	return 0;
}

void ue_python_init_fbx_mesh(PyObject *ue_module)
{
	ue_PyFbxMeshType.tp_new = PyType_GenericNew;;
	ue_PyFbxMeshType.tp_init = (initproc)py_ue_fbx_mesh_init;
	if (PyType_Ready(&ue_PyFbxMeshType) < 0)
		return;

	Py_INCREF(&ue_PyFbxMeshType);
	PyModule_AddObject(ue_module, "FbxMesh", (PyObject *)&ue_PyFbxMeshType);
}

PyObject *py_ue_new_fbx_mesh(FbxMesh *fbx_mesh)
{
	ue_PyFbxMesh *ret = (ue_PyFbxMesh *)PyObject_New(ue_PyFbxMesh, &ue_PyFbxMeshType);
	ret->fbx_mesh = fbx_mesh;
	return (PyObject *)ret;
}

#endif
#endif
