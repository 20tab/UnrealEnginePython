#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_get_anim_instance(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<USkeletalMeshComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMeshComponent");
	
	USkeletalMeshComponent *skeletal = (USkeletalMeshComponent *)self->ue_object;

	UAnimInstance *anim = skeletal->GetAnimInstance();
	if (!anim) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	ue_PyUObject *ret = ue_get_python_wrapper((UObject *)anim);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

