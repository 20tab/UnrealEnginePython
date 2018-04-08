#include "UEPyFSoftSkinVertex.h"

#if WITH_EDITOR

static PyObject *py_ue_fsoft_skin_vertex_get_color(ue_PyFSoftSkinVertex *self, void *closure)
{
	return py_ue_new_fcolor(self->ss_vertex.Color);
}

static int py_ue_fsoft_skin_vertex_set_color(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	ue_PyFColor *py_color = py_ue_is_fcolor(value);
	if (py_color)
	{
		self->ss_vertex.Color = py_color->color;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a FColor");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_position(ue_PyFSoftSkinVertex *self, void *closure)
{
	return py_ue_new_fvector(self->ss_vertex.Position);
}

static int py_ue_fsoft_skin_vertex_set_position(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		self->ss_vertex.Position = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a FVector");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_tangent_x(ue_PyFSoftSkinVertex *self, void *closure)
{
	return py_ue_new_fvector(self->ss_vertex.TangentX);
}

static int py_ue_fsoft_skin_vertex_set_tangent_x(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		self->ss_vertex.TangentX = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a FVector");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_tangent_y(ue_PyFSoftSkinVertex *self, void *closure)
{
	return py_ue_new_fvector(self->ss_vertex.TangentX);
}

static int py_ue_fsoft_skin_vertex_set_tangent_y(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		self->ss_vertex.TangentY = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a FVector");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_tangent_z(ue_PyFSoftSkinVertex *self, void *closure)
{
	return py_ue_new_fvector(self->ss_vertex.TangentZ);
}

static int py_ue_fsoft_skin_vertex_set_tangent_z(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	ue_PyFVector *py_vec = py_ue_is_fvector(value);
	if (py_vec)
	{
		self->ss_vertex.TangentZ = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a FVector");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_influence_bones(ue_PyFSoftSkinVertex *self, void *closure)
{
	uint8 *data = self->ss_vertex.InfluenceBones;
	return Py_BuildValue((char*)"(iiiiiiii)", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
}

static int py_ue_fsoft_skin_vertex_set_influence_bones(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	PyObject *py_iter = PyObject_GetIter(value);
	int pos = 0;
	if (py_iter)
	{
		while (PyObject *py_item = PyIter_Next(py_iter))
		{
			if (!PyNumber_Check(py_item))
			{
				Py_DECREF(py_iter);
				PyErr_SetString(PyExc_TypeError, "value is not an iterable of numbers");
				return -1;
			}
			PyObject *py_num = PyNumber_Long(py_item);
			uint8 num = PyLong_AsUnsignedLong(py_num);
			Py_DECREF(py_num);
			self->ss_vertex.InfluenceBones[pos] = num;
			pos++;
			if (pos >= MAX_TOTAL_INFLUENCES)
				break;
		}
		Py_DECREF(py_iter);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not an iterable of numbers");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_influence_weights(ue_PyFSoftSkinVertex *self, void *closure)
{
	uint8 *data = self->ss_vertex.InfluenceWeights;
	return Py_BuildValue((char*)"(iiiiiiii)", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
}

static int py_ue_fsoft_skin_vertex_set_influence_weights(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	PyObject *py_iter = PyObject_GetIter(value);
	int pos = 0;
	if (py_iter)
	{
		while (PyObject *py_item = PyIter_Next(py_iter))
		{
			if (!PyNumber_Check(py_item))
			{
				Py_DECREF(py_iter);
				PyErr_SetString(PyExc_TypeError, "value is not an iterable of numbers");
				return -1;
			}
			PyObject *py_num = PyNumber_Long(py_item);
			uint8 num = PyLong_AsUnsignedLong(py_num);
			Py_DECREF(py_num);
			self->ss_vertex.InfluenceWeights[pos] = num;
			pos++;
			if (pos >= MAX_TOTAL_INFLUENCES)
				break;
		}
		Py_DECREF(py_iter);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not an iterable of numbers");
	return -1;
}


static PyObject *py_ue_fsoft_skin_vertex_get_uvs(ue_PyFSoftSkinVertex *self, void *closure)
{
	PyObject *py_tuple = PyTuple_New(MAX_TEXCOORDS);
	for (int32 i = 0; i < MAX_TEXCOORDS; i++)
	{
		PyTuple_SetItem(py_tuple, i, Py_BuildValue((char*)"(ff)", self->ss_vertex.UVs[i].X, self->ss_vertex.UVs[i].Y));
	}
	return py_tuple;
}

static int py_ue_fsoft_skin_vertex_set_uvs(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{
	PyObject *py_iter = PyObject_GetIter(value);
	int pos = 0;
	if (py_iter)
	{
		while (PyObject *py_item = PyIter_Next(py_iter))
		{
			if (!PyTuple_Check(py_item))
			{
				Py_DECREF(py_iter);
				PyErr_SetString(PyExc_TypeError, "value is not an iterable of 2-members tuples");
				return -1;
			}

			if (PyTuple_Size(py_item) != 2)
			{
				Py_DECREF(py_iter);
				PyErr_SetString(PyExc_TypeError, "value is not an iterable of 2-members tuples");
				return -1;
			}

			PyObject *py_x = PyTuple_GetItem(py_item, 0);
			PyObject *py_y = PyTuple_GetItem(py_item, 1);

			if (!PyNumber_Check(py_x))
			{
				Py_DECREF(py_iter);
				PyErr_SetString(PyExc_TypeError, "tuple item is not a number");
				return -1;
			}
			PyObject *py_num = PyNumber_Float(py_x);
			float x = PyFloat_AsDouble(py_num);
			Py_DECREF(py_num);

			if (!PyNumber_Check(py_y))
			{
				Py_DECREF(py_iter);
				PyErr_SetString(PyExc_TypeError, "tuple item is not a number");
				return -1;
			}
			py_num = PyNumber_Float(py_y);
			float y = PyFloat_AsDouble(py_num);
			Py_DECREF(py_num);

			self->ss_vertex.UVs[pos] = FVector2D(x, y);
			pos++;
			if (pos >= MAX_TEXCOORDS)
				break;
		}
		Py_DECREF(py_iter);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not an iterable of numbers");
	return -1;
}

static int py_ue_fsoft_skin_vertex_set_material_index(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{

	if (PyNumber_Check(value))
	{
		PyObject *py_num = PyNumber_Long(value);
		self->material_index = PyLong_AsUnsignedLong(py_num);
		Py_DECREF(py_num);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a number");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_material_index(ue_PyFSoftSkinVertex *self, void *closure)
{
	return PyLong_FromUnsignedLong(self->material_index);
}

static int py_ue_fsoft_skin_vertex_set_smoothing_group(ue_PyFSoftSkinVertex *self, PyObject *value, void *closure)
{

	if (PyNumber_Check(value))
	{
		PyObject *py_num = PyNumber_Long(value);
		self->smoothing_group = PyLong_AsUnsignedLong(py_num);
		Py_DECREF(py_num);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a number");
	return -1;
}

static PyObject *py_ue_fsoft_skin_vertex_get_smoothing_group(ue_PyFSoftSkinVertex *self, void *closure)
{
	return PyLong_FromUnsignedLong(self->smoothing_group);
}

static PyGetSetDef ue_PyFSoftSkinVertex_getseters[] = {
	{ (char *) "color", (getter)py_ue_fsoft_skin_vertex_get_color, (setter)py_ue_fsoft_skin_vertex_set_color, (char *)"", NULL },
	{ (char *) "influence_bones", (getter)py_ue_fsoft_skin_vertex_get_influence_bones, (setter)py_ue_fsoft_skin_vertex_set_influence_bones, (char *)"", NULL },
	{ (char *) "influence_weights", (getter)py_ue_fsoft_skin_vertex_get_influence_weights, (setter)py_ue_fsoft_skin_vertex_set_influence_weights, (char *)"", NULL },
	{ (char *) "position", (getter)py_ue_fsoft_skin_vertex_get_position, (setter)py_ue_fsoft_skin_vertex_set_position, (char *)"", NULL },
	{ (char *) "tangent_x", (getter)py_ue_fsoft_skin_vertex_get_tangent_x, (setter)py_ue_fsoft_skin_vertex_set_tangent_x, (char *)"", NULL },
	{ (char *) "tangent_y", (getter)py_ue_fsoft_skin_vertex_get_tangent_y, (setter)py_ue_fsoft_skin_vertex_set_tangent_y, (char *)"", NULL },
	{ (char *) "tangent_z", (getter)py_ue_fsoft_skin_vertex_get_tangent_z, (setter)py_ue_fsoft_skin_vertex_set_tangent_z, (char *)"", NULL },
	{ (char *) "uvs", (getter)py_ue_fsoft_skin_vertex_get_uvs, (setter)py_ue_fsoft_skin_vertex_set_uvs, (char *)"", NULL },
	{ (char *) "material_index", (getter)py_ue_fsoft_skin_vertex_get_material_index, (setter)py_ue_fsoft_skin_vertex_set_material_index, (char *)"", NULL },
	{ (char *) "smoothing_group", (getter)py_ue_fsoft_skin_vertex_get_smoothing_group, (setter)py_ue_fsoft_skin_vertex_set_smoothing_group, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_fsoft_skin_vertex_copy(ue_PyFSoftSkinVertex *self, PyObject * args)
{
	ue_PyFSoftSkinVertex *py_ss_vertex = (ue_PyFSoftSkinVertex *)py_ue_new_fsoft_skin_vertex(self->ss_vertex);
	py_ss_vertex->material_index = self->material_index;
	py_ss_vertex->smoothing_group = self->smoothing_group;
	return (PyObject *)py_ss_vertex;
}

static PyMethodDef ue_PyFSoftSkinVertex_methods[] = {
	{ "copy", (PyCFunction)py_ue_fsoft_skin_vertex_copy, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFSoftSkinVertex_str(ue_PyFSoftSkinVertex *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FSoftSkinVertex {'position': {'x': %S, 'y': %S, 'z': %S}}>",
		PyFloat_FromDouble(self->ss_vertex.Position.X), PyFloat_FromDouble(self->ss_vertex.Position.Y), PyFloat_FromDouble(self->ss_vertex.Position.Z));
}

static PyTypeObject ue_PyFSoftSkinVertexType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FSoftSkinVertex", /* tp_name */
	sizeof(ue_PyFSoftSkinVertex), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,       /* tp_dealloc */
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
	(reprfunc)ue_PyFSoftSkinVertex_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FSoftSkinVertex",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFSoftSkinVertex_methods,             /* tp_methods */
	0,
	ue_PyFSoftSkinVertex_getseters,
};

static int py_ue_fsoft_skin_vertex_init(ue_PyFSoftSkinVertex *self, PyObject * args)
{
	new(&self->ss_vertex) FSoftSkinVertex();
	return 0;
}

void ue_python_init_fsoft_skin_vertex(PyObject *ue_module)
{
	ue_PyFSoftSkinVertexType.tp_new = PyType_GenericNew;
	ue_PyFSoftSkinVertexType.tp_init = (initproc)py_ue_fsoft_skin_vertex_init;

	if (PyType_Ready(&ue_PyFSoftSkinVertexType) < 0)
		return;

	Py_INCREF(&ue_PyFSoftSkinVertexType);
	PyModule_AddObject(ue_module, "FSoftSkinVertex", (PyObject *)&ue_PyFSoftSkinVertexType);
}

ue_PyFSoftSkinVertex *py_ue_is_fsoft_skin_vertex(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFSoftSkinVertexType))
		return nullptr;
	return (ue_PyFSoftSkinVertex *)obj;
}

PyObject *py_ue_new_fsoft_skin_vertex(FSoftSkinVertex ss_vertex)
{
	ue_PyFSoftSkinVertex *ret = (ue_PyFSoftSkinVertex *)PyObject_New(ue_PyFSoftSkinVertex, &ue_PyFSoftSkinVertexType);
	new(&ret->ss_vertex) FSoftSkinVertex(ss_vertex);
	return (PyObject *)ret;
}

#endif