
#include "UEPyMaterial.h"

#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/MaterialGraph/MaterialGraph.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Editor/UnrealEd/Classes/MaterialGraph/MaterialGraphSchema.h"
#endif

#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Wrappers/UEPyFLinearColor.h"
#include "Wrappers/UEPyFVector.h"
#include "Engine/Texture.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/StaticMesh.h"

PyObject *py_ue_set_material_by_name(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *slot_name;
	PyObject *py_mat;
	if (!PyArg_ParseTuple(args, "sO:set_material_by_name", &slot_name, &py_mat))
	{
		return nullptr;
	}

	UPrimitiveComponent *primitive = ue_py_check_type<UPrimitiveComponent>(self);
	if (!primitive)
		return PyErr_Format(PyExc_Exception, "uobject is not a UPrimitiveComponent");

	UMaterialInterface *material = ue_py_check_type<UMaterialInterface>(py_mat);
	if (!material)
		return PyErr_Format(PyExc_Exception, "argument is not a UMaterialInterface");

	primitive->SetMaterialByName(FName(UTF8_TO_TCHAR(slot_name)), material);

	Py_RETURN_NONE;
}

PyObject *py_ue_set_material(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int slot;
	PyObject *py_mat;
	if (!PyArg_ParseTuple(args, "iO:set_material", &slot, &py_mat))
	{
		return nullptr;
	}

	UMaterialInterface *material = ue_py_check_type<UMaterialInterface>(py_mat);
	if (!material)
		return PyErr_Format(PyExc_Exception, "argument is not a UMaterialInterface");

#if ENGINE_MINOR_VERSION >= 20
#if WITH_EDITOR
	UStaticMesh *mesh = ue_py_check_type<UStaticMesh>(self);
	if (mesh)
	{
		mesh->SetMaterial(slot, material);
		Py_RETURN_NONE;
	}
#endif
#endif

	UPrimitiveComponent *primitive = ue_py_check_type<UPrimitiveComponent>(self);
	if (!primitive)
		return PyErr_Format(PyExc_Exception, "uobject is not a UPrimitiveComponent");


	primitive->SetMaterial(slot, material);

	Py_RETURN_NONE;
}

PyObject *py_ue_set_material_scalar_parameter(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *scalarName = nullptr;
	float scalarValue = 0;
	if (!PyArg_ParseTuple(args, "sf:set_material_scalar_parameter", &scalarName, &scalarValue))
	{
		return NULL;
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	bool valid = false;

#if WITH_EDITOR
	if (self->ue_object->IsA<UMaterialInstanceConstant>())
	{
		UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
		material_instance->SetScalarParameterValueEditorOnly(parameterName, scalarValue);
		valid = true;
	}
#endif

	if (self->ue_object->IsA<UMaterialInstanceDynamic>())
	{
		UMaterialInstanceDynamic *material_instance = (UMaterialInstanceDynamic *)self->ue_object;
		material_instance->SetScalarParameterValue(parameterName, scalarValue);
		valid = true;
	}

	if (!valid)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a MaterialInstance");
	}

	Py_RETURN_NONE;

}

PyObject *py_ue_set_material_static_switch_parameter(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *switchName = nullptr;
	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "sO:set_material_static_switch_parameter", &switchName, &py_bool))
	{
		return NULL;
	}

	FName parameterName(UTF8_TO_TCHAR(switchName));

	bool switchValue = false;
	if (PyObject_IsTrue(py_bool))
	{
		switchValue = true;
	}

	bool valid = false;

#if WITH_EDITOR
	if (self->ue_object->IsA<UMaterialInstance>())
	{
		UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;
		valid = true;
		FStaticParameterSet staticParameterSet = material_instance->GetStaticParameters();

		bool isExisting = false;
		for (auto& parameter : staticParameterSet.StaticSwitchParameters)
		{
#if ENGINE_MINOR_VERSION < 19
			if (parameter.bOverride && parameter.ParameterName == parameterName)
#else
			if (parameter.bOverride && parameter.ParameterInfo.Name == parameterName)
#endif
			{
				parameter.Value = switchValue;
				isExisting = true;
				break;
			}
		}

		if (!isExisting)
		{
			FStaticSwitchParameter SwitchParameter;
#if ENGINE_MINOR_VERSION < 19
			SwitchParameter.ParameterName = parameterName;
#else
			SwitchParameter.ParameterInfo.Name = parameterName;
#endif
			SwitchParameter.Value = switchValue;

			SwitchParameter.bOverride = true;
			staticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
		}


		material_instance->UpdateStaticPermutation(staticParameterSet);

	}


#endif

	if (!valid)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a MaterialInstance");
	}

	Py_RETURN_NONE;

}

PyObject *py_ue_get_material_scalar_parameter(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *scalarName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_scalar_parameter", &scalarName))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	float value = 0;

	material_instance->GetScalarParameterValue(parameterName, value);

	return PyFloat_FromDouble(value);

}

PyObject *py_ue_get_material_static_switch_parameter(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *switchName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_static_switch_parameter", &switchName))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(switchName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	bool value = false;

	FGuid guid;
	material_instance->GetStaticSwitchParameterValue(parameterName, value, guid);

	if (value)
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_set_material_vector_parameter(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	char *vectorName;
	PyObject *vectorValue = nullptr;
	if (!PyArg_ParseTuple(args, "sO:set_material_vector_parameter", &vectorName, &vectorValue))
	{
		return NULL;
	}

	FLinearColor vectorParameter(0, 0, 0, 0);

	ue_PyFLinearColor *py_vector = py_ue_is_flinearcolor(vectorValue);
	if (!py_vector)
	{
		ue_PyFVector *py_true_vector = py_ue_is_fvector(vectorValue);
		if (!py_true_vector)
		{
			return PyErr_Format(PyExc_Exception, "argument must be an FLinearColor or FVector");
		}
		else
		{
			vectorParameter.R = py_true_vector->vec.X;
			vectorParameter.G = py_true_vector->vec.Y;
			vectorParameter.B = py_true_vector->vec.Z;
			vectorParameter.A = 1;
		}
	}
	else
	{
		vectorParameter = py_vector->color;
	}

	FName parameterName(UTF8_TO_TCHAR(vectorName));

	bool valid = false;

#if WITH_EDITOR
	if (self->ue_object->IsA<UMaterialInstanceConstant>())
	{
		UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
		material_instance->SetVectorParameterValueEditorOnly(parameterName, vectorParameter);
		valid = true;
	}
#endif

	if (self->ue_object->IsA<UMaterialInstanceDynamic>())
	{
		UMaterialInstanceDynamic *material_instance = (UMaterialInstanceDynamic *)self->ue_object;
		material_instance->SetVectorParameterValue(parameterName, vectorParameter);
		valid = true;
	}

	if (!valid)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a MaterialInstance");
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_get_material_vector_parameter(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *scalarName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_vector_parameter", &scalarName))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	FLinearColor vec(0, 0, 0);

	material_instance->GetVectorParameterValue(parameterName, vec);

	return py_ue_new_flinearcolor(vec);
}


PyObject *py_ue_set_material_texture_parameter(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	char *textureName;
	PyObject *textureObject = nullptr;
	if (!PyArg_ParseTuple(args, "sO:set_texture_parameter", &textureName, &textureObject))
	{
		return nullptr;
	}

	UTexture *ue_texture = ue_py_check_type<UTexture>(textureObject);
	if (!ue_texture)
		return PyErr_Format(PyExc_Exception, "uobject is not a UTexture");

	FName parameterName(UTF8_TO_TCHAR(textureName));

	bool valid = false;

#if WITH_EDITOR
	if (self->ue_object->IsA<UMaterialInstanceConstant>())
	{
		UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
		material_instance->SetTextureParameterValueEditorOnly(parameterName, ue_texture);
		valid = true;
	}
#endif

	if (self->ue_object->IsA<UMaterialInstanceDynamic>())
	{
		UMaterialInstanceDynamic *material_instance = (UMaterialInstanceDynamic *)self->ue_object;
		material_instance->SetTextureParameterValue(parameterName, ue_texture);
		valid = true;
	}

	if (!valid)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a MaterialInstance");
	}

	Py_RETURN_NONE;
}

PyObject *py_ue_get_material_texture_parameter(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *scalarName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_texture_parameter", &scalarName))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	UTexture *texture = nullptr;

	if (!material_instance->GetTextureParameterValue(parameterName, texture))
	{
		Py_RETURN_NONE;
	}

	Py_RETURN_UOBJECT(texture);
}

PyObject *py_ue_create_material_instance_dynamic(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_material = nullptr;

	if (!PyArg_ParseTuple(args, "O:create_material_instance_dynamic", &py_material))
	{
		return nullptr;
	}

	UMaterialInterface *material_interface = ue_py_check_type<UMaterialInterface>(py_material);
	if (!material_interface)
		return PyErr_Format(PyExc_Exception, "argument is not a UMaterialInterface");

	UMaterialInstanceDynamic *material_dynamic = UMaterialInstanceDynamic::Create(material_interface, self->ue_object);

	Py_RETURN_UOBJECT(material_dynamic);
}



#if WITH_EDITOR
PyObject *py_ue_set_material_parent(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);


	PyObject *py_material = nullptr;

	if (!PyArg_ParseTuple(args, "O:set_parent", &py_material))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstanceConstant>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstanceConstant");
	}


	if (!ue_is_pyuobject(py_material))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_material;
	if (!py_obj->ue_object->IsA<UMaterialInterface>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInterface");


	UMaterialInterface *materialInterface = (UMaterialInterface *)py_obj->ue_object;
	UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
	material_instance->SetParentEditorOnly(materialInterface);
	material_instance->PostEditChange();

	Py_RETURN_NONE;

}

PyObject *py_ue_static_mesh_set_collision_for_lod(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index;
	int material_index;
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "iiO:static_mesh_set_collision_for_lod", &lod_index, &material_index, &py_bool))
	{
		return NULL;
	}


	if (!self->ue_object->IsA<UStaticMesh>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a StaticMesh");
	}

	UStaticMesh *mesh = (UStaticMesh *)self->ue_object;

	bool enabled = false;
	if (PyObject_IsTrue(py_bool))
	{
		enabled = true;
	}

#if ENGINE_MINOR_VERSION >= 23
	FMeshSectionInfo info = mesh->GetSectionInfoMap().Get(lod_index, material_index);
#else
	FMeshSectionInfo info = mesh->SectionInfoMap.Get(lod_index, material_index);
#endif
	info.bEnableCollision = enabled;
	mesh->SectionInfoMap.Set(lod_index, material_index, info);

	mesh->MarkPackageDirty();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_static_mesh_set_shadow_for_lod(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	int lod_index;
	int material_index;
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "iiO:static_mesh_set_shadow_for_lod", &lod_index, &material_index, &py_bool))
	{
		return NULL;
	}


	if (!self->ue_object->IsA<UStaticMesh>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a StaticMesh");
	}

	UStaticMesh *mesh = (UStaticMesh *)self->ue_object;

	bool enabled = false;
	if (PyObject_IsTrue(py_bool))
	{
		enabled = true;
	}

	FMeshSectionInfo info = mesh->SectionInfoMap.Get(lod_index, material_index);
	info.bCastShadow = enabled;
	mesh->SectionInfoMap.Set(lod_index, material_index, info);

	mesh->MarkPackageDirty();

	Py_RETURN_NONE;
}

PyObject *py_ue_get_material_graph(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UMaterial *material = ue_py_check_type<UMaterial>(self);
	if (!material)
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInterface");

	UMaterialGraph *graph = material->MaterialGraph;
	if (!graph)
	{
		graph = (UMaterialGraph *)FBlueprintEditorUtils::CreateNewGraph(material, NAME_None, UMaterialGraph::StaticClass(), UMaterialGraphSchema::StaticClass());
		material->MaterialGraph = graph;
	}
	if (!graph)
		return PyErr_Format(PyExc_Exception, "Unable to retrieve/allocate MaterialGraph");

	Py_RETURN_UOBJECT(graph);
}

#endif
