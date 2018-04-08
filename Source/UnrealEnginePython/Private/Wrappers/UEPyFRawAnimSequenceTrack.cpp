#include "UEPyFRawAnimSequenceTrack.h"

static PyObject *py_ue_fraw_anim_sequence_track_get_pos_keys(ue_PyFRawAnimSequenceTrack *self, void *closure)
{
	PyObject *py_list = PyList_New(0);
	for (FVector vec : self->raw_anim_sequence_track.PosKeys)
	{
		PyObject *py_vec = py_ue_new_fvector(vec);
		PyList_Append(py_list, py_vec);
		Py_DECREF(py_vec);
	}
	return py_list;
}

static PyObject *py_ue_fraw_anim_sequence_track_get_scale_keys(ue_PyFRawAnimSequenceTrack *self, void *closure)
{
	PyObject *py_list = PyList_New(0);
	for (FVector vec : self->raw_anim_sequence_track.ScaleKeys)
	{
		PyObject *py_vec = py_ue_new_fvector(vec);
		PyList_Append(py_list, py_vec);
		Py_DECREF(py_vec);
	}
	return py_list;
}

static PyObject *py_ue_fraw_anim_sequence_track_get_rot_keys(ue_PyFRawAnimSequenceTrack *self, void *closure)
{
	PyObject *py_list = PyList_New(0);
	for (FQuat quat : self->raw_anim_sequence_track.RotKeys)
	{
		PyObject *py_quat = py_ue_new_fquat(quat);
		PyList_Append(py_list, py_quat);
		Py_DECREF(py_quat);
	}
	return py_list;
}

static int py_ue_fraw_anim_sequence_track_set_pos_keys(ue_PyFRawAnimSequenceTrack *self, PyObject *value, void *closure)
{
	TArray<FVector> pos;
	if (value)
	{
		PyObject *py_iter = PyObject_GetIter(value);
		if (py_iter)
		{
			bool failed = false;
			while (PyObject *py_item = PyIter_Next(py_iter))
			{
				ue_PyFVector *py_vec = py_ue_is_fvector(py_item);
				if (!py_vec)
				{
					failed = true;
					break;
				}
				pos.Add(py_vec->vec);
			}
			Py_DECREF(py_iter);
			if (!failed)
			{
				self->raw_anim_sequence_track.PosKeys = pos;
				return 0;
			}
		}
	}
	PyErr_SetString(PyExc_TypeError, "value is not an iterable of FVector's");
	return -1;
}

static int py_ue_fraw_anim_sequence_track_set_scale_keys(ue_PyFRawAnimSequenceTrack *self, PyObject *value, void *closure)
{
	TArray<FVector> scale;
	if (value)
	{
		PyObject *py_iter = PyObject_GetIter(value);
		if (py_iter)
		{
			bool failed = false;
			while (PyObject *py_item = PyIter_Next(py_iter))
			{
				ue_PyFVector *py_vec = py_ue_is_fvector(py_item);
				if (!py_vec)
				{
					failed = true;
					break;
				}
				scale.Add(py_vec->vec);
			}
			Py_DECREF(py_iter);
			if (!failed)
			{
				self->raw_anim_sequence_track.ScaleKeys = scale;
				return 0;
			}
		}
	}
	PyErr_SetString(PyExc_TypeError, "value is not an iterable of FVector's");
	return -1;
}

static int py_ue_fraw_anim_sequence_track_set_rot_keys(ue_PyFRawAnimSequenceTrack *self, PyObject *value, void *closure)
{
	TArray<FQuat> rot;
	if (value)
	{
		PyObject *py_iter = PyObject_GetIter(value);
		if (py_iter)
		{
			bool failed = false;
			while (PyObject *py_item = PyIter_Next(py_iter))
			{
				ue_PyFQuat *py_quat = py_ue_is_fquat(py_item);
				if (!py_quat)
				{
					failed = true;
					break;
				}
				rot.Add(py_quat->quat);
			}
			Py_DECREF(py_iter);
			if (!failed)
			{
				self->raw_anim_sequence_track.RotKeys = rot;
				return 0;
			}
		}
	}
	PyErr_SetString(PyExc_TypeError, "value is not an iterable of FQuat's");
	return -1;
}


static PyGetSetDef ue_PyFRawAnimSequenceTrack_getseters[] = {
	{(char *) "pos_keys", (getter)py_ue_fraw_anim_sequence_track_get_pos_keys, (setter)py_ue_fraw_anim_sequence_track_set_pos_keys, (char *)"", NULL },
	{(char *) "rot_keys", (getter)py_ue_fraw_anim_sequence_track_get_rot_keys, (setter)py_ue_fraw_anim_sequence_track_set_rot_keys, (char *)"", NULL },
	{(char *) "scale_keys", (getter)py_ue_fraw_anim_sequence_track_get_scale_keys, (setter)py_ue_fraw_anim_sequence_track_set_scale_keys, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFRawAnimSequenceTrack_str(ue_PyFRawAnimSequenceTrack *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FRawAnimSequenceTrack {'pos': %d, 'rot': %d, 'scale': %d}>",
		self->raw_anim_sequence_track.PosKeys.Num(), self->raw_anim_sequence_track.RotKeys.Num(), self->raw_anim_sequence_track.ScaleKeys.Num());
}

static PyTypeObject ue_PyFRawAnimSequenceTrackType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FRawAnimSequenceTrack", /* tp_name */
	sizeof(ue_PyFRawAnimSequenceTrack), /* tp_basicsize */
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
	(reprfunc)ue_PyFRawAnimSequenceTrack_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
#endif
	"Unreal Engine FRawAnimSequenceTrack",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,             /* tp_methods */
	0,
	ue_PyFRawAnimSequenceTrack_getseters,
};



void ue_python_init_fraw_anim_sequence_track(PyObject *ue_module)
{
	ue_PyFRawAnimSequenceTrackType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFRawAnimSequenceTrackType) < 0)
		return;

	Py_INCREF(&ue_PyFRawAnimSequenceTrackType);
	PyModule_AddObject(ue_module, "FRawAnimSequenceTrack", (PyObject *)&ue_PyFRawAnimSequenceTrackType);
}

ue_PyFRawAnimSequenceTrack *py_ue_is_fraw_anim_sequence_track(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFRawAnimSequenceTrackType))
		return nullptr;
	return (ue_PyFRawAnimSequenceTrack *)obj;
}

PyObject *py_ue_new_fraw_anim_sequence_track(FRawAnimSequenceTrack raw_anim_sequence_track)
{
	ue_PyFRawAnimSequenceTrack *ret = (ue_PyFRawAnimSequenceTrack *)PyObject_New(ue_PyFRawAnimSequenceTrack, &ue_PyFRawAnimSequenceTrackType);
	new(&ret->raw_anim_sequence_track) FRawAnimSequenceTrack(raw_anim_sequence_track);
	return (PyObject *)ret;
}
