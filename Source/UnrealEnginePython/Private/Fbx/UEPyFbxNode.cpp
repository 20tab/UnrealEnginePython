#include "UEPyFbxNode.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 12

#include "UEPyFbx.h"


static PyObject *py_ue_fbx_node_get_child_count(ue_PyFbxNode *self, PyObject *args)
{
	return PyLong_FromLong(self->fbx_node->GetChildCount());
}

static PyObject *py_ue_fbx_node_get_name(ue_PyFbxNode *self, PyObject *args)
{
	return PyUnicode_FromString(self->fbx_node->GetName());
}

static PyObject *py_ue_fbx_node_get_local_translation(ue_PyFbxNode *self, PyObject *args)
{
	FbxDouble3 fbx_vec = self->fbx_node->LclTranslation.Get();
	return py_ue_new_fvector(FVector(fbx_vec[0], fbx_vec[1], fbx_vec[2]));
}

static PyObject *py_ue_fbx_node_get_local_rotation(ue_PyFbxNode *self, PyObject *args)
{
	FbxDouble3 fbx_vec = self->fbx_node->LclRotation.Get();
	return py_ue_new_fvector(FVector(fbx_vec[0], fbx_vec[1], fbx_vec[2]));
}

static PyObject *py_ue_fbx_node_get_local_scaling(ue_PyFbxNode *self, PyObject *args)
{
	FbxDouble3 fbx_vec = self->fbx_node->LclScaling.Get();
	return py_ue_new_fvector(FVector(fbx_vec[0], fbx_vec[1], fbx_vec[2]));
}

static PyObject *py_ue_fbx_node_get_child(ue_PyFbxNode *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxNode *fbx_node = self->fbx_node->GetChild(index);
	if (!fbx_node)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve FbxNode at index %d", index);
	}
	return py_ue_new_fbx_node(fbx_node);
}

static PyObject *py_ue_fbx_node_get_parent(ue_PyFbxNode *self, PyObject *args)
{
	FbxNode *fbx_node = self->fbx_node->GetParent();
	if (!fbx_node)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve FbxNode parent");
	}
	return py_ue_new_fbx_node(fbx_node);
}

static PyObject *py_ue_fbx_node_get_node_attribute(ue_PyFbxNode *self, PyObject *args)
{

	FbxNodeAttribute *fbx_node_attribute = self->fbx_node->GetNodeAttribute();
	if (!fbx_node_attribute)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve FbxNodeAttribute");
	}
	return py_ue_new_fbx_object(fbx_node_attribute);
}

static PyObject *py_ue_fbx_node_get_node_attribute_count(ue_PyFbxNode *self, PyObject *args)
{
	return PyLong_FromLong(self->fbx_node->GetNodeAttributeCount());
}

static PyObject *py_ue_fbx_node_get_node_attribute_by_index(ue_PyFbxNode *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxNodeAttribute *fbx_node_attribute = self->fbx_node->GetNodeAttributeByIndex(index);
	if (!fbx_node_attribute)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve FbxNodeAttribute at index %d", index);
	}
	return py_ue_new_fbx_object(fbx_node_attribute);
}

static PyObject *py_ue_fbx_node_get_mesh(ue_PyFbxNode *self, PyObject *args)
{

	FbxMesh *fbx_mesh = self->fbx_node->GetMesh();
	if (!fbx_mesh)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve FbxMesh from FbxNode");
	}
	return py_ue_new_fbx_mesh(fbx_mesh);
}

static PyObject *py_ue_fbx_node_evaluate_local_transform(ue_PyFbxNode *self, PyObject *args)
{
	float t;
	if (!PyArg_ParseTuple(args, "f", &t))
	{
		return nullptr;
	}
	FbxTime time;
	time.SetSecondDouble(t);
	FbxAMatrix& matrix = self->fbx_node->EvaluateLocalTransform(time);
	FTransform transform;
	FbxVector4 mt = matrix.GetT();
	FbxQuaternion mq = matrix.GetQ();
	FbxVector4 ms = matrix.GetS();
	transform.SetTranslation(FVector(mt[0], -mt[1], mt[2]));
	transform.SetRotation(FQuat(mq[0], -mq[1], mq[2], -mq[3]));
	transform.SetScale3D(FVector(ms[0], ms[1], ms[2]));
	return py_ue_new_ftransform(transform);
}

static PyObject *py_ue_fbx_node_evaluate_global_transform(ue_PyFbxNode *self, PyObject *args)
{
	float t;
	if (!PyArg_ParseTuple(args, "f", &t))
	{
		return nullptr;
	}
	FbxTime time;
	time.SetSecondDouble(t);
	FbxAMatrix& matrix = self->fbx_node->EvaluateGlobalTransform(time);
	FTransform transform;
	FbxVector4 mt = matrix.GetT();
	FbxQuaternion mq = matrix.GetQ();
	FbxVector4 ms = matrix.GetS();
	transform.SetTranslation(FVector(mt[0], -mt[1], mt[2]));
	transform.SetRotation(FQuat(mq[0], -mq[1], mq[2], -mq[3]));
	transform.SetScale3D(FVector(ms[0], ms[1], ms[2]));
	return py_ue_new_ftransform(transform);
}

static PyMethodDef ue_PyFbxNode_methods[] = {
	{ "get_child_count", (PyCFunction)py_ue_fbx_node_get_child_count, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_fbx_node_get_name, METH_VARARGS, "" },
	{ "evaluate_local_transform", (PyCFunction)py_ue_fbx_node_evaluate_local_transform, METH_VARARGS, "" },
	{ "evaluate_global_transform", (PyCFunction)py_ue_fbx_node_evaluate_global_transform, METH_VARARGS, "" },
	{ "get_local_translation", (PyCFunction)py_ue_fbx_node_get_local_translation, METH_VARARGS, "" },
	{ "get_local_rotation", (PyCFunction)py_ue_fbx_node_get_local_rotation, METH_VARARGS, "" },
	{ "get_local_scaling", (PyCFunction)py_ue_fbx_node_get_local_scaling, METH_VARARGS, "" },
	{ "get_child", (PyCFunction)py_ue_fbx_node_get_child, METH_VARARGS, "" },
	{ "get_parent", (PyCFunction)py_ue_fbx_node_get_parent, METH_VARARGS, "" },
	{ "get_node_attribute", (PyCFunction)py_ue_fbx_node_get_node_attribute, METH_VARARGS, "" },
	{ "get_node_attribute_count", (PyCFunction)py_ue_fbx_node_get_node_attribute_count, METH_VARARGS, "" },
	{ "get_node_attribute_by_index", (PyCFunction)py_ue_fbx_node_get_node_attribute_by_index, METH_VARARGS, "" },
	{ "get_mesh", (PyCFunction)py_ue_fbx_node_get_mesh, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxNodeType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxNode", /* tp_name */
	sizeof(ue_PyFbxNode),    /* tp_basicsize */
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
	"Unreal Engine FbxNode", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxNode_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_node_init(ue_PyFbxNode *self, PyObject * args)
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

	self->fbx_node = FbxNode::Create(py_fbx_manager->fbx_manager, name);
	return 0;
}

void ue_python_init_fbx_node(PyObject *ue_module)
{
	ue_PyFbxNodeType.tp_new = PyType_GenericNew;;
	ue_PyFbxNodeType.tp_init = (initproc)py_ue_fbx_node_init;
	if (PyType_Ready(&ue_PyFbxNodeType) < 0)
		return;

	Py_INCREF(&ue_PyFbxNodeType);
	PyModule_AddObject(ue_module, "FbxNode", (PyObject *)&ue_PyFbxNodeType);
}

PyObject *py_ue_new_fbx_node(FbxNode *fbx_node)
{
	ue_PyFbxNode *ret = (ue_PyFbxNode *)PyObject_New(ue_PyFbxNode, &ue_PyFbxNodeType);
	ret->fbx_node = fbx_node;
	return (PyObject *)ret;
}

ue_PyFbxNode *py_ue_is_fbx_node(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFbxNodeType))
		return nullptr;
	return (ue_PyFbxNode *)obj;
}

#endif
#endif
