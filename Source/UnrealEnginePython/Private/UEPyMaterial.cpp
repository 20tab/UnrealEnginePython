

#include "UnrealEnginePythonPrivatePCH.h"
#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/MaterialGraph/MaterialGraph.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Editor/UnrealEd/Classes/MaterialGraph/MaterialGraphSchema.h"
#endif

PyObject *py_ue_set_material_scalar_parameter(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *scalarName = nullptr;
	float scalarValue = 0;
	if (!PyArg_ParseTuple(args, "sf:set_material_scalar_parameter", &scalarName, &scalarValue)) {
		return NULL;
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	bool valid = false;

#if WITH_EDITOR
	if (self->ue_object->IsA<UMaterialInstanceConstant>()) {
		UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
		material_instance->SetScalarParameterValueEditorOnly(parameterName, scalarValue);
		valid = true;
	}
#endif

	if (self->ue_object->IsA<UMaterialInstanceDynamic>()) {
		UMaterialInstanceDynamic *material_instance = (UMaterialInstanceDynamic *)self->ue_object;
		material_instance->SetScalarParameterValue(parameterName, scalarValue);
		valid = true;
	}

	if (!valid) {
		return PyErr_Format(PyExc_Exception, "uobject is not a MaterialInstance");
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_get_material_scalar_parameter(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *scalarName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_scalar_parameter", &scalarName)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	float value = 0;

	material_instance->GetScalarParameterValue(parameterName, value);

	return PyFloat_FromDouble(value);

}

PyObject *py_ue_get_material_static_switch_parameter(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *switchName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_static_switch_parameter", &switchName)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(switchName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	bool value = false;

	FGuid guid;
	material_instance->GetStaticSwitchParameterValue(parameterName, value, guid);

	if (value) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_set_material_vector_parameter(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	char *vectorName;
	PyObject *vectorValue = nullptr;
	if (!PyArg_ParseTuple(args, "sO:set_material_vector_parameter", &vectorName, &vectorValue)) {
		return NULL;
	}

	FLinearColor vectorParameter(0, 0, 0, 0);

	ue_PyFLinearColor *py_vector = py_ue_is_flinearcolor(vectorValue);
	if (!py_vector) {
		ue_PyFVector *py_true_vector = py_ue_is_fvector(vectorValue);
		if (!py_true_vector) {
			return PyErr_Format(PyExc_Exception, "argument must be an FLinearColor or FVector");
		}
		else {
			vectorParameter.R = py_true_vector->vec.X;
			vectorParameter.G = py_true_vector->vec.Y;
			vectorParameter.B = py_true_vector->vec.Z;
			vectorParameter.A = 1;
		}
	}
	else {
		vectorParameter = py_vector->color;
	}

	FName parameterName(UTF8_TO_TCHAR(vectorName));

	bool valid = false;

#if WITH_EDITOR
	if (self->ue_object->IsA<UMaterialInstanceConstant>()) {
		UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
		material_instance->SetVectorParameterValueEditorOnly(parameterName, vectorParameter);
		valid = true;
	}
#endif

	if (self->ue_object->IsA<UMaterialInstanceDynamic>()) {
		UMaterialInstanceDynamic *material_instance = (UMaterialInstanceDynamic *)self->ue_object;
		material_instance->SetVectorParameterValue(parameterName, vectorParameter);
		valid = true;
	}

	if (!valid) {
		return PyErr_Format(PyExc_Exception, "uobject is not a MaterialInstance");
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_get_material_vector_parameter(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *scalarName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_vector_parameter", &scalarName)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	FLinearColor vec(0, 0, 0);

	material_instance->GetVectorParameterValue(parameterName, vec);

	return py_ue_new_flinearcolor(vec);
}


PyObject *py_ue_set_material_texture_parameter(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	char *textureName;
	PyObject *textureObject = nullptr;
	if (!PyArg_ParseTuple(args, "sO:set_texture_parameter", &textureName, &textureObject)) {
		return NULL;
	}

	if (!ue_is_pyuobject(textureObject)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)textureObject;
	if (!py_obj->ue_object->IsA<UTexture>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UTexture");

	UTexture *ue_texture = (UTexture *)py_obj->ue_object;

	FName parameterName(UTF8_TO_TCHAR(textureName));

	bool valid = false;

#if WITH_EDITOR
	if (self->ue_object->IsA<UMaterialInstanceConstant>()) {
		UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
		material_instance->SetTextureParameterValueEditorOnly(parameterName, ue_texture);
		valid = true;
	}
#endif

	if (self->ue_object->IsA<UMaterialInstanceDynamic>()) {
		UMaterialInstanceDynamic *material_instance = (UMaterialInstanceDynamic *)self->ue_object;
		material_instance->SetTextureParameterValue(parameterName, ue_texture);
		valid = true;
	}

	if (!valid) {
		return PyErr_Format(PyExc_Exception, "uobject is not a MaterialInstance");
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_get_material_texture_parameter(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *scalarName = nullptr;
	if (!PyArg_ParseTuple(args, "s:get_material_texture_parameter", &scalarName)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstance>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstance");
	}

	FName parameterName(UTF8_TO_TCHAR(scalarName));

	UMaterialInstance *material_instance = (UMaterialInstance *)self->ue_object;

	UTexture *texture = nullptr;

	if (!material_instance->GetTextureParameterValue(parameterName, texture)) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	ue_PyUObject *ret = ue_get_python_wrapper(texture);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_create_material_instance_dynamic(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_material = nullptr;

	if (!PyArg_ParseTuple(args, "O:create_material_instance_dynamic", &py_material)) {
		return NULL;
	}

	if (!ue_is_pyuobject(py_material)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_material;

	if (!py_obj->ue_object->IsA<UMaterialInstanceConstant>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstanceConstant");
	}

	UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)py_obj->ue_object;

	UMaterialInstanceDynamic *material_dynamic = UMaterialInstanceDynamic::Create(material_instance, self->ue_object);

	ue_PyUObject *ret = ue_get_python_wrapper(material_dynamic);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_ue_set_material(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int index;
	PyObject *py_material = nullptr;

	if (!PyArg_ParseTuple(args, "iO:set_material", &index, &py_material)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UPrimitiveComponent>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UPrimitiveComponent");
	}

	UPrimitiveComponent *component = (UPrimitiveComponent *)self->ue_object;

	if (!ue_is_pyuobject(py_material)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_material;

	if (!py_obj->ue_object->IsA<UMaterialInterface>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInterface");
	}

	UMaterialInterface *material_interface = (UMaterialInterface *)py_obj->ue_object;

	component->SetMaterial(index, material_interface);

	Py_INCREF(Py_None);
	return Py_None;
}


#if WITH_EDITOR
PyObject *py_ue_set_material_parent(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);


	PyObject *py_material = nullptr;

	if (!PyArg_ParseTuple(args, "O:set_parent", &py_material)) {
		return NULL;
	}

	if (!self->ue_object->IsA<UMaterialInstanceConstant>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInstanceConstant");
	}


	if (!ue_is_pyuobject(py_material)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_material;
	if (!py_obj->ue_object->IsA<UMaterialInterface>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInterface");


	UMaterialInterface *materialInterface = (UMaterialInterface *)py_obj->ue_object;
	UMaterialInstanceConstant *material_instance = (UMaterialInstanceConstant *)self->ue_object;
	material_instance->SetParentEditorOnly(materialInterface);
	material_instance->PostEditChange();

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_static_mesh_set_collision_for_lod(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	int lod_index;
	int material_index;
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "iiO:static_mesh_set_collision_for_lod", &lod_index, &material_index, &py_bool)) {
		return NULL;
	}


	if (!self->ue_object->IsA<UStaticMesh>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a StaticMesh");
	}

	UStaticMesh *mesh = (UStaticMesh *)self->ue_object;

	bool enabled = false;
	if (PyObject_IsTrue(py_bool)) {
		enabled = true;
	}

	FMeshSectionInfo info = mesh->SectionInfoMap.Get(lod_index, material_index);
	info.bEnableCollision = enabled;
	mesh->SectionInfoMap.Set(lod_index, material_index, info);

	mesh->MarkPackageDirty();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_static_mesh_set_shadow_for_lod(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);

	int lod_index;
	int material_index;
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "iiO:static_mesh_set_shadow_for_lod", &lod_index, &material_index, &py_bool)) {
		return NULL;
	}


	if (!self->ue_object->IsA<UStaticMesh>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a StaticMesh");
	}

	UStaticMesh *mesh = (UStaticMesh *)self->ue_object;

	bool enabled = false;
	if (PyObject_IsTrue(py_bool)) {
		enabled = true;
	}

	FMeshSectionInfo info = mesh->SectionInfoMap.Get(lod_index, material_index);
	info.bCastShadow = enabled;
	mesh->SectionInfoMap.Set(lod_index, material_index, info);

	mesh->MarkPackageDirty();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_get_material_graph(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UMaterial>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInterface");
	}

	UMaterial *material = (UMaterial *)self->ue_object;

	UMaterialGraph *graph = material->MaterialGraph;
	if (!graph)
		material->MaterialGraph = (UMaterialGraph *)FBlueprintEditorUtils::CreateNewGraph(material, NAME_None, UMaterialGraph::StaticClass(), UMaterialGraphSchema::StaticClass());
	if (!graph)
		return PyErr_Format(PyExc_Exception, "Unable to retrieve/allocate MaterialGraph");

	ue_PyUObject *ret = ue_get_python_wrapper(graph);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

#endif
