#include "UEPySkeletal.h"

#include "Runtime/Engine/Public/ComponentReregisterContext.h"
#if WITH_EDITOR
#include "Developer/MeshUtilities/Public/MeshUtilities.h"
#include "Wrappers/UEPyFMorphTargetDelta.h"
#include "Wrappers/UEPyFSoftSkinVertex.h"
#if ENGINE_MINOR_VERSION > 20
#include "Runtime/Engine/Public/Rendering/SkeletalMeshLODImporterData.h"
#endif
#if ENGINE_MINOR_VERSION > 18
#include "Runtime/Engine/Public/Rendering/SkeletalMeshModel.h"
#endif
#endif

#include "Animation/AnimInstance.h"


PyObject *py_ue_get_anim_instance(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<USkeletalMeshComponent>())
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMeshComponent");

	USkeletalMeshComponent *skeletal = (USkeletalMeshComponent *)self->ue_object;

	UAnimInstance *anim = skeletal->GetAnimInstance();
	if (!anim)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	Py_RETURN_UOBJECT((UObject *)anim);
}

PyObject *py_ue_set_skeletal_mesh(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_skeleton_get_parent_index(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_skeleton_bones_get_num(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	USkeleton *skeleton = ue_py_check_type<USkeleton>(self);
	if (!skeleton)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeleton");

	return PyLong_FromLong(skeleton->GetReferenceSkeleton().GetNum());
}

PyObject *py_ue_skeleton_get_bone_name(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_skeleton_find_bone_index(ue_PyUObject *self, PyObject * args)
{

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

PyObject *py_ue_skeleton_get_ref_bone_pose(ue_PyUObject *self, PyObject * args)
{

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
PyObject *py_ue_skeleton_add_bone(ue_PyUObject *self, PyObject * args)
{

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

	if (skeleton->GetReferenceSkeleton().FindBoneIndex(FName(UTF8_TO_TCHAR(name))) > -1)
	{
		return PyErr_Format(PyExc_Exception, "bone %s already exists", name);
	}

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

	return PyLong_FromLong(skeleton->GetReferenceSkeleton().FindBoneIndex(FName(UTF8_TO_TCHAR(name))));
}
#endif

#if WITH_EDITOR

#if ENGINE_MINOR_VERSION > 12
PyObject *py_ue_skeletal_mesh_set_soft_vertices(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	PyObject *py_ss_vertex;
	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "O|ii:skeletal_mesh_set_soft_vertices", &py_ss_vertex, &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_iter = PyObject_GetIter(py_ss_vertex);
	if (!py_iter)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of FSoftSkinVertex");
	}

	TArray<FSoftSkinVertex> soft_vertices;

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		ue_PyFSoftSkinVertex *ss_vertex = py_ue_is_fsoft_skin_vertex(py_item);
		if (!ss_vertex)
		{
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
PyObject *py_ue_skeletal_mesh_get_soft_vertices(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "|ii:skeletal_mesh_get_soft_vertices", &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_list = PyList_New(0);

	for (int32 i = 0; i < model.Sections[section_index].SoftVertices.Num(); i++)
	{
		PyList_Append(py_list, py_ue_new_fsoft_skin_vertex(model.Sections[section_index].SoftVertices[i]));
	}

	return py_list;
}
#endif

PyObject *py_ue_skeletal_mesh_get_lod(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int lod_index = 0;

	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_get_lod", &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif


	PyObject *py_list = PyList_New(0);

	TArray<uint32> indices;
#if ENGINE_MINOR_VERSION > 18
	indices = model.IndexBuffer;
#else
	model.MultiSizeIndexContainer.GetIndexBuffer(indices);
#endif

	for (int32 index = 0; index < indices.Num(); index++)
	{
		int32 section_index;
		int32 vertex_index;
#if ENGINE_MINOR_VERSION > 18
		model.GetSectionFromVertexIndex(indices[index], section_index, vertex_index);
#else
		bool has_extra_influences;
		model.GetSectionFromVertexIndex(indices[index], section_index, vertex_index, has_extra_influences);
#endif
		FSoftSkinVertex ssv = model.Sections[section_index].SoftVertices[vertex_index];
		// fix bone id
		for (int32 i = 0; i < MAX_TOTAL_INFLUENCES; i++)
		{
			if (ssv.InfluenceBones[i] < model.Sections[section_index].BoneMap.Num())
			{
				ssv.InfluenceBones[i] = model.Sections[section_index].BoneMap[ssv.InfluenceBones[i]];
			}
			else
			{
				UE_LOG(LogPython, Warning, TEXT("unable to retrieve bone mapping for index %d, forcing to 0"), ssv.InfluenceBones[i]);
				ssv.InfluenceBones[i] = 0;
			}
		}
		ue_PyFSoftSkinVertex *py_ss_vertex = (ue_PyFSoftSkinVertex *)py_ue_new_fsoft_skin_vertex(ssv);
		py_ss_vertex->material_index = section_index;
		PyList_Append(py_list, (PyObject *)py_ss_vertex);
	}

	return py_list;
}


PyObject *py_ue_skeletal_mesh_get_raw_indices(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int lod_index = 0;

	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_get_raw_indices", &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif


	PyObject *py_list = PyList_New(0);

	int32 *raw_indices = (int32 *)model.RawPointIndices.Lock(LOCK_READ_ONLY);
	int32 *indices = (int32 *)FMemory_Alloca(model.RawPointIndices.GetBulkDataSize());
	FMemory::Memcpy(indices, raw_indices, model.RawPointIndices.GetBulkDataSize());
	model.RawPointIndices.Unlock();

	for (int32 index = 0; index < model.RawPointIndices.GetBulkDataSize() / sizeof(int32); index++)
	{
		PyList_Append(py_list, PyLong_FromLong(indices[index]));
	}

	return py_list;
}

#endif

PyObject *py_ue_skeletal_mesh_set_skeleton(ue_PyUObject * self, PyObject * args)
{
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

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 12
PyObject *py_ue_skeletal_mesh_set_bone_map(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	PyObject *py_map;
	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "O|ii:skeletal_mesh_set_bone_map", &py_map, &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_iter = PyObject_GetIter(py_map);
	if (!py_iter)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
	}

	TArray<FBoneIndexType> bone_map;

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		if (!PyNumber_Check(py_item))
		{
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
PyObject *py_ue_skeletal_mesh_get_bone_map(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index = 0;
	int section_index = 0;
	if (!PyArg_ParseTuple(args, "|ii:skeletal_mesh_get_bone_map", &lod_index, &section_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	if (section_index < 0 || section_index >= model.Sections.Num())
		return PyErr_Format(PyExc_Exception, "invalid Section index, must be between 0 and %d", model.Sections.Num() - 1);

	PyObject *py_list = PyList_New(0);

	for (uint16 index : model.Sections[section_index].BoneMap)
	{
		PyList_Append(py_list, PyLong_FromUnsignedLong(index));
	}

	return py_list;
}
#endif

PyObject *py_ue_skeletal_mesh_get_active_bone_indices(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_get_active_bone_indices", &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	PyObject *py_list = PyList_New(0);

	for (uint16 index : model.ActiveBoneIndices)
	{
		PyList_Append(py_list, PyLong_FromUnsignedLong(index));
	}

	return py_list;
}

PyObject *py_ue_skeletal_mesh_set_active_bone_indices(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	PyObject *py_map;
	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "O|i:skeletal_mesh_set_active_bone_indices", &py_map, &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	PyObject *py_iter = PyObject_GetIter(py_map);
	if (!py_iter)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
	}

	TArray<FBoneIndexType> active_indices;

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		if (!PyNumber_Check(py_item))
		{
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

PyObject *py_ue_skeletal_mesh_get_required_bones(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_get_required_bones", &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	PyObject *py_list = PyList_New(0);

	for (uint16 index : model.RequiredBones)
	{
		PyList_Append(py_list, PyLong_FromUnsignedLong(index));
	}

	return py_list;
}

PyObject *py_ue_skeletal_mesh_set_required_bones(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	PyObject *py_map;
	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "O|i:skeletal_mesh_set_required_bones", &py_map, &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel &model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &model = resource->LODModels[lod_index];
#endif

	PyObject *py_iter = PyObject_GetIter(py_map);
	if (!py_iter)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of numbers");
	}

	TArray<FBoneIndexType> required_bones;

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		if (!PyNumber_Check(py_item))
		{
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
#endif

#if WITH_EDITOR
PyObject *py_ue_skeletal_mesh_lods_num(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	return PyLong_FromLong(resource->LODModels.Num());
}

PyObject *py_ue_skeletal_mesh_sections_num(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index = 0;
	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_sections_num", &lod_index))
		return nullptr;

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index >= resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num() - 1);

	return PyLong_FromLong(resource->LODModels[lod_index].Sections.Num());
}

PyObject *py_ue_skeletal_mesh_build_lod(ue_PyUObject *self, PyObject * args, PyObject * kwargs)
{
	ue_py_check(self);

	PyObject *py_ss_vertex;
	int lod_index = 0;

	PyObject *py_compute_normals = nullptr;
	PyObject *py_compute_tangents = nullptr;
	PyObject *py_use_mikk = nullptr;

	static char *kw_names[] = { (char *)"soft_vertices", (char *)"lod", (char *)"compute_normals", (char *)"compute_tangents", (char *)"use_mikk", nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iOOO:skeletal_mesh_build_lod", kw_names, &py_ss_vertex, &lod_index, &py_compute_normals, &py_compute_tangents, &py_use_mikk))
	{
		return nullptr;
	}

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a SkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index > resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num());

	mesh->PreEditChange(nullptr);

	if (lod_index == resource->LODModels.Num())
	{
#if ENGINE_MINOR_VERSION < 19
		resource->LODModels.Add(new FStaticLODModel());
#else
		resource->LODModels.Add(new FSkeletalMeshLODModel());
#endif

#if ENGINE_MINOR_VERSION < 20
		mesh->LODInfo.AddZeroed();
#else
		mesh->AddLODInfo();
#endif
	}
	else
	{
		// reinitialized already existent LOD
#if ENGINE_MINOR_VERSION < 19
		new(&resource->LODModels[lod_index]) FStaticLODModel();
#else
		new(&resource->LODModels[lod_index]) FSkeletalMeshLODModel();
#endif
	}

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel& LODModel = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel& LODModel = resource->LODModels[lod_index];
#endif

#if ENGINE_MINOR_VERSION < 20
	mesh->LODInfo[lod_index].LODHysteresis = 0.02;
#else
	mesh->GetLODInfo(lod_index)->LODHysteresis = 0.02;
#endif

	FSkeletalMeshOptimizationSettings settings;

#if ENGINE_MINOR_VERSION < 20
	mesh->LODInfo[lod_index].ReductionSettings = settings;
#else
	mesh->GetLODInfo(lod_index)->ReductionSettings = settings;
#endif


	LODModel.NumTexCoords = 1;

	IMeshUtilities & MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");

	PyObject *py_iter = PyObject_GetIter(py_ss_vertex);
	if (!py_iter)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of FSoftSkinVertex");
	}

	TArray<FSoftSkinVertex> soft_vertices;

	TArray<FVector> points;
#if ENGINE_MINOR_VERSION > 20
	TArray<SkeletalMeshImportData::FMeshWedge> wedges;
	TArray<SkeletalMeshImportData::FMeshFace> faces;
	TArray<SkeletalMeshImportData::FVertInfluence> influences;
#else
	TArray<FMeshWedge> wedges;
	TArray<FMeshFace> faces;
	TArray<FVertInfluence> influences;
#endif
	TArray<int32> points_to_map;

	TArray<FVector> tangentsX;
	TArray<FVector> tangentsY;
	TArray<FVector> tangentsZ;
	TArray<uint16> material_indices;
	TArray<uint32> smoothing_groups;

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		ue_PyFSoftSkinVertex *ss_vertex = py_ue_is_fsoft_skin_vertex(py_item);
		if (!ss_vertex)
		{
			Py_DECREF(py_iter);
			return PyErr_Format(PyExc_Exception, "argument is not an iterable of FSoftSkinVertex");
		}
		int32 vertex_index = points.Add(ss_vertex->ss_vertex.Position);

		points_to_map.Add(vertex_index);

#if ENGINE_MINOR_VERSION > 20
		SkeletalMeshImportData::FMeshWedge wedge;
#else
		FMeshWedge wedge;
#endif
		wedge.iVertex = vertex_index;
		wedge.Color = ss_vertex->ss_vertex.Color;
		for (int32 i = 0; i < MAX_TEXCOORDS; i++)
		{
			wedge.UVs[i] = ss_vertex->ss_vertex.UVs[i];
		}
		int32 wedge_index = wedges.Add(wedge);

		for (int32 i = 0; i < MAX_TOTAL_INFLUENCES; i++)
		{
#if ENGINE_MINOR_VERSION > 20
			SkeletalMeshImportData::FVertInfluence influence;
#else
			FVertInfluence influence;
#endif
			influence.VertIndex = wedge_index;
			influence.BoneIndex = ss_vertex->ss_vertex.InfluenceBones[i];
			influence.Weight = ss_vertex->ss_vertex.InfluenceWeights[i] / 255.f;
			influences.Add(influence);
		}

		tangentsX.Add(ss_vertex->ss_vertex.TangentX);
		tangentsY.Add(ss_vertex->ss_vertex.TangentY);
		tangentsZ.Add(ss_vertex->ss_vertex.TangentZ);

		material_indices.Add(ss_vertex->material_index);
		smoothing_groups.Add(ss_vertex->smoothing_group);
	}

	Py_DECREF(py_iter);

	if (wedges.Num() % 3 != 0)
		return PyErr_Format(PyExc_Exception, "invalid number of FSoftSkinVertex, must be a multiple of 3");

	for (int32 i = 0; i < wedges.Num(); i += 3)
	{
#if ENGINE_MINOR_VERSION > 20
		SkeletalMeshImportData::FMeshFace face;
#else
		FMeshFace face;
#endif
		face.iWedge[0] = i;
		face.iWedge[1] = i + 1;
		face.iWedge[2] = i + 2;

		face.MeshMaterialIndex = material_indices[i];
		face.SmoothingGroups = smoothing_groups[i];

		face.TangentX[0] = tangentsX[i];
		face.TangentX[1] = tangentsX[i + 1];
		face.TangentX[2] = tangentsX[i + 2];

		face.TangentY[0] = tangentsY[i];
		face.TangentY[1] = tangentsY[i + 1];
		face.TangentY[2] = tangentsY[i + 2];

		face.TangentZ[0] = tangentsZ[i];
		face.TangentZ[1] = tangentsZ[i + 1];
		face.TangentZ[2] = tangentsZ[i + 2];

		faces.Add(face);
	}

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel & lod_model = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel & lod_model = resource->LODModels[lod_index];
#endif

	IMeshUtilities::MeshBuildOptions build_settings;
	build_settings.bUseMikkTSpace = (py_use_mikk && PyObject_IsTrue(py_use_mikk));
	build_settings.bComputeNormals = (py_compute_normals && PyObject_IsTrue(py_compute_normals));
	build_settings.bComputeTangents = (py_compute_tangents && PyObject_IsTrue(py_compute_tangents));
	build_settings.bRemoveDegenerateTriangles = true;

	bool success = MeshUtilities.BuildSkeletalMesh(lod_model, mesh->RefSkeleton, influences, wedges, faces, points, points_to_map, build_settings);

	if (!success)
	{
		return PyErr_Format(PyExc_Exception, "unable to create new Skeletal LOD");
	}

#if ENGINE_MINOR_VERSION < 19
	for (int32 i = 0; i < lod_model.Sections.Num(); i++)
	{
		mesh->LODInfo[lod_index].TriangleSortSettings.AddZeroed();
	}
#endif

	mesh->CalculateRequiredBones(LODModel, mesh->RefSkeleton, nullptr);
	mesh->CalculateInvRefMatrices();

	mesh->Skeleton->RecreateBoneTree(mesh);
	mesh->Skeleton->SetPreviewMesh(mesh);

	// calculate bounds from points
	mesh->SetImportedBounds(FBoxSphereBounds(points.GetData(), points.Num()));

	mesh->Skeleton->PostEditChange();
	mesh->Skeleton->MarkPackageDirty();

	mesh->PostEditChange();
	mesh->MarkPackageDirty();

	Py_RETURN_NONE;
}


PyObject *py_ue_skeletal_mesh_register_morph_target(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	PyObject *py_morph;

	if (!PyArg_ParseTuple(args, "O:skeletal_mesh_register_morph_target", &py_morph))
	{
		return nullptr;
	}

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a SkeletalMesh");

	UMorphTarget *morph = ue_py_check_type<UMorphTarget>(py_morph);
	if (!morph)
		return PyErr_Format(PyExc_Exception, "argument is not a MorphTarget");

#if ENGINE_MINOR_VERSION > 16
	if (!morph->HasValidData())
		return PyErr_Format(PyExc_Exception, "the MorphTarget has no valid data");
#endif

	mesh->PreEditChange(nullptr);

	mesh->RegisterMorphTarget(morph);

	mesh->PostEditChange();

	mesh->MarkPackageDirty();

	Py_RETURN_NONE;
}

PyObject *py_ue_morph_target_populate_deltas(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	PyObject *py_deltas;
	int lod_index = 0;

	if (!PyArg_ParseTuple(args, "O|i:morph_target_populate_deltas", &py_deltas, &lod_index))
	{
		return nullptr;
	}

	UMorphTarget *morph = ue_py_check_type<UMorphTarget>(self);
	if (!morph)
		return PyErr_Format(PyExc_Exception, "uobject is not a MorphTarget");

	if (lod_index < 0)
		return PyErr_Format(PyExc_Exception, "invalid LOD index");

	PyObject *py_iter = PyObject_GetIter(py_deltas);
	if (!py_iter)
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of FMorphTargetDelta");

	TArray<FMorphTargetDelta> deltas;

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		ue_PyFMorphTargetDelta *py_delta = py_ue_is_fmorph_target_delta(py_item);
		if (!py_delta)
		{
			Py_DECREF(py_iter);
			return PyErr_Format(PyExc_Exception, "argument is not an iterable of FMorphTargetDelta");
		}
		deltas.Add(py_delta->morph_target_delta);
	}

	Py_DECREF(py_iter);

#if ENGINE_MINOR_VERSION < 19
	morph->PopulateDeltas(deltas, lod_index);
#else
	FSkeletalMeshModel *model = morph->BaseSkelMesh->GetImportedModel();
	morph->PopulateDeltas(deltas, lod_index, model->LODModels[lod_index].Sections);
#endif

#if ENGINE_MINOR_VERSION > 16
	if (morph->HasValidData())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
#else
	Py_RETURN_TRUE;
#endif
}

PyObject *py_ue_morph_target_get_deltas(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index = 0;

	if (!PyArg_ParseTuple(args, "|i:morph_target_get_deltas", &lod_index))
	{
		return nullptr;
	}

	UMorphTarget *morph = ue_py_check_type<UMorphTarget>(self);
	if (!morph)
		return PyErr_Format(PyExc_Exception, "uobject is not a MorphTarget");

	if (lod_index < 0 || lod_index > morph->MorphLODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index");

	PyObject *py_list = PyList_New(0);

	for (FMorphTargetDelta delta : morph->MorphLODModels[lod_index].Vertices)
	{
		PyList_Append(py_list, py_ue_new_fmorph_target_delta(delta));
	}

	return py_list;
}

PyObject *py_ue_skeletal_mesh_to_import_vertex_map(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index = 0;

	if (!PyArg_ParseTuple(args, "|i:skeletal_mesh_to_import_vertex_map", &lod_index))
	{
		return nullptr;
	}

	USkeletalMesh *mesh = ue_py_check_type<USkeletalMesh>(self);
	if (!mesh)
		return PyErr_Format(PyExc_Exception, "uobject is not a USkeletalMesh");

#if ENGINE_MINOR_VERSION < 19
	FSkeletalMeshResource *resource = mesh->GetImportedResource();
#else
	FSkeletalMeshModel *resource = mesh->GetImportedModel();
#endif

	if (lod_index < 0 || lod_index > resource->LODModels.Num())
		return PyErr_Format(PyExc_Exception, "invalid LOD index, must be between 0 and %d", resource->LODModels.Num());

#if ENGINE_MINOR_VERSION < 19
	FStaticLODModel& LODModel = resource->LODModels[lod_index];
#else
	FSkeletalMeshLODModel &LODModel = resource->LODModels[lod_index];
#endif

	PyObject *py_list = PyList_New(0);

	for (int32 value : LODModel.MeshToImportVertexMap)
	{
		PyList_Append(py_list, PyLong_FromLong(value));
	}

	return py_list;
}
#endif
