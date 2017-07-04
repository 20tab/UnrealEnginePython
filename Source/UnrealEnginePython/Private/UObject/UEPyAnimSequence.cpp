#include "UnrealEnginePythonPrivatePCH.h"
#include "Animation/AnimSequence.h"
#include "Animation/BlendSpaceBase.h"

PyObject *py_ue_anim_sequence_get_skeleton(ue_PyUObject * self, PyObject * args) {
	ue_py_check(self);

	UAnimSequence *anim_seq = ue_py_check_type<UAnimSequence>(self);
	if (!anim_seq)
		return PyErr_Format(PyExc_Exception, "UObject is not a UAnimSequence.");

	USkeleton *skeleton = anim_seq->GetSkeleton();
	if (!skeleton) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	ue_PyUObject *ret = ue_get_python_wrapper((UObject *)skeleton);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}


PyObject *py_ue_anim_sequence_get_raw_animation_data(ue_PyUObject * self, PyObject * args) {
	ue_py_check(self);

	UAnimSequence *anim_seq = ue_py_check_type<UAnimSequence>(self);
	if (!anim_seq)
		return PyErr_Format(PyExc_Exception, "UObject is not a UAnimSequence.");

	PyObject *py_list = PyList_New(0);

	for (FRawAnimSequenceTrack rast : anim_seq->GetRawAnimationData()) {
		PyObject *py_item = py_ue_new_fraw_anim_sequence_track(rast);
		PyList_Append(py_list, py_item);
		Py_DECREF(py_item);
	}

	return py_list;
}

PyObject *py_ue_anim_sequence_get_raw_animation_track(ue_PyUObject * self, PyObject * args) {
	ue_py_check(self);

	int index;
	if (!PyArg_ParseTuple(args, "i:get_raw_animation_track", &index))
		return nullptr;

	UAnimSequence *anim_seq = ue_py_check_type<UAnimSequence>(self);
	if (!anim_seq)
		return PyErr_Format(PyExc_Exception, "UObject is not a UAnimSequence.");

	if (index < 0 || index >= anim_seq->GetAnimationTrackNames().Num())
		return PyErr_Format(PyExc_Exception, "invalid track index %d", index);

	return py_ue_new_fraw_anim_sequence_track(anim_seq->GetRawAnimationTrack(index));
}
