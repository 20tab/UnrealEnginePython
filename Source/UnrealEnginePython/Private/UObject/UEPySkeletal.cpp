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

PyObject *py_ue_set_skeletal_mesh(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_skeletal_mesh;
	PyObject *py_reinit_pose = nullptr;

	if (!PyArg_ParseTuple(args, "O|O:set_skeletal_mesh", &py_skeletal_mesh, &py_reinit_pose))
		return nullptr;

	USkinnedMeshComponent *component = ue_py_check_type<USkinnedMeshComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMeshComponent");


	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(py_skeletal_mesh);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "argument is not a USkeletalMesh");

	bool reinit_pose = true;

	if (py_reinit_pose && !PyObject_IsTrue(py_reinit_pose))
		reinit_pose = false;

	component->SetSkeletalMesh(mesh, reinit_pose);

	Py_RETURN_NONE;
}

