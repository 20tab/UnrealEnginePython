
#include "UEPyFbxObject.h"

#if ENGINE_MINOR_VERSION > 12
#if WITH_EDITOR

#include "UEPyFbx.h"

#include "Runtime/Engine/Classes/Curves/RichCurve.h"

static PyObject *py_ue_fbx_object_get_name(ue_PyFbxObject *self, PyObject *args)
{
	return PyUnicode_FromString(self->fbx_object->GetName());
}

static PyObject *py_ue_fbx_object_get_class_name(ue_PyFbxObject *self, PyObject *args)
{
	return PyUnicode_FromString(self->fbx_object->GetClassId().GetName());
}

static PyObject *py_ue_fbx_object_get_member_count(ue_PyFbxObject *self, PyObject *args)
{
	FbxCollection *fbx_collection = FbxCast<FbxCollection>(self->fbx_object);
	if (!fbx_collection)
		return PyErr_Format(PyExc_Exception, "unable to cast to FbxCollection");
	return PyLong_FromLong(fbx_collection->GetMemberCount());
}

static PyObject *py_ue_fbx_object_get_member(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxCollection *fbx_collection = FbxCast<FbxCollection>(self->fbx_object);
	if (!fbx_collection)
		return PyErr_Format(PyExc_Exception, "unable to cast to FbxCollection");
	FbxObject *fbx_object = fbx_collection->GetMember(index);
	if (!fbx_object)
		return PyErr_Format(PyExc_Exception, "unable to find FbxObject with index %d", index);
	return py_ue_new_fbx_object(fbx_collection->GetMember(index));
}

static PyObject *py_ue_fbx_object_get_next_property(ue_PyFbxObject *self, PyObject *args)
{
	PyObject *py_object;
	if (!PyArg_ParseTuple(args, "O", &py_object))
	{
		return nullptr;
	}

	ue_PyFbxProperty *py_fbx_property = py_ue_is_fbx_property(py_object);
	if (!py_fbx_property)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a FbxProperty");
	}

	FbxProperty fbx_property = self->fbx_object->GetNextProperty(py_fbx_property->fbx_property);
	if (!fbx_property.IsValid())
		Py_RETURN_NONE;
	return py_ue_new_fbx_property(fbx_property);
}

static PyObject *py_ue_fbx_object_get_first_property(ue_PyFbxObject *self, PyObject *args)
{
	FbxProperty fbx_property = self->fbx_object->GetFirstProperty();
	if (!fbx_property.IsValid())
		Py_RETURN_NONE;
	return py_ue_new_fbx_property(fbx_property);
}

static PyObject *py_ue_fbx_object_get_channels_count(ue_PyFbxObject *self, PyObject *args)
{
	FbxAnimCurveNode *fbx_anim_curve_node = FbxCast<FbxAnimCurveNode>(self->fbx_object);
	if (!fbx_anim_curve_node)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurveNode");
	return PyLong_FromLong(fbx_anim_curve_node->GetChannelsCount());
}

static PyObject *py_ue_fbx_object_to_node(ue_PyFbxObject *self, PyObject *args)
{
	FbxNode *fbx_node = FbxCast<FbxNode>(self->fbx_object);
	if (!fbx_node)
		return PyErr_Format(PyExc_Exception, "object is not a FbxNode");
	return py_ue_new_fbx_node(fbx_node);
}

static PyObject *py_ue_fbx_object_get_channel_name(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxAnimCurveNode *fbx_anim_curve_node = FbxCast<FbxAnimCurveNode>(self->fbx_object);
	if (!fbx_anim_curve_node)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurveNode");
	return PyUnicode_FromString(fbx_anim_curve_node->GetChannelName(index));
}

static PyObject *py_ue_fbx_object_get_curve_count(ue_PyFbxObject *self, PyObject *args)
{
	int channel;
	if (!PyArg_ParseTuple(args, "i", &channel))
	{
		return nullptr;
	}
	FbxAnimCurveNode *fbx_anim_curve_node = FbxCast<FbxAnimCurveNode>(self->fbx_object);
	if (!fbx_anim_curve_node)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurveNode");
	return PyLong_FromLong(fbx_anim_curve_node->GetCurveCount(channel));
}

static PyObject *py_ue_fbx_object_get_curve(ue_PyFbxObject *self, PyObject *args)
{
	int channel;
	int index = 0;
	if (!PyArg_ParseTuple(args, "i|i:get_curve", &channel, &index))
	{
		return nullptr;
	}
	FbxAnimCurveNode *fbx_anim_curve_node = FbxCast<FbxAnimCurveNode>(self->fbx_object);
	if (!fbx_anim_curve_node)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurveNode");
	FbxAnimCurve *fbx_anim_curve = fbx_anim_curve_node->GetCurve(channel, index);
	if (!fbx_anim_curve)
		Py_RETURN_NONE;
	return py_ue_new_fbx_object(fbx_anim_curve_node->GetCurve(channel, index));
}

static PyObject *py_ue_fbx_object_key_get_count(ue_PyFbxObject *self, PyObject *args)
{
	FbxAnimCurve *fbx_anim_curve = FbxCast<FbxAnimCurve>(self->fbx_object);
	if (!fbx_anim_curve)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurve");
	return PyLong_FromLong(fbx_anim_curve->KeyGetCount());
}

static PyObject *py_ue_fbx_object_key_get_value(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxAnimCurve *fbx_anim_curve = FbxCast<FbxAnimCurve>(self->fbx_object);
	if (!fbx_anim_curve)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurve");
	return PyFloat_FromDouble(fbx_anim_curve->KeyGetValue(index));
}

static PyObject *py_ue_fbx_object_key_get_seconds(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxAnimCurve *fbx_anim_curve = FbxCast<FbxAnimCurve>(self->fbx_object);
	if (!fbx_anim_curve)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurve");
	return PyFloat_FromDouble(fbx_anim_curve->KeyGetTime(index).GetSecondDouble());
}

static PyObject *py_ue_fbx_object_key_get_left_tangent(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxAnimCurve *fbx_anim_curve = FbxCast<FbxAnimCurve>(self->fbx_object);
	if (!fbx_anim_curve)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurve");
	return PyFloat_FromDouble(fbx_anim_curve->KeyGetLeftDerivative(index));
}

static PyObject *py_ue_fbx_object_key_get_right_tangent(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxAnimCurve *fbx_anim_curve = FbxCast<FbxAnimCurve>(self->fbx_object);
	if (!fbx_anim_curve)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurve");
	return PyFloat_FromDouble(fbx_anim_curve->KeyGetRightDerivative(index));
}

static PyObject *py_ue_fbx_object_key_get_interp_mode(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxAnimCurve *fbx_anim_curve = FbxCast<FbxAnimCurve>(self->fbx_object);
	if (!fbx_anim_curve)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurve");

	ERichCurveInterpMode Mode = RCIM_Linear;
	// Convert the interpolation type from FBX to Unreal.
	switch (fbx_anim_curve->KeyGetInterpolation(index))
	{
	case FbxAnimCurveDef::eInterpolationCubic:
		Mode = RCIM_Cubic;
		break;

	case FbxAnimCurveDef::eInterpolationConstant:
		if (fbx_anim_curve->KeyGetTangentMode(index) != (FbxAnimCurveDef::ETangentMode)FbxAnimCurveDef::eConstantStandard)
		{
			// warning not support
			;
		}
		Mode = RCIM_Constant;
		break;

	case FbxAnimCurveDef::eInterpolationLinear:
		Mode = RCIM_Linear;
		break;
	}

	return PyLong_FromUnsignedLong(uint64(Mode));
}

static PyObject *py_ue_fbx_object_key_get_tangent_mode(ue_PyFbxObject *self, PyObject *args)
{
	int index;
	if (!PyArg_ParseTuple(args, "i", &index))
	{
		return nullptr;
	}
	FbxAnimCurve *fbx_anim_curve = FbxCast<FbxAnimCurve>(self->fbx_object);
	if (!fbx_anim_curve)
		return PyErr_Format(PyExc_Exception, "object is not a FbxAnimCurve");

	ERichCurveTangentMode Mode = RCTM_Auto;
	// Convert the interpolation type from FBX to Unreal.
	if (fbx_anim_curve->KeyGetInterpolation(index) ==
		FbxAnimCurveDef::eInterpolationCubic)
	{
		switch (fbx_anim_curve->KeyGetTangentMode(index))
		{
			// Auto tangents will now be imported as user tangents to allow the
			// user to modify them without inadvertently resetting other tangents
			// 				case KFbxAnimCurveDef::eTANGENT_AUTO:
			// 					if ((KFbxAnimCurveDef::eTANGENT_GENERIC_CLAMP & FbxKey.GetTangentMode(true)))
			// 					{
			// 						Mode = CIM_CurveAutoClamped;
			// 					}
			// 					else
			// 					{
			// 						Mode = CIM_CurveAuto;
			// 					}
			// 					break;
		case FbxAnimCurveDef::eTangentBreak:
			Mode = RCTM_Break;
			break;
		case FbxAnimCurveDef::eTangentAuto:
			Mode = RCTM_Auto;
			break;
		case FbxAnimCurveDef::eTangentUser:
		case FbxAnimCurveDef::eTangentTCB:
			Mode = RCTM_User;
			break;
		default:
			break;
		}
	}

	return PyLong_FromUnsignedLong(uint64(Mode));
}

static PyMethodDef ue_PyFbxObject_methods[] = {
	{ "get_member_count", (PyCFunction)py_ue_fbx_object_get_member_count, METH_VARARGS, "" },
	{ "get_member", (PyCFunction)py_ue_fbx_object_get_member, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_fbx_object_get_name, METH_VARARGS, "" },
	{ "to_node", (PyCFunction)py_ue_fbx_object_to_node, METH_VARARGS, "" },
	{ "get_class_name", (PyCFunction)py_ue_fbx_object_get_class_name, METH_VARARGS, "" },
	{ "get_first_property", (PyCFunction)py_ue_fbx_object_get_first_property, METH_VARARGS, "" },
	{ "get_next_property", (PyCFunction)py_ue_fbx_object_get_next_property, METH_VARARGS, "" },
	{ "get_channels_count", (PyCFunction)py_ue_fbx_object_get_channels_count, METH_VARARGS, "" },
	{ "get_channel_name", (PyCFunction)py_ue_fbx_object_get_channel_name, METH_VARARGS, "" },
	{ "get_curve_count", (PyCFunction)py_ue_fbx_object_get_curve_count, METH_VARARGS, "" },
	{ "get_curve", (PyCFunction)py_ue_fbx_object_get_curve, METH_VARARGS, "" },
	{ "key_get_count", (PyCFunction)py_ue_fbx_object_key_get_count, METH_VARARGS, "" },
	{ "key_get_value", (PyCFunction)py_ue_fbx_object_key_get_value, METH_VARARGS, "" },
	{ "key_get_seconds", (PyCFunction)py_ue_fbx_object_key_get_seconds, METH_VARARGS, "" },
	{ "key_get_left_tangent", (PyCFunction)py_ue_fbx_object_key_get_left_tangent, METH_VARARGS, "" },
	{ "key_get_right_tangent", (PyCFunction)py_ue_fbx_object_key_get_right_tangent, METH_VARARGS, "" },
	{ "key_get_interp_mode", (PyCFunction)py_ue_fbx_object_key_get_interp_mode, METH_VARARGS, "" },
	{ "key_get_tangent_mode", (PyCFunction)py_ue_fbx_object_key_get_tangent_mode, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxObject", /* tp_name */
	sizeof(ue_PyFbxObject),    /* tp_basicsize */
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
	"Unreal Engine FbxObject", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxObject_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_object_init(ue_PyFbxObject *self, PyObject * args)
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

	self->fbx_object = FbxObject::Create(py_fbx_manager->fbx_manager, name);
	return 0;
}

void ue_python_init_fbx_object(PyObject *ue_module)
{
	ue_PyFbxObjectType.tp_new = PyType_GenericNew;;
	ue_PyFbxObjectType.tp_init = (initproc)py_ue_fbx_object_init;
	if (PyType_Ready(&ue_PyFbxObjectType) < 0)
		return;

	Py_INCREF(&ue_PyFbxObjectType);
	PyModule_AddObject(ue_module, "FbxObject", (PyObject *)&ue_PyFbxObjectType);
}

PyObject *py_ue_new_fbx_object(FbxObject *fbx_object)
{
	ue_PyFbxObject *ret = (ue_PyFbxObject *)PyObject_New(ue_PyFbxObject, &ue_PyFbxObjectType);
	ret->fbx_object = fbx_object;
	return (PyObject *)ret;
}

ue_PyFbxObject *py_ue_is_fbx_object(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFbxObjectType))
		return nullptr;
	return (ue_PyFbxObject *)obj;
}
#endif
#endif
