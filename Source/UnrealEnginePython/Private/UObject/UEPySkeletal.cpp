#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/Engine/Public/ComponentReregisterContext.h"


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

#if ENGINE_MINOR_VERSION > 13
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

	ue_PyFTransform *transform = py_ue_is_ftransform(py_transform);
	if (!transform)
		return PyErr_Format(PyExc_Exception, "argument is not a FTransform");

#if WITH_EDITOR
	skeleton->PreEditChange(nullptr);
#endif

	{
		const FReferenceSkeleton &ref = skeleton->GetReferenceSkeleton();
		// horrible hack to modify the skeleton in place
		FReferenceSkeletonModifier modifier((FReferenceSkeleton &)ref, skeleton);

		TCHAR *bone_name = UTF8_TO_TCHAR(name);

		modifier.Add(FMeshBoneInfo(FName(bone_name), FString(bone_name), parent_index), transform->transform);
	}


#if WITH_EDITOR
	skeleton->PostEditChange();
#endif
	skeleton->MarkPackageDirty();

	Py_RETURN_NONE;
}
#endif

#if ENGINE_MINOR_VERSION > 12
PyObject *py_ue_skeletal_mesh_set_soft_vertices(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	PyObject *py_ss_vertex;
	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "O|ii:skeletal_mesh_set_soft_vertices", &py_ss_vertex, &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_iter = PyObject_GetIter(py_ss_vertex);
	if (!py_iter) {
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of FSoftSkinVertex");
	}

	TArray<FSoftSkinVertex> soft_vertices;

	while (PyObject *py_item = PyIter_Next(py_iter)) {
		ue_PyFSoftSkinVertex *ss_vertex = py_ue_is_fsoft_skin_vertex(py_item);
		if (!ss_vertex) {
			Py_DECREF(py_iter);
			return PyErr_Format(PyExc_Exception, "argument is not an iterable of FSoftSkinVertex");
		}
		soft_vertices.Add(ss_vertex->ss_vertex);
	}
	Py_DECREF(py_iter);

	// temporarily disable all USkinnedMeshComponent's
	TComponentReregisterContext<USkinnedMeshComponent> ReregisterContext;

	mesh->ReleaseResources();
	mesh->ReleaseResourcesFence.Wait();

	model.Sections[section_index].SoftVertices = soft_vertices;

	model.Sections[section_index].NumVertices = soft_vertices.Num();
	model.Sections[section_index].CalcMaxBoneInfluences();

	mesh->RefBasesInvMatrix.Empty();
	mesh->CalculateInvRefMatrices();

#if WITH_EDITOR
	mesh->PostEditChange();
#endif
	mesh->InitResources();
	mesh->MarkPackageDirty();

	Py_RETURN_NONE;

}
#endif

#if ENGINE_MINOR_VERSION > 12
PyObject *py_ue_skeletal_mesh_get_soft_vertices(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "|ii:skeletal_mesh_get_soft_vertices", &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0;i < model.Sections[section_index].SoftVertices.Num();i++) {
		PyList_Append(py_list, py_ue_new_fsoft_skin_vertex(model.Sections[section_index].SoftVertices[i]));
	}

	return py_list;
}
#endif

PyObject *py_ue_skeletal_mesh_set_skeleton(ue_PyUObject * self, PyObject * args) {
	ue_py_check(self);

	PyObject *py_skeleton;
	if (!PyArg_ParseTuple(args, "O:skeletal_mesh_set_skeleton", &py_skeleton))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "UObject is not a USkeletalMesh.");

	USkeleton *skeleton = ue_py_check_type<USkeleton>(py_skeleton);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "argument is not a USkeleton.");

	mesh->ReleaseResources();
	mesh->ReleaseResourcesFence.Wait();

	mesh->Skeleton = skeleton;

	mesh->RefSkeleton = skeleton->GetReferenceSkeleton();

	mesh->RefBasesInvMatrix.Empty();
	mesh->CalculateInvRefMatrices();

#if WITH_EDITOR
	mesh->PostEditChange();
#endif
	mesh->InitResources();
	mesh->MarkPackageDirty();

	Py_RETURN_NONE;
}

#if ENGINE_MINOR_VERSION > 12
PyObject *py_ue_skeletal_mesh_set_bone_map(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	PyObject *py_map;
	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "O|ii:skeletal_mesh_set_bone_map", &py_map, &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_iter = PyObject_GetIter(py_map);
	if (!py_iter) {
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
	}

	TArray<FBoneIndexType> bone_map;

	while (PyObject *py_item = PyIter_Next(py_iter)) {
		if (!PyNumber_Check(py_item)) {
			Py_DECREF(py_iter);
			return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
		}
		PyObject *py_num = PyNumber_Long(py_item);
		uint16 index = PyLong_AsUnsignedLong(py_num);
		Py_DECREF(py_num);
		bone_map.Add(index);
	}
	Py_DECREF(py_iter);

	// temporarily disable all USkinnedMeshComponent's
	TComponentReregisterContext<USkinnedMeshComponent> ReregisterContext;

	mesh->ReleaseResources();
	mesh->ReleaseResourcesFence.Wait();

	model.Sections[section_index].BoneMap = bone_map;

	mesh->RefBasesInvMatrix.Empty();
	mesh->CalculateInvRefMatrices();

#if WITH_EDITOR
	mesh->PostEditChange();
#endif
	mesh->InitResources();
	mesh->MarkPackageDirty();

	Py_RETURN_NONE;

}
#endif

#if ENGINE_MINOR_VERSION > 12
PyObject *py_ue_skeletal_mesh_get_bone_map(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "|ii:skeletal_mesh_get_bone_map", &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_list = PyList_New(0);

	for (uint16 index : model.Sections[section_index].BoneMap) {
		PyList_Append(py_list, PyLong_FromUnsignedLong(index));
	}

	return py_list;
}
#endif

PyObject *py_ue_skeletal_mesh_get_active_bone_indices(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_get_active_bone_indices", &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	PyObject *py_list = PyList_New(0);

	for (uint16 index : model.ActiveBoneIndices) {
		PyList_Append(py_list, PyLong_FromUnsignedLong(index));
	}

	return py_list;
}

PyObject *py_ue_skeletal_mesh_set_active_bone_indices(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	PyObject *py_map;
	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "O|i:skeletal_mesh_set_active_bone_indices", &py_map, &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	PyObject *py_iter = PyObject_GetIter(py_map);
	if (!py_iter) {
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
	}

	TArray<FBoneIndexType> active_indices;

	while (PyObject *py_item = PyIter_Next(py_iter)) {
		if (!PyNumber_Check(py_item)) {
			Py_DECREF(py_iter);
			return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
		}
		PyObject *py_num = PyNumber_Long(py_item);
		uint16 index = PyLong_AsUnsignedLong(py_num);
		Py_DECREF(py_num);
		active_indices.Add(index);
	}
	Py_DECREF(py_iter);

	// temporarily disable all USkinnedMeshComponent's
	TComponentReregisterContext<USkinnedMeshComponent> ReregisterContext;

	mesh->ReleaseResources();
	mesh->ReleaseResourcesFence.Wait();

	model.ActiveBoneIndices = active_indices;
	model.ActiveBoneIndices.Sort();

	mesh->RefBasesInvMatrix.Empty();
	mesh->CalculateInvRefMatrices();

#if WITH_EDITOR
	mesh->PostEditChange();
#endif
	mesh->InitResources();
	mesh->MarkPackageDirty();

	Py_RETURN_NONE;

}

PyObject *py_ue_skeletal_mesh_get_required_bones(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_get_required_bones", &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	PyObject *py_list = PyList_New(0);

	for (uint16 index : model.RequiredBones) {
		PyList_Append(py_list, PyLong_FromUnsignedLong(index));
	}

	return py_list;
}

PyObject *py_ue_skeletal_mesh_set_required_bones(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	PyObject *py_map;
	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "O|i:skeletal_mesh_set_required_bones", &py_map, &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

	FSkeletalMeshResource *resource = mesh->GetImportedResource();

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	FStaticLODModel &model = resource->LODModels[lod_index];

	PyObject *py_iter = PyObject_GetIter(py_map);
	if (!py_iter) {
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
	}

	TArray<FBoneIndexType> required_bones;

	while (PyObject *py_item = PyIter_Next(py_iter)) {
		if (!PyNumber_Check(py_item)) {
			Py_DECREF(py_iter);
			return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
		}
		PyObject *py_num = PyNumber_Long(py_item);
		uint16 index = PyLong_AsUnsignedLong(py_num);
		Py_DECREF(py_num);
		required_bones.Add(index);
	}
	Py_DECREF(py_iter);

	// temporarily disable all USkinnedMeshComponent's
	TComponentReregisterContext<USkinnedMeshComponent> ReregisterContext;

	mesh->ReleaseResources();
	mesh->ReleaseResourcesFence.Wait();

	model.RequiredBones = required_bones;
	model.RequiredBones.Sort();

	mesh->RefBasesInvMatrix.Empty();
	mesh->CalculateInvRefMatrices();

#if WITH_EDITOR
	mesh->PostEditChange();
#endif
	mesh->InitResources();
	mesh->MarkPackageDirty();

	Py_RETURN_NONE;

}