#include "UEPyFbxPose.h"

#if ENGINE_MINOR_VERSION > 12

#if WITH_EDITOR

#include "UEPyFbx.h"

static PyObject *py_ue_fbx_pose_get_count(ue_PyFbxPose *self, PyObject *args)
{
	return PyLong_FromLong(self->fbx_pose->GetCount());
}

static PyObject *py_ue_fbx_pose_get_name(ue_PyFbxPose *self, PyObject *args)
{
	return PyUnicode_FromString(self->fbx_pose->GetName());
}

static PyObject *py_ue_fbx_pose_is_bind_pose(ue_PyFbxPose *self, PyObject *args)
{
	if (self->fbx_pose->IsBindPose())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

static PyObject *py_ue_fbx_pose_is_rest_pose(ue_PyFbxPose *self, PyObject *args)
{
	if (self->fbx_pose->IsRestPose())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

static PyObject *py_ue_fbx_pose_get_node(ue_PyFbxPose *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxNode *fbx_node = self->fbx_pose->GetNode(index);
	if (!fbx_node)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve FbxNode at index %d", index);
	}
	return py_ue_new_fbx_node(fbx_node);
}

static PyObject *py_ue_fbx_pose_find(ue_PyFbxPose *self, PyObject *args)
{
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O", &py_obj))
	{
		return nullptr;
	}

	ue_PyFbxNode *py_node = py_ue_is_fbx_node(py_obj);
	if (!py_node)
		return PyErr_Format(PyExc_Exception, "argument is not a FbxNode");

	int index = self->fbx_pose->Find(py_node->fbx_node);
	if (index < 0)
		return PyErr_Format(PyExc_Exception, "unable to retrieve FbxNode index from FbxPose");

	return PyLong_FromLong(index);
}

static PyObject *py_ue_fbx_pose_get_transform(ue_PyFbxPose *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}

	FbxMatrix fbx_matrix = self->fbx_pose->GetMatrix(index);
	FbxAMatrix matrix = *(FbxAMatrix *)&fbx_matrix;
	FTransform transform;
	FbxVector4 mt = matrix.GetT();
	FbxQuaternion mq = matrix.GetQ();
	FbxVector4 ms = matrix.GetS();
	transform.SetTranslation(FVector(mt[0], -mt[1], mt[2]));
	transform.SetRotation(FQuat(mq[0], -mq[1], mq[2], -mq[3]));
	transform.SetScale3D(FVector(ms[0], ms[1], ms[2]));
	return py_ue_new_ftransform(transform);
}

static PyMethodDef ue_PyFbxPose_methods[] = {
	{ "get_count", (PyCFunction)py_ue_fbx_pose_get_count, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_fbx_pose_get_name, METH_VARARGS, "" },
	{ "get_node", (PyCFunction)py_ue_fbx_pose_get_node, METH_VARARGS, "" },
	{ "is_bind_pose", (PyCFunction)py_ue_fbx_pose_is_bind_pose, METH_VARARGS, "" },
	{ "is_rest_pose", (PyCFunction)py_ue_fbx_pose_is_rest_pose, METH_VARARGS, "" },
	{ "find", (PyCFunction)py_ue_fbx_pose_find, METH_VARARGS, "" },
	{ "get_transform", (PyCFunction)py_ue_fbx_pose_get_transform, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxPoseType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxPose", /* tp_name */
	sizeof(ue_PyFbxPose),    /* tp_basicsize */
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
	"Unreal Engine FbxPose", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxPose_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_pose_init(ue_PyFbxPose *self, PyObject * args)
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

	self->fbx_pose = FbxPose::Create(py_fbx_manager->fbx_manager, name);
	return 0;
}

void ue_python_init_fbx_pose(PyObject *ue_module)
{
	ue_PyFbxPoseType.tp_new = PyType_GenericNew;;
	ue_PyFbxPoseType.tp_init = (initproc)py_ue_fbx_pose_init;
	if (PyType_Ready(&ue_PyFbxPoseType) < 0)
		return;

	Py_INCREF(&ue_PyFbxPoseType);
	PyModule_AddObject(ue_module, "FbxPose", (PyObject *)&ue_PyFbxPoseType);
}

PyObject *py_ue_new_fbx_pose(FbxPose *fbx_pose)
{
	ue_PyFbxPose *ret = (ue_PyFbxPose *)PyObject_New(ue_PyFbxPose, &ue_PyFbxPoseType);
	ret->fbx_pose = fbx_pose;
	return (PyObject *)ret;
}

#endif
#endif
