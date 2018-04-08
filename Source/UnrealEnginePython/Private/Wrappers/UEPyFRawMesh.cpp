#include "UEPyFRawMesh.h"

#if WITH_EDITOR

#if ENGINE_MINOR_VERSION > 13

#include "Engine/StaticMesh.h"

static PyObject *py_ue_fraw_mesh_set_vertex_positions(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O:set_vertex_positions", &data))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<FVector> vertex;

	for (;;)
	{
		PyObject *item_x = PyIter_Next(iter);
		if (!item_x)
			break;

		if (ue_PyFVector *py_fvector = py_ue_is_fvector(item_x))
		{
			vertex.Add(py_fvector->vec);
		}
		else
		{
			if (!PyNumber_Check(item_x))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_x = PyNumber_Float(item_x);
			float x = PyFloat_AsDouble(py_x);
			Py_DECREF(py_x);

			PyObject *item_y = PyIter_Next(iter);
			if (!item_y)
				break;
			if (!PyNumber_Check(item_y))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_y = PyNumber_Float(item_y);
			float y = PyFloat_AsDouble(py_y);
			Py_DECREF(py_y);

			PyObject *item_z = PyIter_Next(iter);
			if (!item_z)
				break;
			if (!PyNumber_Check(item_z))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_z = PyNumber_Float(item_z);
			float z = PyFloat_AsDouble(py_z);
			Py_DECREF(py_z);

			vertex.Add(FVector(x, y, z));
		}
	}


	self->raw_mesh.VertexPositions = vertex;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fraw_mesh_set_wedge_tex_coords(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	int index = 0;
	if (!PyArg_ParseTuple(args, "O|i:set_wedge_tex_coords", &data, &index))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<FVector2D> uv;

	for (;;)
	{
		PyObject *item_x = PyIter_Next(iter);
		if (!item_x)
			break;

		if (PyTuple_Check(item_x) && PyTuple_Size(item_x) == 2)
		{
			float x, y;
			if (!PyArg_ParseTuple(item_x, "ff", &x, &y))
				return nullptr;
			uv.Add(FVector2D(x, y));
		}
		else
		{
			if (!PyNumber_Check(item_x))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_x = PyNumber_Float(item_x);
			float x = PyFloat_AsDouble(py_x);
			Py_DECREF(py_x);

			PyObject *item_y = PyIter_Next(iter);
			if (!item_y)
				break;
			if (!PyNumber_Check(item_y))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_y = PyNumber_Float(item_y);
			float y = PyFloat_AsDouble(py_y);
			Py_DECREF(py_y);

			uv.Add(FVector2D(x, y));
		}
	}


	self->raw_mesh.WedgeTexCoords[index] = uv;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fraw_mesh_set_wedge_indices(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O:set_wedge_indices", &data))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<uint32> indices;

	for (;;)
	{
		PyObject *item = PyIter_Next(iter);
		if (!item)
			break;
		if (!PyNumber_Check(item))
			return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
		PyObject *py_value = PyNumber_Long(item);
		uint32 i = PyLong_AsUnsignedLong(py_value);
		Py_DECREF(py_value);

		indices.Add(i);
	}


	self->raw_mesh.WedgeIndices = indices;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fraw_mesh_set_face_material_indices(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O:set_face_material_indices", &data))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<int32> indices;

	for (;;)
	{
		PyObject *item = PyIter_Next(iter);
		if (!item)
			break;
		if (!PyNumber_Check(item))
			return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
		PyObject *py_value = PyNumber_Long(item);
		int32 i = PyLong_AsLong(py_value);
		Py_DECREF(py_value);

		indices.Add(i);
	}


	self->raw_mesh.FaceMaterialIndices = indices;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}

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

static PyObject *py_ue_fraw_mesh_get_wedge_position(ue_PyFRawMesh *self, PyObject * args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i:get_wedge_position", &index))
	{
		return nullptr;
	}

	if (index > self->raw_mesh.WedgeIndices.Num() - 1 || index < 0)
		return PyErr_Format(PyExc_IndexError, "wedge index error");

	FVector vec = self->raw_mesh.GetWedgePosition(index);

	return py_ue_new_fvector(vec);
}

static PyObject *py_ue_fraw_mesh_set_wedge_tangent_x(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O:set_wedge_tangent_x", &data))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<FVector> vertex;

	for (;;)
	{
		PyObject *item_x = PyIter_Next(iter);
		if (!item_x)
			break;
		if (ue_PyFVector *py_fvector = py_ue_is_fvector(item_x))
		{
			vertex.Add(py_fvector->vec);
		}
		else
		{
			if (!PyNumber_Check(item_x))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_x = PyNumber_Float(item_x);
			float x = PyFloat_AsDouble(py_x);
			Py_DECREF(py_x);

			PyObject *item_y = PyIter_Next(iter);
			if (!item_y)
				break;
			if (!PyNumber_Check(item_y))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_y = PyNumber_Float(item_y);
			float y = PyFloat_AsDouble(py_y);
			Py_DECREF(py_y);

			PyObject *item_z = PyIter_Next(iter);
			if (!item_z)
				break;
			if (!PyNumber_Check(item_z))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_z = PyNumber_Float(item_z);
			float z = PyFloat_AsDouble(py_z);
			Py_DECREF(py_z);

			vertex.Add(FVector(x, y, z));
		}
	}


	self->raw_mesh.WedgeTangentX = vertex;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fraw_mesh_set_wedge_tangent_y(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O:set_wedge_tangent_y", &data))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<FVector> vertex;

	for (;;)
	{
		PyObject *item_x = PyIter_Next(iter);
		if (!item_x)
			break;
		if (ue_PyFVector *py_fvector = py_ue_is_fvector(item_x))
		{
			vertex.Add(py_fvector->vec);
		}
		else
		{
			if (!PyNumber_Check(item_x))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_x = PyNumber_Float(item_x);
			float x = PyFloat_AsDouble(py_x);
			Py_DECREF(py_x);

			PyObject *item_y = PyIter_Next(iter);
			if (!item_y)
				break;
			if (!PyNumber_Check(item_y))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_y = PyNumber_Float(item_y);
			float y = PyFloat_AsDouble(py_y);
			Py_DECREF(py_y);

			PyObject *item_z = PyIter_Next(iter);
			if (!item_z)
				break;
			if (!PyNumber_Check(item_z))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_z = PyNumber_Float(item_z);
			float z = PyFloat_AsDouble(py_z);
			Py_DECREF(py_z);

			vertex.Add(FVector(x, y, z));
		}
	}


	self->raw_mesh.WedgeTangentY = vertex;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fraw_mesh_set_wedge_tangent_z(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O:set_wedge_tangent_z", &data))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<FVector> vertex;

	for (;;)
	{
		PyObject *item_x = PyIter_Next(iter);
		if (!item_x)
			break;

		if (ue_PyFVector *py_fvector = py_ue_is_fvector(item_x))
		{
			vertex.Add(py_fvector->vec);
		}
		else
		{
			if (!PyNumber_Check(item_x))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_x = PyNumber_Float(item_x);
			float x = PyFloat_AsDouble(py_x);
			Py_DECREF(py_x);

			PyObject *item_y = PyIter_Next(iter);
			if (!item_y)
				break;
			if (!PyNumber_Check(item_y))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_y = PyNumber_Float(item_y);
			float y = PyFloat_AsDouble(py_y);
			Py_DECREF(py_y);

			PyObject *item_z = PyIter_Next(iter);
			if (!item_z)
				break;
			if (!PyNumber_Check(item_z))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_z = PyNumber_Float(item_z);
			float z = PyFloat_AsDouble(py_z);
			Py_DECREF(py_z);

			vertex.Add(FVector(x, y, z));
		}
	}


	self->raw_mesh.WedgeTangentZ = vertex;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}


static PyObject *py_ue_fraw_mesh_set_wedge_colors(ue_PyFRawMesh *self, PyObject * args)
{
	PyObject *data;
	if (!PyArg_ParseTuple(args, "O:set_wedge_colors", &data))
	{
		return nullptr;
	}

	PyObject *iter = PyObject_GetIter(data);
	if (!iter)
		return PyErr_Format(PyExc_TypeError, "argument is not an iterable");

	TArray<FColor> colors;

	for (;;)
	{
		PyObject *item_x = PyIter_Next(iter);
		if (!item_x)
			break;
		if (ue_PyFColor *py_fcolor = py_ue_is_fcolor(item_x))
		{
			colors.Add(py_fcolor->color);
		}
		else
		{
			if (!PyNumber_Check(item_x))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_x = PyNumber_Long(item_x);
			uint8 x = PyLong_AsUnsignedLong(py_x);
			Py_DECREF(py_x);

			PyObject *item_y = PyIter_Next(iter);
			if (!item_y)
				break;
			if (!PyNumber_Check(item_y))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_y = PyNumber_Long(item_y);
			uint8 y = PyLong_AsUnsignedLong(py_y);
			Py_DECREF(py_y);

			PyObject *item_z = PyIter_Next(iter);
			if (!item_z)
				break;
			if (!PyNumber_Check(item_z))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_z = PyNumber_Long(item_z);
			uint8 z = PyLong_AsUnsignedLong(py_z);
			Py_DECREF(py_z);

			PyObject *item_a = PyIter_Next(iter);
			if (!item_a)
				break;
			if (!PyNumber_Check(item_a))
				return PyErr_Format(PyExc_Exception, "argument iterable does not contains only numbers");
			PyObject *py_a = PyNumber_Long(item_a);
			uint8 a = PyLong_AsUnsignedLong(py_a);
			Py_DECREF(py_a);

			colors.Add(FColor(x, y, z, a));
		}
	}


	self->raw_mesh.WedgeColors = colors;

	Py_DECREF(iter);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fraw_mesh_get_vertex_positions(ue_PyFRawMesh *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.VertexPositions.Num(); i++)
	{
		PyList_Append(py_list, py_ue_new_fvector(self->raw_mesh.VertexPositions[i]));
	}

	return py_list;
}

static PyObject *py_ue_fraw_mesh_get_wedge_tangent_x(ue_PyFRawMesh *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.WedgeTangentX.Num(); i++)
	{
		PyList_Append(py_list, py_ue_new_fvector(self->raw_mesh.WedgeTangentX[i]));
	}

	return py_list;
}

static PyObject *py_ue_fraw_mesh_get_wedge_tangent_y(ue_PyFRawMesh *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.WedgeTangentY.Num(); i++)
	{
		PyList_Append(py_list, py_ue_new_fvector(self->raw_mesh.WedgeTangentY[i]));
	}

	return py_list;
}

static PyObject *py_ue_fraw_mesh_get_wedge_tangent_z(ue_PyFRawMesh *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.WedgeTangentZ.Num(); i++)
	{
		PyList_Append(py_list, py_ue_new_fvector(self->raw_mesh.WedgeTangentZ[i]));
	}

	return py_list;
}

static PyObject *py_ue_fraw_mesh_get_wedge_colors(ue_PyFRawMesh *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.WedgeColors.Num(); i++)
	{
		PyList_Append(py_list, py_ue_new_fcolor(self->raw_mesh.WedgeColors[i]));
	}

	return py_list;
}

static PyObject *py_ue_fraw_mesh_get_wedge_indices(ue_PyFRawMesh *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.WedgeIndices.Num(); i++)
	{
		PyList_Append(py_list, PyLong_FromUnsignedLong(self->raw_mesh.WedgeIndices[i]));
	}

	return py_list;
}

static PyObject *py_ue_fraw_mesh_get_wedges_num(ue_PyFRawMesh *self, PyObject * args)
{
	return PyLong_FromUnsignedLong(self->raw_mesh.WedgeIndices.Num());
}

static PyObject *py_ue_fraw_mesh_get_face_material_indices(ue_PyFRawMesh *self, PyObject * args)
{

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.FaceMaterialIndices.Num(); i++)
	{
		PyList_Append(py_list, PyLong_FromUnsignedLong(self->raw_mesh.FaceMaterialIndices[i]));
	}

	return py_list;
}

static PyObject *py_ue_fraw_mesh_get_wedge_tex_coords(ue_PyFRawMesh *self, PyObject * args)
{

	int index = 0;

	if (!PyArg_ParseTuple(args, "|i:get_wedge_tex_coords", &index))
		return nullptr;

	if (index < 0 || index >= MAX_MESH_TEXTURE_COORDS)
		return PyErr_Format(PyExc_Exception, "invalid TexCoords index");

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < self->raw_mesh.WedgeTexCoords[index].Num(); i++)
	{
		PyList_Append(py_list, Py_BuildValue((char*)"(ff)", self->raw_mesh.WedgeTexCoords[index][i].X, self->raw_mesh.WedgeTexCoords[index][i].Y));
	}

	return py_list;
}


static PyMethodDef ue_PyFRawMesh_methods[] = {
	{ "set_vertex_positions", (PyCFunction)py_ue_fraw_mesh_set_vertex_positions, METH_VARARGS, "" },
	{ "set_wedge_indices", (PyCFunction)py_ue_fraw_mesh_set_wedge_indices, METH_VARARGS, "" },
	{ "set_face_material_indices", (PyCFunction)py_ue_fraw_mesh_set_face_material_indices, METH_VARARGS, "" },
	{ "set_wedge_tex_coords", (PyCFunction)py_ue_fraw_mesh_set_wedge_tex_coords, METH_VARARGS, "" },
	{ "set_wedge_tangent_x", (PyCFunction)py_ue_fraw_mesh_set_wedge_tangent_x, METH_VARARGS, "" },
	{ "set_wedge_tangent_y", (PyCFunction)py_ue_fraw_mesh_set_wedge_tangent_y, METH_VARARGS, "" },
	{ "set_wedge_tangent_z", (PyCFunction)py_ue_fraw_mesh_set_wedge_tangent_z, METH_VARARGS, "" },
	{ "set_wedge_colors", (PyCFunction)py_ue_fraw_mesh_set_wedge_colors, METH_VARARGS, "" },
	{ "get_wedge_position", (PyCFunction)py_ue_fraw_mesh_get_wedge_position, METH_VARARGS, "" },
	{ "get_vertex_positions", (PyCFunction)py_ue_fraw_mesh_get_vertex_positions, METH_VARARGS, "" },
	{ "get_wedge_indices", (PyCFunction)py_ue_fraw_mesh_get_wedge_indices, METH_VARARGS, "" },
	{ "get_wedge_tex_coords", (PyCFunction)py_ue_fraw_mesh_get_wedge_tex_coords, METH_VARARGS, "" },
	{ "get_wedge_tangent_x", (PyCFunction)py_ue_fraw_mesh_get_wedge_tangent_x, METH_VARARGS, "" },
	{ "get_wedge_tangent_y", (PyCFunction)py_ue_fraw_mesh_get_wedge_tangent_y, METH_VARARGS, "" },
	{ "get_wedge_tangent_z", (PyCFunction)py_ue_fraw_mesh_get_wedge_tangent_z, METH_VARARGS, "" },
	{ "get_wedge_colors", (PyCFunction)py_ue_fraw_mesh_get_wedge_colors, METH_VARARGS, "" },
	{ "get_face_material_indices", (PyCFunction)py_ue_fraw_mesh_get_face_material_indices, METH_VARARGS, "" },
	{ "save_to_static_mesh_source_model", (PyCFunction)py_ue_fraw_mesh_save_to_static_mesh_source_model, METH_VARARGS, "" },
	{ "get_wedges_num", (PyCFunction)py_ue_fraw_mesh_get_wedges_num, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static int ue_py_fraw_mesh_init(ue_PyFRawMesh *self, PyObject *args, PyObject *kwargs)
{
	new(&self->raw_mesh) FRawMesh();
	return 0;
}

static PyTypeObject ue_PyFRawMeshType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FRawMesh", /* tp_name */
	sizeof(ue_PyFRawMesh),    /* tp_basicsize */
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
	"Unreal Engine FRawMesh", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFRawMesh_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

PyObject *py_ue_new_fraw_mesh(FRawMesh raw_mesh)
{
	ue_PyFRawMesh *ret = (ue_PyFRawMesh *)PyObject_New(ue_PyFRawMesh, &ue_PyFRawMeshType);

	new(&ret->raw_mesh) FRawMesh(raw_mesh);
	return (PyObject *)ret;
}

void ue_python_init_fraw_mesh(PyObject *ue_module)
{
	ue_PyFRawMeshType.tp_new = PyType_GenericNew;;
	ue_PyFRawMeshType.tp_init = (initproc)ue_py_fraw_mesh_init;
	if (PyType_Ready(&ue_PyFRawMeshType) < 0)
		return;

	Py_INCREF(&ue_PyFRawMeshType);
	PyModule_AddObject(ue_module, "FRawMesh", (PyObject *)&ue_PyFRawMeshType);
}

#endif
#endif
