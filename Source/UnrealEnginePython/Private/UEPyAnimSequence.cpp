#include "UnrealEnginePythonPrivatePCH.h"
#include "Animation/AnimSequence.h"

PyObject *py_ue_anim_sequence_get_skeleton(ue_PyUObject * self, PyObject * args) {
	ue_py_check(self);

	if (!self->ue_object->IsA<UAnimSequence>())
		return PyErr_Format(PyExc_Exception, "UObject is not a UAnimSequence.");

	UAnimSequence *anim_seq = (UAnimSequence *)self->ue_object;
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
