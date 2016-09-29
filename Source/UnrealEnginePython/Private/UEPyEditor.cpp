#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Editor/UnrealEd/Classes/Factories/Factory.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "UnrealEd.h"
#include "FbxMeshUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"


PyObject *py_unreal_engine_get_editor_world(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	UWorld *world = GEditor->GetEditorWorldContext().World();
	ue_PyUObject *ret = ue_get_python_wrapper(world);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}


PyObject *py_unreal_engine_editor_get_selected_actors(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *actors = PyList_New(0);

	USelection *selection = GEditor->GetSelectedActors();
	int32 nums = selection->CountSelections<UObject>();

	for (int32 i = 0; i < nums; i++) {
		UObject *obj = selection->GetSelectedObject(i);
		if (!obj->IsA<AActor>())
			continue;
		AActor *actor = (AActor *)obj;
		ue_PyUObject *item = ue_get_python_wrapper(actor);
		if (item)
			PyList_Append(actors, (PyObject *)item);
	}

	return actors;
}

PyObject *py_unreal_engine_editor_deselect_actors(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	GEditor->SelectNone(true, true, false);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_play(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *obj = nullptr;
	if (!PyArg_ParseTuple(args, "O:editor_play", &obj)) {
		return NULL;
	}

	bool pie = true;

	if (obj && !PyObject_IsTrue(obj))
		pie = false;

	GEditor->PlayInEditor(GEditor->GetEditorWorldContext().World(), pie);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_select_actor(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:editor_select_actor", &obj)) {
		return NULL;
	}

	if (!ue_is_pyuobject(obj))
		return PyErr_Format(PyExc_Exception, "invalid uobject");

	ue_PyUObject *py_actor = (ue_PyUObject *)obj;

	if (!py_actor->ue_object->IsA<AActor>())
		return PyErr_Format(PyExc_Exception, "uobject is not an Actor");

	AActor *actor = (AActor *)py_actor->ue_object;

	GEditor->SelectActor(actor, true, true);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_import_asset(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	char *filename;
	char *destination;
	PyObject *obj = nullptr;
	if (!PyArg_ParseTuple(args, "ss|O:import_asset", &filename, &destination, &obj)) {
		return NULL;
	}

	FString Result;
	// avoid crash on wrong path
	if (!FPackageName::TryConvertLongPackageNameToFilename(UTF8_TO_TCHAR(destination), Result, "")) {
		return PyErr_Format(PyExc_Exception, "invalid asset root path");
	}

	UClass *factory_class = nullptr;
	UFactory *factory = nullptr;

	if (!obj || obj == Py_None) {
		factory_class = nullptr;
	}
	else if (ue_is_pyuobject(obj)) {
		ue_PyUObject *py_factory = (ue_PyUObject *)obj;
		if (py_factory->ue_object->IsA<UClass>()) {
			factory_class = (UClass *)py_factory->ue_object;
		}
		else if (py_factory->ue_object->IsA<UFactory>()) {
			factory = (UFactory *)py_factory->ue_object;
		}
		else {
			return PyErr_Format(PyExc_Exception, "uobject is not a Class");
		}	
	}
	else if (PyUnicode_Check(obj)) {
		char *class_name = PyUnicode_AsUTF8(obj);
		UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(class_name));
		if (u_class) {
			ue_PyUObject *py_obj = ue_get_python_wrapper(u_class);
			if (py_obj)
				Py_INCREF(py_obj);
			factory_class = (UClass *)py_obj->ue_object;
		}
	}
	else {
		return PyErr_Format(PyExc_Exception, "invalid uobject");
	}
		
	
	if (factory_class) {
		factory = NewObject<UFactory>(GetTransientPackage(), factory_class);
		if (!factory) {
			return PyErr_Format(PyExc_Exception, "unable to create factory");
		}
	}

	TArray<FString> files;
	files.Add(UTF8_TO_TCHAR(filename));

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject *> objects = AssetToolsModule.Get().ImportAssets(files, UTF8_TO_TCHAR(destination), factory, false);

	if (objects.Num() > 0) {
		UObject *object = objects[0];
		ue_PyUObject *ret = ue_get_python_wrapper(object);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}
	 

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_message_dialog_open(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	int app_msg_type;
	char *text;
	char *title = nullptr;
	if (!PyArg_ParseTuple(args, "is|s:message_dialog_open", &app_msg_type, &text, &title)) {
		return NULL;
	}

	FText *f_title = nullptr;

	if (title) {
		FText f_title_value = FText::FromString(UTF8_TO_TCHAR(title));
		f_title = &f_title_value;
	}

	EAppReturnType::Type ret = FMessageDialog::Open((EAppMsgType::Type) app_msg_type, FText::FromString(UTF8_TO_TCHAR(text)), f_title);

	return PyLong_FromLong(ret);
}

PyObject *py_unreal_engine_get_asset(PyObject * self, PyObject * args) {
	char *path;

	if (!PyArg_ParseTuple(args, "s:get_asset", &path)) {
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData asset = AssetRegistryModule.Get().GetAssetByObjectPath(UTF8_TO_TCHAR(path));
	if (!asset.IsValid())
		return PyErr_Format(PyExc_Exception, "unable to find asset %s", path);
	ue_PyUObject *ret = ue_get_python_wrapper(asset.GetAsset());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_get_assets(PyObject * self, PyObject * args) {
	char *path;
	PyObject *py_recursive = nullptr;

	if (!PyArg_ParseTuple(args, "s|O:get_assets", &path, &py_recursive)) {
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	bool recursive = false;
	if (py_recursive && PyObject_IsTrue(py_recursive))
		recursive = true;

	TArray<FAssetData> assets;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().GetAssetsByPath(UTF8_TO_TCHAR(path), assets, recursive);

	PyObject *assets_list = PyList_New(0);

	for (FAssetData asset : assets) {
		if (!asset.IsValid())
			continue;
		ue_PyUObject *ret = ue_get_python_wrapper(asset.GetAsset());
		if (ret) {
			PyList_Append(assets_list, (PyObject *)ret);
		}
	}

	return assets_list;
}

PyObject *py_unreal_engine_get_assets_by_class(PyObject * self, PyObject * args) {
	char *path;
	PyObject *py_recursive = nullptr;

	if (!PyArg_ParseTuple(args, "s|O:get_assets_by_class", &path, &py_recursive)) {
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	bool recursive = false;
	if (py_recursive && PyObject_IsTrue(py_recursive))
		recursive = true;

	TArray<FAssetData> assets;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().GetAssetsByClass(UTF8_TO_TCHAR(path), assets, recursive);

	PyObject *assets_list = PyList_New(0);

	for (FAssetData asset : assets) {
		if (!asset.IsValid())
			continue;
		ue_PyUObject *ret = ue_get_python_wrapper(asset.GetAsset());
		if (ret) {
			PyList_Append(assets_list, (PyObject *)ret);
		}
	}

	return assets_list;
}

PyObject *py_unreal_engine_get_selected_assets(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");


	TArray<FAssetData> assets;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().GetSelectedAssets(assets);

	PyObject *assets_list = PyList_New(0);

	for (FAssetData asset : assets) {
		if (!asset.IsValid())
			continue;
		ue_PyUObject *ret = ue_get_python_wrapper(asset.GetAsset());
		if (ret) {
			PyList_Append(assets_list, (PyObject *)ret);
		}
	}

	return assets_list;
}

PyObject *py_unreal_engine_set_fbx_import_option(PyObject * self, PyObject * args) {
	PyObject *obj;

	if (!PyArg_ParseTuple(args, "O:set_fbx_import_option", &obj)) {
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	if (!ue_is_pyuobject(obj))
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;
	if (!py_obj->ue_object->IsA<UFbxImportUI>())
		return PyErr_Format(PyExc_Exception, "object is not a FbxImportUI");

	UFbxImportUI *ui = (UFbxImportUI *)py_obj->ue_object;

	FbxMeshUtils::SetImportOption(ui);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_create_blueprint(PyObject * self, PyObject * args) {

	PyObject *py_parent;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:create_blueprint", &py_parent, &name)) {
		return NULL;
	}

	UClass *parent = UObject::StaticClass();

	if (py_parent != Py_None) {
		if (!ue_is_pyuobject(py_parent)) {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		ue_PyUObject *py_obj = (ue_PyUObject *)py_parent;
		if (!py_obj->ue_object->IsA<UClass>())
			return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
		parent = (UClass *)py_obj->ue_object;
	}

	if (name[0] != '/') {
		return PyErr_Format(PyExc_Exception, "path must start with a /");
	}

	char *bp_name = strrchr(name, '/') + 1;
	if (strlen(bp_name) < 1) {
		return PyErr_Format(PyExc_Exception, "invalid blueprint name");
	}

	UPackage *outer = CreatePackage(nullptr, UTF8_TO_TCHAR(name));
	if (!outer)
		return PyErr_Format(PyExc_Exception, "unable to create package");

	UBlueprint *bp = FKismetEditorUtilities::CreateBlueprint(parent, outer, UTF8_TO_TCHAR(bp_name), EBlueprintType::BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	if (bp) {
		FAssetRegistryModule::AssetCreated(bp);
		outer->MarkPackageDirty();
	}

	ue_PyUObject *ret = ue_get_python_wrapper(bp);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}


PyObject *py_unreal_engine_reload_blueprint(PyObject * self, PyObject * args) {

	PyObject *py_blueprint;
	if (!PyArg_ParseTuple(args, "O:reload_blueprint", &py_blueprint)) {
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	UBlueprint *reloaded_bp = FKismetEditorUtilities::ReloadBlueprint(bp);

	ue_PyUObject *ret = ue_get_python_wrapper(reloaded_bp);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_unreal_engine_compile_blueprint(PyObject * self, PyObject * args) {

	PyObject *py_blueprint;
	if (!PyArg_ParseTuple(args, "O:compile_blueprint", &py_blueprint)) {
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	FKismetEditorUtilities::CompileBlueprint(bp);

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_unreal_engine_replace_blueprint(PyObject * self, PyObject * args) {

	PyObject *py_blueprint;
	PyObject *py_blueprint_new;
	if (!PyArg_ParseTuple(args, "OO:replace_blueprint", &py_blueprint, &py_blueprint_new)) {
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	if (!ue_is_pyuobject(py_blueprint_new)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	py_obj = (ue_PyUObject *)py_blueprint_new;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp_new = (UBlueprint *)py_obj->ue_object;

	UBlueprint *replaced_bp = FKismetEditorUtilities::ReplaceBlueprint(bp, bp_new);

	ue_PyUObject *ret = ue_get_python_wrapper(replaced_bp);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_unreal_engine_create_blueprint_from_actor(PyObject * self, PyObject * args) {

	PyObject *py_actor;
	char *name;
	if (!PyArg_ParseTuple(args, "sO:create_blueprint", &name, &py_actor)) {
		return NULL;
	}

	if (name[0] != '/') {
		return PyErr_Format(PyExc_Exception, "path must start with a /");
	}

	if (!ue_is_pyuobject(py_actor)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}
	ue_PyUObject *py_obj = (ue_PyUObject *)py_actor;
	if (!py_obj->ue_object->IsA<AActor>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
	AActor *actor = (AActor *)py_obj->ue_object;

	UBlueprint *bp = FKismetEditorUtilities::CreateBlueprintFromActor(UTF8_TO_TCHAR(name), actor, true);

	ue_PyUObject *ret = ue_get_python_wrapper(bp);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_unreal_engine_add_component_to_blueprint(PyObject * self, PyObject * args) {

	PyObject *py_blueprint;
	PyObject *py_component;
	char *name;
	if (!PyArg_ParseTuple(args, "OOs:add_component_to_blueprint", &py_blueprint, &py_component, &name)) {
		return NULL;
	}


	if (!ue_is_pyuobject(py_blueprint)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	if (!ue_is_pyuobject(py_component)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	py_obj = (ue_PyUObject *)py_component;
	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
	UClass *component_class = (UClass *)py_obj->ue_object;


	bp->Modify();
	USCS_Node *node = bp->SimpleConstructionScript->CreateNode(component_class, UTF8_TO_TCHAR(name));
	if (!node) {
		return PyErr_Format(PyExc_Exception, "unable to allocate new component");
	}
	USCS_Node *root = bp->SimpleConstructionScript->GetDefaultSceneRootNode();
	if (root) {
		root->AddChildNode(node);
	}
	else {
		bp->SimpleConstructionScript->AddNode(node);
	}
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

	ue_PyUObject *ret = ue_get_python_wrapper(node->ComponentTemplate);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
	
}

PyObject *py_unreal_engine_blueprint_add_member_variable(PyObject * self, PyObject * args) {

	PyObject *py_blueprint;
	char *name;
	char *in_type;
	PyObject *py_is_array = nullptr;
	if (!PyArg_ParseTuple(args, "Oss|O:blueprint_add_member_variable", &py_blueprint, &name, &in_type, &py_is_array)) {
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	bool is_array = false;
	if (py_is_array && PyObject_IsTrue(py_is_array))
		is_array = true;

	FEdGraphPinType pin(UTF8_TO_TCHAR(in_type), FString(""), nullptr, is_array, false);

	if (FBlueprintEditorUtils::AddMemberVariable(bp, UTF8_TO_TCHAR(name), pin)) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_unreal_engine_blueprint_add_new_timeline(PyObject * self, PyObject * args) {

	PyObject *py_blueprint;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:blueprint_add_new_timeline", &py_blueprint, &name)) {
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	UTimelineTemplate *timeline = FBlueprintEditorUtils::AddNewTimeline(bp, UTF8_TO_TCHAR(name));
	if (!timeline) {
		return PyErr_Format(PyExc_Exception, "unable to add new timeline %s", name);
	}

	ue_PyUObject *ret = ue_get_python_wrapper(timeline);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

#endif

