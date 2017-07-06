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

PyObject *py_ue_skeleton_get_parent_index(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int index;
	if (!PyArg_ParseTuple(args, "i:skeleton_get_parent_index", &index))
		return nullptr;

	USkeleton *skeleton = ue_py_check_type<USkeleton>(self);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeleton");

	if (!skeleton->GetReferenceSkeleton().IsValidIndex(index))
		return PyErr_Format(PyExc_Exception, "invalid bone index");

	return PyLong_FromLong(skeleton->GetReferenceSkeleton().GetParentIndex(index));
}

PyObject *py_ue_skeleton_bones_get_num(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	USkeleton *skeleton = ue_py_check_type<USkeleton>(self);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeleton");

	return PyLong_FromLong(skeleton->GetReferenceSkeleton().GetNum());
}

PyObject *py_ue_skeleton_get_bone_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int index;
	if (!PyArg_ParseTuple(args, "i:skeleton_get_bone_name", &index))
		return nullptr;

	USkeleton *skeleton = ue_py_check_type<USkeleton>(self);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeleton");

	if (!skeleton->GetReferenceSkeleton().IsValidIndex(index))
		return PyErr_Format(PyExc_Exception, "invalid bone index");

	return PyUnicode_FromString(TCHAR_TO_UTF8(*skeleton->GetReferenceSkeleton().GetBoneName(index).ToString()));
}

PyObject *py_ue_skeleton_find_bone_index(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:skeleton_find_bone_index", &name))
		return nullptr;

	USkeleton *skeleton = ue_py_check_type<USkeleton>(self);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeleton");

	int32 index = skeleton->GetReferenceSkeleton().FindBoneIndex(FName(UTF8_TO_TCHAR(name)));
	if (!skeleton->GetReferenceSkeleton().IsValidIndex(index))
		return PyErr_Format(PyExc_Exception, "unable to find bone");

	return PyLong_FromLong(index);
}

PyObject *py_ue_skeleton_get_ref_bone_pose(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int index;
	if (!PyArg_ParseTuple(args, "i:skeleton_get_ref_bone_pose", &index))
		return nullptr;

	USkeleton *skeleton = ue_py_check_type<USkeleton>(self);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeleton");

	if (!skeleton->GetReferenceSkeleton().IsValidIndex(index))
		return PyErr_Format(PyExc_Exception, "invalid bone index");

	return py_ue_new_ftransform(skeleton->GetReferenceSkeleton().GetRefBonePose()[index]);
}

PyObject *py_ue_skeleton_add_bone(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *name;
	int parent_index;
	PyObject *py_transform;
	if (!PyArg_ParseTuple(args, "siO:skeleton_add_bone", &name, &parent_index, &py_transform))
		return nullptr;

	USkeleton *skeleton = ue_py_check_type<USkeleton>(self);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeleton");

	skeleton->PreEditChange(nullptr);

	{
		const FReferenceSkeleton &ref = skeleton->GetReferenceSkeleton();
		// horrible hack to modify the skeleton in place
		FReferenceSkeletonModifier modifier((FReferenceSkeleton &)ref, skeleton);

		TCHAR *bone_name = UTF8_TO_TCHAR(name);

		modifier.Add(FMeshBoneInfo(FName(bone_name), FString(bone_name), parent_index), FTransform());
	}

	skeleton->PostEditChange();
	skeleton->MarkPackageDirty();

	Py_RETURN_NONE;
}