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
#include "Editor/LevelEditor/Public/LevelEditorActions.h"
#include "Editor/UnrealEd/Public/EditorLevelUtils.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "ObjectTools.h"
#include "Developer/AssetTools/Public/IAssetTools.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "Runtime/Engine/Classes/EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/EdGraph/EdGraphSchema.h"
#include "Toolkits/AssetEditorManager.h"
#include "LevelEditor.h"
#include "Editor/LandscapeEditor/Public/LandscapeEditorUtils.h"
#include "Editor/LandscapeEditor/Public/LandscapeEditorModule.h"
#include "Editor/LandscapeEditor/Public/LandscapeFileFormatInterface.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Engine/Blueprint.h"


PyObject *py_unreal_engine_editor_play_in_viewport(PyObject * self, PyObject * args)
{

	PyObject *py_vector = nullptr;
	PyObject *py_rotator = nullptr;

	if (!PyArg_ParseTuple(args, "|OO:editor_play_in_viewport", &py_vector, &py_rotator))
	{
		return NULL;
	}

	FVector v;
	FRotator r;

	if (py_vector)
	{
		ue_PyFVector *vector = py_ue_is_fvector(py_vector);
		if (!vector)
			return PyErr_Format(PyExc_Exception, "argument is not a FVector");
		v = vector->vec;
	}

	if (py_rotator)
	{
		ue_PyFRotator *rotator = py_ue_is_frotator(py_rotator);
		if (!rotator)
			return PyErr_Format(PyExc_Exception, "argument is not a FRotator");
		r = rotator->rot;
	}

	FLevelEditorModule &EditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!EditorModule.GetFirstActiveViewport().IsValid())
		return PyErr_Format(PyExc_Exception, "no active LevelEditor Viewport");

	GEditor->RequestPlaySession(py_vector == nullptr, EditorModule.GetFirstActiveViewport(), true, &v, &r);

	Py_RETURN_NONE;

}

PyObject *py_unreal_engine_request_play_session(PyObject * self, PyObject * args)
{

	PyObject *py_at_player_start = nullptr;
	PyObject *py_simulate_in_editor = nullptr;

	if (!PyArg_ParseTuple(args, "|OO:request_play_session", &py_at_player_start, &py_simulate_in_editor))
	{
		return nullptr;
	}

	bool bAtPlayerStart = py_at_player_start && PyObject_IsTrue(py_at_player_start);
	bool bSimulate = py_simulate_in_editor && PyObject_IsTrue(py_simulate_in_editor);

	GEditor->RequestPlaySession(bAtPlayerStart, nullptr, bSimulate);

	Py_RETURN_NONE;

}


PyObject *py_unreal_engine_get_editor_world(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	UWorld *world = GEditor->GetEditorWorldContext().World();
	Py_RETURN_UOBJECT(world);
}

PyObject *py_unreal_engine_console_exec(PyObject * self, PyObject * args)
{

	char *command;

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	if (!PyArg_ParseTuple(args, "s:console_exec", &command))
	{
		return NULL;
	}

	GEditor->Exec(GEditor->GetEditorWorldContext().World(), UTF8_TO_TCHAR(command), *GLog);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_allow_actor_script_execution_in_editor(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *obj = nullptr;
	if (!PyArg_ParseTuple(args, "O:allow_actor_script_execution_in_editor", &obj))
	{
		return NULL;
	}

	bool enable = false;
	if (PyObject_IsTrue(obj))
		enable = true;

	GAllowActorScriptExecutionInEditor = enable;

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_unreal_engine_editor_get_selected_actors(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *actors = PyList_New(0);

	USelection *selection = GEditor->GetSelectedActors();
	int32 nums = selection->CountSelections<UObject>();

	for (int32 i = 0; i < nums; i++)
	{
		UObject *obj = selection->GetSelectedObject(i);
		if (!obj->IsA<AActor>())
			continue;
		AActor *actor = (AActor *)obj;
		ue_PyUObject *item = ue_get_python_uobject(actor);
		if (item)
			PyList_Append(actors, (PyObject *)item);
	}

	return actors;
}

PyObject *py_unreal_engine_editor_command_build(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FLevelEditorActionCallbacks::Build_Execute();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_command_save_current_level(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FLevelEditorActionCallbacks::Save();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_command_save_all_levels(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FLevelEditorActionCallbacks::SaveAllLevels();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_save_all(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FEditorFileUtils::SaveDirtyPackages(false, true, true, false, false, false);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_command_build_lighting(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FLevelEditorActionCallbacks::BuildLightingOnly_Execute();
	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_unreal_engine_editor_deselect_actors(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	GEditor->SelectNone(true, true, false);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_editor_play(PyObject * self, PyObject * args)
{

	PyObject *py_vector = nullptr;
	PyObject *py_rotator = nullptr;

	if (!PyArg_ParseTuple(args, "|OO:editor_play", &py_vector, &py_rotator))
	{
		return NULL;
	}

	FVector v;
	FRotator r;

	if (py_vector)
	{
		ue_PyFVector *vector = py_ue_is_fvector(py_vector);
		if (!vector)
			return PyErr_Format(PyExc_Exception, "argument is not a FVector");
		v = vector->vec;
	}

	if (py_rotator)
	{
		ue_PyFRotator *rotator = py_ue_is_frotator(py_rotator);
		if (!rotator)
			return PyErr_Format(PyExc_Exception, "argument is not a FRotator");
		r = rotator->rot;
	}

#if ENGINE_MINOR_VERSION >= 17
	const FString mobile_device = FString("");
	GEditor->RequestPlaySession(&v, &r, false, false, mobile_device);
#else
	GEditor->RequestPlaySession(&v, &r, false, false);
#endif

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_editor_select_actor(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:editor_select_actor", &obj))
	{
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

PyObject *py_unreal_engine_import_asset(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	//char *filename;
	PyObject * assetsObject = nullptr;
	char *destination;
	PyObject *obj = nullptr;
	PyObject *py_sync = nullptr;
	if (!PyArg_ParseTuple(args, "Os|OO:import_asset", &assetsObject, &destination, &obj, &py_sync))
	{
		return NULL;
	}

	FString Result;
	// avoid crash on wrong path
	if (!FPackageName::TryConvertLongPackageNameToFilename(UTF8_TO_TCHAR(destination), Result, ""))
	{
		return PyErr_Format(PyExc_Exception, "invalid asset root path");
	}

	UClass *factory_class = nullptr;
	UFactory *factory = nullptr;
	bool sync_to_browser = false;

	if (!obj || obj == Py_None)
	{
		factory_class = nullptr;
	}
	else if (ue_is_pyuobject(obj))
	{
		ue_PyUObject *py_factory = (ue_PyUObject *)obj;
		if (py_factory->ue_object->IsA<UClass>())
		{
			factory_class = (UClass *)py_factory->ue_object;
		}
		else if (py_factory->ue_object->IsA<UFactory>())
		{
			factory = (UFactory *)py_factory->ue_object;
		}
		else
		{
			return PyErr_Format(PyExc_Exception, "uobject is not a Class");
		}
	}
	else if (PyUnicodeOrString_Check(obj))
	{
		char *class_name = PyUnicode_AsUTF8(obj);
		UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(class_name));
		if (u_class)
		{
			ue_PyUObject *py_obj = ue_get_python_uobject(u_class);
			if (!py_obj)
			{
				return PyErr_Format(PyExc_Exception, "invalid uobject");
			}
			factory_class = (UClass *)py_obj->ue_object;
		}
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "invalid uobject");
	}

	if (factory_class)
	{
		factory = NewObject<UFactory>(GetTransientPackage(), factory_class);
		if (!factory)
		{
			return PyErr_Format(PyExc_Exception, "unable to create factory");
		}
	}


	TArray<FString> files;

	if (PyList_Check(assetsObject))
	{
		// parse the list object
		int numLines = PyList_Size(assetsObject);


		if (numLines <= 0)
		{
			return PyErr_Format(PyExc_Exception, "Asset paths is not a valid list");
		}

		for (int i = 0; i < numLines; ++i)
		{

			PyObject * strObj = PyList_GetItem(assetsObject, i);
#if PY_MAJOR_VERSION >= 3
			char * filename = PyBytes_AS_STRING(PyUnicode_AsEncodedString(strObj, "utf-8", "Error"));
#else
			char * filename = PyString_AsString(PyObject_Str(strObj));
#endif
			files.Add(UTF8_TO_TCHAR(filename));
		}
	}
	else if (PyUnicodeOrString_Check(assetsObject))
	{
#if PY_MAJOR_VERSION >= 3
		char * filename = PyBytes_AS_STRING(PyUnicode_AsEncodedString(assetsObject, "utf-8", "Error"));
#else
		char * filename = PyString_AsString(PyObject_Str(assetsObject));
#endif
		files.Add(UTF8_TO_TCHAR(filename));
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "Not a string nor valid list of string");
	}

	if (py_sync && PyObject_IsTrue(py_sync))
	{
		sync_to_browser = true;
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject *> objects = AssetToolsModule.Get().ImportAssets(files, UTF8_TO_TCHAR(destination), factory, sync_to_browser);

	if (objects.Num() == 1)
	{

		UObject *object = objects[0];
		Py_RETURN_UOBJECT(object);
	}
	else if (objects.Num() > 1)
	{
		PyObject *assets_list = PyList_New(0);

		for (UObject *object : objects)
		{
			ue_PyUObject *ret = ue_get_python_uobject(object);
			if (!ret)
			{
				Py_DECREF(assets_list);
				return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
			}

			PyList_Append(assets_list, (PyObject *)ret);
		}

		return assets_list;

	}

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_editor_tick(PyObject * self, PyObject * args)
{
	float delta_seconds = FApp::GetDeltaTime();
	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "|fO:editor_tick", &delta_seconds, &py_bool))
	{
		return NULL;
	}

	GEditor->Tick(delta_seconds, (py_bool && PyObject_IsTrue(py_bool)) ? true : false);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_message_dialog_open(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	int app_msg_type;
	char *text;

	if (!PyArg_ParseTuple(args, "is:message_dialog_open", &app_msg_type, &text))
	{
		return NULL;
	}

	EAppReturnType::Type ret = FMessageDialog::Open((EAppMsgType::Type) app_msg_type, FText::FromString(UTF8_TO_TCHAR(text)));

	return PyLong_FromLong(ret);
}

PyObject *py_unreal_engine_create_modal_save_asset_dialog(PyObject * self, PyObject * args)
{

	char *title = (char *)"";
	char *path = (char*)"";
	char *default_name = (char*)"";

	if (!PyArg_ParseTuple(args, "|sss:create_modal_save_asset_dialog", &title, &path, &default_name))
	{
		return nullptr;
	}

	FSaveAssetDialogConfig config;
	config.DialogTitleOverride = FText::FromString(FString(UTF8_TO_TCHAR(title)));
	config.DefaultPath = FString(UTF8_TO_TCHAR(path));
	config.DefaultAssetName = FString(UTF8_TO_TCHAR(default_name));
	config.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

	FContentBrowserModule &ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString ret = ContentBrowserModule.Get().CreateModalSaveAssetDialog(config);

	if (ret.IsEmpty())
	{
		Py_RETURN_NONE;
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*ret));
}

PyObject *py_unreal_engine_get_asset(PyObject * self, PyObject * args)
{
	char *path;

	if (!PyArg_ParseTuple(args, "s:get_asset", &path))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData asset = AssetRegistryModule.Get().GetAssetByObjectPath(UTF8_TO_TCHAR(path));
	if (!asset.IsValid())
		return PyErr_Format(PyExc_Exception, "unable to find asset %s", path);
	Py_RETURN_UOBJECT(asset.GetAsset());
}

PyObject *py_unreal_engine_find_asset(PyObject * self, PyObject * args)
{
	char *path;

	if (!PyArg_ParseTuple(args, "s:find_asset", &path))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData asset = AssetRegistryModule.Get().GetAssetByObjectPath(UTF8_TO_TCHAR(path));
	if (!asset.IsValid())
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
	Py_RETURN_UOBJECT(asset.GetAsset());
}

PyObject *py_unreal_engine_get_asset_referencers(PyObject * self, PyObject * args)
{
	char *path;

	if (!PyArg_ParseTuple(args, "s:get_asset_referencers", &path))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FName> referencers;
	AssetRegistryModule.Get().GetReferencers(UTF8_TO_TCHAR(path), referencers);

	PyObject *referencers_list = PyList_New(0);
	for (FName name : referencers)
	{
		PyList_Append(referencers_list, PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString())));
	}
	return referencers_list;
}

PyObject *py_unreal_engine_get_asset_dependencies(PyObject * self, PyObject * args)
{
	char *path;

	if (!PyArg_ParseTuple(args, "s:get_asset_dependencies", &path))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FName> dependencies;
	AssetRegistryModule.Get().GetDependencies(UTF8_TO_TCHAR(path), dependencies);

	PyObject *dependencies_list = PyList_New(0);
	for (FName name : dependencies)
	{
		PyList_Append(dependencies_list, PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString())));
	}
	return dependencies_list;
}

PyObject *py_unreal_engine_get_long_package_path(PyObject * self, PyObject * args)
{
	char *path;
	if (!PyArg_ParseTuple(args, "s:get_long_package_path", &path))
	{
		return NULL;
	}

	const FString package_path = FPackageName::GetLongPackagePath(UTF8_TO_TCHAR(path));

	return PyUnicode_FromString(TCHAR_TO_UTF8(*(package_path)));
}

PyObject *py_unreal_engine_rename_asset(PyObject * self, PyObject * args)
{
	char *path;
	char *package_name;
	char *object_name;
	if (!PyArg_ParseTuple(args, "sss:rename_asset", &path, &package_name, &object_name))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData asset = AssetRegistryModule.Get().GetAssetByObjectPath(UTF8_TO_TCHAR(path));
	if (!asset.IsValid())
		return PyErr_Format(PyExc_Exception, "unable to find asset %s", path);

	UObject *u_object = asset.GetAsset();
	ObjectTools::FPackageGroupName pgn;
	pgn.ObjectName = UTF8_TO_TCHAR(object_name);
	pgn.GroupName = FString("");
	pgn.PackageName = UTF8_TO_TCHAR(package_name);

	TSet<UPackage *> refused_packages;
	FText error_text;
	if (!ObjectTools::RenameSingleObject(u_object, pgn, refused_packages, error_text, nullptr, false))
	{
		return PyErr_Format(PyExc_Exception, "unable to rename asset %s", path);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_duplicate_asset(PyObject * self, PyObject * args)
{
	char *path;
	char *package_name;
	char *object_name;
	if (!PyArg_ParseTuple(args, "sss:duplicate_asset", &path, &package_name, &object_name))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData asset = AssetRegistryModule.Get().GetAssetByObjectPath(UTF8_TO_TCHAR(path));
	if (!asset.IsValid())
		return PyErr_Format(PyExc_Exception, "unable to find asset %s", path);

	UObject *u_object = asset.GetAsset();
	ObjectTools::FPackageGroupName pgn;
	pgn.ObjectName = UTF8_TO_TCHAR(object_name);
	pgn.GroupName = FString("");
	pgn.PackageName = UTF8_TO_TCHAR(package_name);

	TSet<UPackage *> refused_packages;
	FText error_text;
#if ENGINE_MINOR_VERSION < 14
	UObject *new_asset = ObjectTools::DuplicateSingleObject(u_object, pgn, refused_packages);
#else
	UObject *new_asset = ObjectTools::DuplicateSingleObject(u_object, pgn, refused_packages, false);
#endif
	if (!new_asset)
	{
		return PyErr_Format(PyExc_Exception, "unable to duplicate asset %s", path);
	}

	Py_RETURN_UOBJECT(new_asset);
}

PyObject *py_unreal_engine_delete_asset(PyObject * self, PyObject * args)
{
	char *path;
	PyObject *py_show_confirmation = nullptr;
	if (!PyArg_ParseTuple(args, "s|O:delete_asset", &path, &py_show_confirmation))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	bool show_confirmation = false;
	if (py_show_confirmation && PyObject_IsTrue(py_show_confirmation))
	{
		show_confirmation = true;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData asset = AssetRegistryModule.Get().GetAssetByObjectPath(UTF8_TO_TCHAR(path));
	if (!asset.IsValid())
		return PyErr_Format(PyExc_Exception, "unable to find asset %s", path);

	UObject *u_object = asset.GetAsset();

	TArray<UObject *> objects;
	objects.Add(u_object);

	if (ObjectTools::DeleteObjects(objects, show_confirmation) < 1)
	{
		if (ObjectTools::ForceDeleteObjects(objects, show_confirmation) < 1)
		{
			return PyErr_Format(PyExc_Exception, "unable to delete asset %s", path);
		}
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_delete_object(PyObject * self, PyObject * args)
{
	PyObject *py_obj;
	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "O|O:delete_object", &py_obj, &py_bool))
	{
		return NULL;
	}

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	UObject *u_object = ue_py_check_type<UObject>(py_obj);
	if (!u_object)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	TArray<UObject *> objects_to_delete;
	objects_to_delete.Add(u_object);


	if (py_bool && PyObject_IsTrue(py_bool))
	{
		if (ObjectTools::ForceDeleteObjects(objects_to_delete, false) < 1)
		{
			return PyErr_Format(PyExc_Exception, "unable to delete object");
		}
	}
	else
	{
		if (ObjectTools::DeleteObjects(objects_to_delete, false) < 1)
		{
			return PyErr_Format(PyExc_Exception, "unable to delete asset");
		}
	}

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_unreal_engine_get_assets(PyObject * self, PyObject * args)
{
	char *path;
	PyObject *py_recursive = nullptr;

	if (!PyArg_ParseTuple(args, "s|O:get_assets", &path, &py_recursive))
	{
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

	for (FAssetData asset : assets)
	{
		if (!asset.IsValid())
			continue;
		ue_PyUObject *ret = ue_get_python_uobject(asset.GetAsset());
		if (ret)
		{
			PyList_Append(assets_list, (PyObject *)ret);
		}
	}

	return assets_list;
}

PyObject *py_unreal_engine_get_assets_by_filter(PyObject * self, PyObject * args, PyObject *kwargs)
{

	PyObject *pyfilter;
	PyObject *py_return_asset_data = nullptr;

	static char *kw_names[] = { (char *)"filter", (char *)"return_asset_data", NULL };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:get_assets_by_filter", kw_names, &pyfilter, &py_return_asset_data))
	{
		return NULL;
	}

	ue_PyFARFilter *filter = py_ue_is_farfilter(pyfilter);
	if (!filter)
		return PyErr_Format(PyExc_Exception, "Arg is not a FARFilter");

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	py_ue_sync_farfilter((PyObject *)filter);
	TArray<FAssetData> assets;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().SearchAllAssets(true);
	AssetRegistryModule.Get().GetAssets(filter->filter, assets);

	bool return_asset_data = false;
	if (py_return_asset_data && PyObject_IsTrue(py_return_asset_data))
		return_asset_data = true;
	PyObject *assets_list = PyList_New(0);
	for (FAssetData asset : assets)
	{
		if (!asset.IsValid())
			continue;
		PyObject *ret = nullptr;
		if (return_asset_data)
		{
			ret = py_ue_new_fassetdata(asset);
		}
		else
		{
			ret = (PyObject *)ue_get_python_uobject(asset.GetAsset());
		}

		if (ret)
		{
			PyList_Append(assets_list, ret);
		}
	}

	return assets_list;
}

PyObject *py_unreal_engine_get_discovered_plugins(PyObject * self, PyObject * args)
{

	PyObject *plugins_list = PyList_New(0);

	for (TSharedRef<IPlugin>plugin : IPluginManager::Get().GetDiscoveredPlugins())
	{
		PyObject *ret = py_ue_new_iplugin(&plugin.Get());
		if (ret)
		{
			PyList_Append(plugins_list, ret);
		}
	}

	return plugins_list;
}

PyObject *py_unreal_engine_get_enabled_plugins(PyObject * self, PyObject * args)
{

	PyObject *plugins_list = PyList_New(0);

	for (TSharedRef<IPlugin>plugin : IPluginManager::Get().GetEnabledPlugins())
	{
		PyObject *ret = py_ue_new_iplugin(&plugin.Get());
		if (ret)
		{
			PyList_Append(plugins_list, ret);
		}
	}

	return plugins_list;
}


PyObject *py_unreal_engine_find_plugin(PyObject * self, PyObject * args)
{
	char *name;

	if (!PyArg_ParseTuple(args, "s:find_plugin", &name))
	{
		return NULL;
	}

	TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(FString(UTF8_TO_TCHAR(name)));

	if (!plugin.IsValid())
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *ret = py_ue_new_iplugin(plugin.Get());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_unreal_engine_get_assets_by_class(PyObject * self, PyObject * args)
{
	char *path;
	PyObject *py_recursive = nullptr;

	if (!PyArg_ParseTuple(args, "s|O:get_assets_by_class", &path, &py_recursive))
	{
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

	for (FAssetData asset : assets)
	{
		if (!asset.IsValid())
			continue;
		ue_PyUObject *ret = ue_get_python_uobject(asset.GetAsset());
		if (ret)
		{
			PyList_Append(assets_list, (PyObject *)ret);
		}
	}

	return assets_list;
}

PyObject *py_unreal_engine_get_selected_assets(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");


	TArray<FAssetData> assets;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().GetSelectedAssets(assets);

	PyObject *assets_list = PyList_New(0);

	for (FAssetData asset : assets)
	{
		if (!asset.IsValid())
			continue;
		ue_PyUObject *ret = ue_get_python_uobject(asset.GetAsset());
		if (ret)
		{
			PyList_Append(assets_list, (PyObject *)ret);
		}
	}

	return assets_list;
}

PyObject *py_unreal_engine_open_editor_for_asset(PyObject * self, PyObject * args)
{
	PyObject *py_obj;

	if (!PyArg_ParseTuple(args, "O:open_editor_for_asset", &py_obj))
	{
		return NULL;
	}

	UObject *u_obj = ue_py_check_type<UObject>(py_obj);
	if (!u_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	if (FAssetEditorManager::Get().OpenEditorForAsset(u_obj))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}
	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_unreal_engine_close_editor_for_asset(PyObject * self, PyObject * args)
{
	PyObject *py_obj;

	if (!PyArg_ParseTuple(args, "O:close_editor_for_asset", &py_obj))
	{
		return NULL;
	}

	UObject *u_obj = ue_py_check_type<UObject>(py_obj);
	if (!u_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	FAssetEditorManager::Get().CloseAllEditorsForAsset(u_obj);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_close_all_asset_editors(PyObject * self, PyObject * args)
{
	FAssetEditorManager::Get().CloseAllAssetEditors();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_set_fbx_import_option(PyObject * self, PyObject * args)
{
	PyObject *obj;

	if (!PyArg_ParseTuple(args, "O:set_fbx_import_option", &obj))
	{
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

PyObject *py_unreal_engine_create_blueprint(PyObject * self, PyObject * args)
{

	PyObject *py_parent;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:create_blueprint", &py_parent, &name))
	{
		return NULL;
	}

	UClass *parent = UObject::StaticClass();

	if (py_parent != Py_None)
	{
		if (!ue_is_pyuobject(py_parent))
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		ue_PyUObject *py_obj = (ue_PyUObject *)py_parent;
		if (!py_obj->ue_object->IsA<UClass>())
			return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
		parent = (UClass *)py_obj->ue_object;
	}

	if (name[0] != '/')
	{
		return PyErr_Format(PyExc_Exception, "path must start with a /");
	}

	char *bp_name = strrchr(name, '/') + 1;
	if (strlen(bp_name) < 1)
	{
		return PyErr_Format(PyExc_Exception, "invalid blueprint name");
	}

	UPackage *outer = CreatePackage(nullptr, UTF8_TO_TCHAR(name));
	if (!outer)
		return PyErr_Format(PyExc_Exception, "unable to create package");

	if (FindObject<UBlueprint>(outer, UTF8_TO_TCHAR(bp_name)) != nullptr)
		return PyErr_Format(PyExc_Exception, "there is already a Blueprint with this name");

	UBlueprint *bp = FKismetEditorUtilities::CreateBlueprint(parent, outer, UTF8_TO_TCHAR(bp_name), EBlueprintType::BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
	if (!bp)
		return PyErr_Format(PyExc_Exception, "unable to create Blueprint");

	FAssetRegistryModule::AssetCreated(bp);
	outer->MarkPackageDirty();


	Py_RETURN_UOBJECT(bp);
}

PyObject *py_unreal_engine_get_blueprint_hierarchy_from_class(PyObject * self, PyObject * args)
{
	PyObject *py_class;
	if (!PyArg_ParseTuple(args, "O:get_blueprint_hierarchy_from_class", &py_class))
	{
		return NULL;
	}

	UClass* u_class = ue_py_check_type<UClass>(py_class);
	if (!u_class)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");
	}


	TArray<UBlueprint*> outBPs;
	UBlueprint::GetBlueprintHierarchyFromClass(u_class, outBPs);

	PyObject *py_bpClasses = PyList_New(0);

	for (UBlueprint* bpClass : outBPs)
	{
		ue_PyUObject *item = ue_get_python_uobject(bpClass);
		if (item)
			PyList_Append(py_bpClasses, (PyObject *)item);
	}
	return py_bpClasses;
}

PyObject *py_unreal_engine_reload_blueprint(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	if (!PyArg_ParseTuple(args, "O:reload_blueprint", &py_blueprint))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	UBlueprint *reloaded_bp = FKismetEditorUtilities::ReloadBlueprint(bp);

	Py_RETURN_UOBJECT(reloaded_bp);
}

PyObject *py_unreal_engine_compile_blueprint(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	if (!PyArg_ParseTuple(args, "O:compile_blueprint", &py_blueprint))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
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

PyObject *py_unreal_engine_replace_blueprint(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	PyObject *py_blueprint_new;
	if (!PyArg_ParseTuple(args, "OO:replace_blueprint", &py_blueprint, &py_blueprint_new))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	if (!ue_is_pyuobject(py_blueprint_new))
	{
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

	Py_RETURN_UOBJECT(replaced_bp);
}

PyObject *py_unreal_engine_create_blueprint_from_actor(PyObject * self, PyObject * args)
{

	PyObject *py_actor;
	char *name;
	if (!PyArg_ParseTuple(args, "sO:create_blueprint_from_actor", &name, &py_actor))
	{
		return NULL;
	}

	if (name[0] != '/')
	{
		return PyErr_Format(PyExc_Exception, "path must start with a /");
	}

	if (!ue_is_pyuobject(py_actor))
	{
		return PyErr_Format(PyExc_Exception, "argument is not an Actor");
	}
	ue_PyUObject *py_obj = (ue_PyUObject *)py_actor;
	if (!py_obj->ue_object->IsA<AActor>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
	AActor *actor = (AActor *)py_obj->ue_object;

	UBlueprint *bp = FKismetEditorUtilities::CreateBlueprintFromActor(UTF8_TO_TCHAR(name), actor, true);

	Py_RETURN_UOBJECT(bp);
}

PyObject *py_unreal_engine_get_blueprint_components(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;

	if (!PyArg_ParseTuple(args, "O|:add_component_to_blueprint", &py_blueprint))
	{
		return nullptr;
	}

	UBlueprint *bp = ue_py_check_type<UBlueprint>(py_blueprint);
	if (!bp)
		return PyErr_Format(PyExc_Exception, "uobject is not a Blueprint");

	PyObject *py_list = PyList_New(0);

	for (USCS_Node *node : bp->SimpleConstructionScript->GetAllNodes())
	{

		ue_PyUObject *item = ue_get_python_uobject(node->ComponentTemplate);
		if (item)
			PyList_Append(py_list, (PyObject *)item);

	}
	return py_list;

}

PyObject *py_unreal_engine_add_component_to_blueprint(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	PyObject *py_component;
	char *name;
	char *parentName = nullptr;

	if (!PyArg_ParseTuple(args, "OOs|s:add_component_to_blueprint", &py_blueprint, &py_component, &name, &parentName))
	{
		return NULL;
	}


	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	if (!ue_is_pyuobject(py_component))
	{
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
	if (!node)
	{
		return PyErr_Format(PyExc_Exception, "unable to allocate new component");
	}

	USCS_Node *parentNode = nullptr;

	if (component_class->IsChildOf<USceneComponent>())
	{

		if (parentName)
		{
			FString strParentName = UTF8_TO_TCHAR(parentName);
			if (strParentName.IsEmpty())
			{
				parentNode = bp->SimpleConstructionScript->GetDefaultSceneRootNode();
			}
			else
			{
				parentNode = bp->SimpleConstructionScript->FindSCSNode(UTF8_TO_TCHAR(parentName));
			}
		}
		else
		{
			parentNode = bp->SimpleConstructionScript->GetDefaultSceneRootNode();
		}
	}

	if (parentNode)
	{
		parentNode->AddChildNode(node);
	}
	else
	{
		bp->SimpleConstructionScript->AddNode(node);
	}

	Py_RETURN_UOBJECT(node->ComponentTemplate);
}

PyObject *py_unreal_engine_blueprint_add_member_variable(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	char *name;
	char *in_type;
	PyObject *py_is_array = nullptr;
	if (!PyArg_ParseTuple(args, "Oss|O:blueprint_add_member_variable", &py_blueprint, &name, &in_type, &py_is_array))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	bool is_array = false;
	if (py_is_array && PyObject_IsTrue(py_is_array))
		is_array = true;
#if ENGINE_MINOR_VERSION > 14
	FEdGraphPinType pin;
	pin.PinCategory = UTF8_TO_TCHAR(in_type);
#if ENGINE_MINOR_VERSION >= 17
	pin.ContainerType = is_array ? EPinContainerType::Array : EPinContainerType::None;
#else
	pin.bIsArray = is_array;
#endif
#else
	FEdGraphPinType pin(UTF8_TO_TCHAR(in_type), FString(""), nullptr, is_array, false);
#endif

	if (FBlueprintEditorUtils::AddMemberVariable(bp, UTF8_TO_TCHAR(name), pin))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_unreal_engine_blueprint_set_variable_visibility(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	char *name;
	PyObject *visibility;
	if (!PyArg_ParseTuple(args, "OsO:blueprint_set_variable_visibility", &py_blueprint, &name, &visibility))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	bool visible = false;
	if (PyObject_IsTrue(visibility))
	{
		visible = true;
	}

	FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(bp, FName(UTF8_TO_TCHAR(name)), !visible);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_blueprint_add_new_timeline(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:blueprint_add_new_timeline", &py_blueprint, &name))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	UTimelineTemplate *timeline = FBlueprintEditorUtils::AddNewTimeline(bp, UTF8_TO_TCHAR(name));
	if (!timeline)
	{
		return PyErr_Format(PyExc_Exception, "unable to add new timeline %s", name);
	}

	Py_RETURN_UOBJECT(timeline);
}

PyObject *py_unreal_engine_blueprint_add_function(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:blueprint_add_function", &py_blueprint, &name))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	UEdGraph *graph = FBlueprintEditorUtils::CreateNewGraph(bp, FName(UTF8_TO_TCHAR(name)), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	FBlueprintEditorUtils::AddFunctionGraph<UClass>(bp, graph, true, nullptr);

	Py_RETURN_UOBJECT(graph);
}

PyObject *py_unreal_engine_blueprint_add_event_dispatcher(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:blueprint_add_event_dispatcher", &py_blueprint, &name))
	{
		return NULL;
	}

	UBlueprint *bp = ue_py_check_type<UBlueprint>(py_blueprint);
	if (!bp)
		return PyErr_Format(PyExc_Exception, "argument is not a UBlueprint");

	FName multicast_name = FName(UTF8_TO_TCHAR(name));

	bp->Modify();

	const UEdGraphSchema_K2 *state = GetDefault<UEdGraphSchema_K2>();

	FEdGraphPinType multicast_type;
	multicast_type.PinCategory = state->PC_MCDelegate;

	if (!FBlueprintEditorUtils::AddMemberVariable(bp, multicast_name, multicast_type))
	{
		return PyErr_Format(PyExc_Exception, "unable to create multicast variable");
	}

	UEdGraph *graph = FBlueprintEditorUtils::CreateNewGraph(bp, multicast_name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	if (!graph)
	{
		return PyErr_Format(PyExc_Exception, "unable to create graph");
	}

	graph->bEditable = false;

	state->CreateDefaultNodesForGraph(*graph);
	state->CreateFunctionGraphTerminators(*graph, (UClass *)nullptr);
	state->AddExtraFunctionFlags(graph, (FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public));
	state->MarkFunctionEntryAsEditable(graph, true);

	bp->DelegateSignatureGraphs.Add(graph);
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

	Py_RETURN_UOBJECT(graph);
}

PyObject *py_unreal_engine_blueprint_mark_as_structurally_modified(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	if (!PyArg_ParseTuple(args, "O:blueprint_add_event_dispatcher", &py_blueprint))
	{
		return NULL;
	}

	UBlueprint *bp = ue_py_check_type<UBlueprint>(py_blueprint);
	if (!bp)
		return PyErr_Format(PyExc_Exception, "argument is not a UBlueprint");

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_blueprint_add_ubergraph_page(PyObject * self, PyObject * args)
{

	PyObject *py_blueprint;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:blueprint_add_ubergraph_page", &py_blueprint, &name))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_blueprint))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_blueprint;
	if (!py_obj->ue_object->IsA<UBlueprint>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");
	UBlueprint *bp = (UBlueprint *)py_obj->ue_object;

	UEdGraph *graph = FBlueprintEditorUtils::CreateNewGraph(bp, FName(UTF8_TO_TCHAR(name)), UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	FBlueprintEditorUtils::AddUbergraphPage(bp, graph);

	Py_RETURN_UOBJECT(graph);
}

PyObject *py_unreal_engine_blueprint_get_all_graphs(PyObject * self, PyObject * args)
{
	PyObject *py_blueprint;

	if (!PyArg_ParseTuple(args, "O:blueprint_get_all_graphs", &py_blueprint))
	{
		return nullptr;
	}

	UBlueprint *bp = ue_py_check_type<UBlueprint>(py_blueprint);
	if (!bp)
		return PyErr_Format(PyExc_Exception, "uobject is not a UBlueprint");

	PyObject *py_graphs = PyList_New(0);

	TArray<UEdGraph*> graphs;

	bp->GetAllGraphs(graphs);

	for (UEdGraph *graph : graphs)
	{
		ue_PyUObject *item = ue_get_python_uobject(graph);
		if (item)
			PyList_Append(py_graphs, (PyObject *)item);
	}

	return py_graphs;
}

PyObject *py_unreal_engine_create_new_graph(PyObject * self, PyObject * args)
{

	PyObject *py_object;
	char *name;
	PyObject *py_graph_class;
	PyObject *py_graph_schema;
	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "OsOO|O:create_new_graph", &py_object, &name, &py_graph_class, &py_graph_schema, &py_bool))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_object))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}
	ue_PyUObject *py_obj = (ue_PyUObject *)py_object;

	if (!ue_is_pyuobject(py_graph_class))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}
	ue_PyUObject *py_class_obj = (ue_PyUObject *)py_graph_class;
	if (!py_class_obj->ue_object->IsA<UClass>())
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");
	}
	UClass *u_class = (UClass *)py_class_obj->ue_object;
	if (!u_class->IsChildOf<UEdGraph>())
	{
		return PyErr_Format(PyExc_Exception, "argument is not a child of UEdGraph");
	}

	if (!ue_is_pyuobject(py_graph_schema))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}
	ue_PyUObject *py_class_schema = (ue_PyUObject *)py_graph_schema;
	if (!py_class_schema->ue_object->IsA<UClass>())
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");
	}
	UClass *u_class_schema = (UClass *)py_class_schema->ue_object;
	if (!u_class_schema->IsChildOf<UEdGraphSchema>())
	{
		return PyErr_Format(PyExc_Exception, "argument is not a child of UEdGraphSchema");
	}

	UEdGraph *graph = FBlueprintEditorUtils::CreateNewGraph(py_obj->ue_object, FName(UTF8_TO_TCHAR(name)), u_class, u_class_schema);
	if (!graph)
	{
		return PyErr_Format(PyExc_Exception, "unable to create graph");
	}

	if (py_bool && PyObject_IsTrue(py_bool))
	{
		graph->GetSchema()->CreateDefaultNodesForGraph(*graph);
	}

	Py_RETURN_UOBJECT(graph);
}

PyObject *py_unreal_engine_editor_on_asset_post_import(PyObject * self, PyObject * args)
{
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:editor_on_asset_post_import", &py_callable))
	{
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "object is not a callable");

	TSharedRef<FPythonSmartDelegate> py_delegate = MakeShareable(new FPythonSmartDelegate);
	py_delegate->SetPyCallable(py_callable);
	FEditorDelegates::OnAssetPostImport.AddSP(py_delegate, &FPythonSmartDelegate::PyFOnAssetPostImport);
	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_create_material_instance(PyObject * self, PyObject * args)
{

	PyObject *py_material;
	char *materialPacakgePath = nullptr;
	char *materialName = nullptr;

	if (!PyArg_ParseTuple(args, "O|ss:create_material_instance", &py_material, &materialPacakgePath, &materialName))
	{
		return NULL;
	}

	if (!ue_is_pyuobject(py_material))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)py_material;
	if (!py_obj->ue_object->IsA<UMaterialInterface>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UMaterialInterface");
	UMaterialInterface *materialInterface = (UMaterialInterface *)py_obj->ue_object;

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

	// Determine an appropriate name
	FString Name;
	FString PackagePath;
	FString PackageName;

	// Create the factory used to generate the asset
	UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	Factory->InitialParent = materialInterface;

	if (materialPacakgePath && materialName)
	{
		Name = UTF8_TO_TCHAR(materialName);
		PackagePath = UTF8_TO_TCHAR(materialPacakgePath);
	}
	else
	{
		AssetToolsModule.Get().CreateUniqueAssetName(materialInterface->GetOutermost()->GetName(), UTF8_TO_TCHAR("_inst"), PackageName, Name);
		PackagePath = FPackageName::GetLongPackagePath(PackageName);
	}

	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(Name, PackagePath, UMaterialInstanceConstant::StaticClass(), Factory);
	if (!NewAsset)
		return PyErr_Format(PyExc_Exception, "unable to create new asset");

	Py_RETURN_UOBJECT(NewAsset);
}

PyObject *py_ue_factory_create_new(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:factory_create_new", &name))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UFactory>())
		return PyErr_Format(PyExc_Exception, "uobject is not a Factory");

	UPackage *outer = CreatePackage(nullptr, UTF8_TO_TCHAR(name));
	if (!outer)
		return PyErr_Format(PyExc_Exception, "unable to create package");

	UFactory *factory = (UFactory *)self->ue_object;
	UClass *u_class = factory->GetSupportedClass();

	char *obj_name = strrchr(name, '/') + 1;
	if (strlen(obj_name) < 1)
	{
		return PyErr_Format(PyExc_Exception, "invalid object name");
	}

	UObject *u_object = factory->FactoryCreateNew(u_class, outer, FName(UTF8_TO_TCHAR(obj_name)), RF_Public | RF_Standalone, nullptr, GWarn);

	if (!u_object)
		return PyErr_Format(PyExc_Exception, "unable to create new object from factory");

	FAssetRegistryModule::AssetCreated(u_object);
	outer->MarkPackageDirty();

	Py_RETURN_UOBJECT(u_object);
}

PyObject *py_ue_factory_import_object(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *filename = nullptr;
	char *name = nullptr;
	if (!PyArg_ParseTuple(args, "ss:factory_import_object", &filename, &name))
	{
		return NULL;
	}

	if (!self->ue_object->IsA<UFactory>())
		return PyErr_Format(PyExc_Exception, "uobject is not a Factory");

	UFactory *factory = (UFactory *)self->ue_object;

	FString object_name = ObjectTools::SanitizeObjectName(FPaths::GetBaseFilename(UTF8_TO_TCHAR(filename)));
	FString pkg_name = FString(UTF8_TO_TCHAR(name)) + TEXT("/") + object_name;

	UPackage *outer = CreatePackage(nullptr, *pkg_name);
	if (!outer)
		return PyErr_Format(PyExc_Exception, "unable to create package");

	bool canceled = false;
	UObject *u_object = factory->ImportObject(factory->ResolveSupportedClass(), outer, FName(*object_name), RF_Public | RF_Standalone, UTF8_TO_TCHAR(filename), nullptr, canceled);

	if (!u_object)
		return PyErr_Format(PyExc_Exception, "unable to create new object from factory");

	FAssetRegistryModule::AssetCreated(u_object);
	outer->MarkPackageDirty();

	Py_RETURN_UOBJECT(u_object);
}

PyObject *py_unreal_engine_add_level_to_world(PyObject *self, PyObject * args)
{

	PyObject *py_world;
	char *name;
	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "Os|O:add_level_to_world", &py_world, &name, &py_bool))
	{
		return NULL;
	}

	UWorld *u_world = ue_py_check_type<UWorld>(py_world);
	if (!u_world)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UWorld");
	}

	UClass *streaming_mode_class = ULevelStreamingKismet::StaticClass();
	if (py_bool && PyObject_IsTrue(py_bool))
	{
		streaming_mode_class = ULevelStreamingAlwaysLoaded::StaticClass();
	}

#if ENGINE_MINOR_VERSION >= 17
	ULevelStreaming *level_streaming = EditorLevelUtils::AddLevelToWorld(u_world, UTF8_TO_TCHAR(name), streaming_mode_class);
#else
	ULevel *level_streaming = EditorLevelUtils::AddLevelToWorld(u_world, UTF8_TO_TCHAR(name), streaming_mode_class);
#endif
	if (!level_streaming)
	{
		return PyErr_Format(PyExc_Exception, "unable to add \"%s\" to the world", name);
	}

#if ENGINE_MINOR_VERSION >= 16
	FEditorDelegates::RefreshLevelBrowser.Broadcast();
#endif

	Py_RETURN_UOBJECT(level_streaming);
}

PyObject *py_unreal_engine_move_selected_actors_to_level(PyObject *self, PyObject * args)
{

	PyObject *py_level;
	if (!PyArg_ParseTuple(args, "O:move_selected_actors_to_level", &py_level))
	{
		return NULL;
	}

	ULevel *level = ue_py_check_type<ULevel>(py_level);
	if (!level)
		return PyErr_Format(PyExc_Exception, "argument is not a ULevel");

#if ENGINE_MINOR_VERSION >= 17
	UEditorLevelUtils::MoveSelectedActorsToLevel(level);
#else
	GEditor->MoveSelectedActorsToLevel(level);
#endif

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_move_actor_to_level(PyObject *self, PyObject * args)
{
	PyObject *py_actor;
	PyObject *py_level;
	if (!PyArg_ParseTuple(args, "OO:move_actor_to_level", &py_actor, &py_level))
	{
		return NULL;
	}

	AActor *actor = ue_py_check_type<AActor>(py_actor);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "argument is not an Actor");

	ULevel *level = ue_py_check_type<ULevel>(py_level);
	if (!level)
		return PyErr_Format(PyExc_Exception, "argument is not a ULevelStreaming");

	TArray<AActor *> actors;
	actors.Add(actor);

	int out = 0;

#if ENGINE_MINOR_VERSION >= 17
	out = EditorLevelUtils::MoveActorsToLevel(actors, level);
#endif
	if (out != 1)
	{
		return PyErr_Format(PyExc_Exception, "unable to move actor to level");
	}

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_editor_take_high_res_screen_shots(PyObject * self, PyObject * args)
{

	GEditor->TakeHighResScreenShots();

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_begin_transaction(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	char *description;
	if (!PyArg_ParseTuple(args, "s:begin_transaction", &description))
	{
		return NULL;
	}

	int ret = GEditor->BeginTransaction(FText::FromString(UTF8_TO_TCHAR(description)));

	return PyLong_FromLong(ret);
}

PyObject *py_unreal_engine_cancel_transaction(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	int tid;
	if (!PyArg_ParseTuple(args, "i:cancel_transaction", &tid))
	{
		return nullptr;
	}

	GEditor->CancelTransaction(tid);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_end_transaction(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");


	int ret = GEditor->EndTransaction();

	return PyLong_FromLong(ret);
}

PyObject *py_unreal_engine_get_transaction_name(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");


	FText text = GEditor->GetTransactionName();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*(text.ToString())));
}

PyObject *py_unreal_engine_is_transaction_active(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");


	bool is_active = GEditor->IsTransactionActive();

	if (is_active)
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_unreal_engine_redo_transaction(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");


	bool redo = GEditor->RedoTransaction();

	if (redo)
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_unreal_engine_reset_transaction(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	char *reason;
	if (!PyArg_ParseTuple(args, "s:reset_transaction", &reason))
	{
		return NULL;
	}

	GEditor->ResetTransaction(FText::FromString(UTF8_TO_TCHAR(reason)));

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_editor_undo(PyObject * self, PyObject * args)
{

	if (!GEditor || !GEditor->Trans)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	if (GEditor->Trans->Undo())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_unreal_engine_editor_redo(PyObject * self, PyObject * args)
{

	if (!GEditor || !GEditor->Trans)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	if (GEditor->Trans->Redo())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}


PyObject *py_unreal_engine_transactions(PyObject * self, PyObject * args)
{

	if (!GEditor || !GEditor->Trans)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *transactions = PyList_New(0);


	int32 nums = GEditor->Trans->GetQueueLength();

	for (int32 i = 0; i < nums; i++)
	{
		const FTransaction *transaction = GEditor->Trans->GetTransaction(i);
		PyList_Append(transactions, PyUnicode_FromString(TCHAR_TO_UTF8(*transaction->GetTitle().ToString())));
	}

	return transactions;
}

PyObject *py_unreal_engine_heightmap_expand(PyObject * self, PyObject * args)
{
	Py_buffer buf;
	int width;
	int height;
	int new_width;
	int new_height;
	if (!PyArg_ParseTuple(args, "y*iiii:heightmap_expand", &buf, &width, &height, &new_width, &new_height))
		return nullptr;

	uint16 *original_data_buf = (uint16 *)buf.buf;

	TArray<uint16> original_data;
	original_data.AddZeroed(buf.len / 2);

	FMemory::Memcpy(original_data.GetData(), original_data_buf, buf.len);

	int offset_x = (new_width - width) / 2;
	int offset_y = (new_height - height) / 2;

	TArray<uint16> data = LandscapeEditorUtils::ExpandData<uint16>(original_data, 0, 0, width - 1, height - 1, -offset_x, -offset_y, new_width - offset_x - 1, new_height - offset_y - 1);

	return PyByteArray_FromStringAndSize((char *)data.GetData(), data.Num() * sizeof(uint16));

}

PyObject *py_unreal_engine_heightmap_import(PyObject * self, PyObject * args)
{
	char *filename;
	int width = 0;
	int height = 0;
	if (!PyArg_ParseTuple(args, "s|ii:heightmap_import", &filename, &width, &height))
		return nullptr;

	ILandscapeEditorModule & LandscapeModule = FModuleManager::GetModuleChecked<ILandscapeEditorModule>("LandscapeEditor");
	const ILandscapeHeightmapFileFormat *format = LandscapeModule.GetHeightmapFormatByExtension(*FPaths::GetExtension(UTF8_TO_TCHAR(filename), true));

	if (!format)
	{
		return PyErr_Format(PyExc_Exception, "invalid heightmap format");
	}

	FLandscapeFileResolution resolution = { (uint32)width, (uint32)height };

	if (width <= 0 || height <= 0)
	{
		FLandscapeHeightmapInfo info = format->Validate(UTF8_TO_TCHAR(filename));
		if (info.ResultCode == ELandscapeImportResult::Error)
		{
			return PyErr_Format(PyExc_Exception, "unable to import heightmap: %s", TCHAR_TO_UTF8(*info.ErrorMessage.ToString()));
		}

		if (info.ResultCode == ELandscapeImportResult::Warning)
		{
			UE_LOG(LogPython, Warning, TEXT("%s"), *info.ErrorMessage.ToString());
		}

		if (info.PossibleResolutions.Num() < 1)
		{
			return PyErr_Format(PyExc_Exception, "unable to retrieve heightmap resolution");
		}

		resolution = info.PossibleResolutions[0];
	}

	FLandscapeHeightmapImportData imported = format->Import(UTF8_TO_TCHAR(filename), resolution);

	if (imported.ResultCode == ELandscapeImportResult::Error)
	{
		return PyErr_Format(PyExc_Exception, "unable to import heightmap: %s", TCHAR_TO_UTF8(*imported.ErrorMessage.ToString()));
	}

	if (imported.ResultCode == ELandscapeImportResult::Warning)
	{
		UE_LOG(LogPython, Warning, TEXT("%s"), *imported.ErrorMessage.ToString());
	}

	return Py_BuildValue((char *)"Oii", PyByteArray_FromStringAndSize((char *)imported.Data.GetData(), imported.Data.Num() * sizeof(uint16)), resolution.Width, resolution.Height);
}

PyObject *py_unreal_engine_play_preview_sound(PyObject * self, PyObject * args)
{
	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject *py_sound;
	if (!PyArg_ParseTuple(args, "O:play_preview_sound", &py_sound))
		return nullptr;

	USoundBase *sound = ue_py_check_type<USoundBase>(py_sound);
	if (!sound)
		return PyErr_Format(PyExc_Exception, "argument is not a USoundBase");

	GEditor->PlayPreviewSound(sound);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_register_settings(PyObject * self, PyObject * args)
{


	char *container_name;
	char *category_name;
	char *section_name;
	char *display_name;
	char *description;
	PyObject *py_uobject;

	if (!PyArg_ParseTuple(args, "sssssO:register_settings", &container_name, &category_name, &section_name, &display_name, &description, &py_uobject))
		return nullptr;

	UObject *u_object = ue_py_check_type<UObject>(py_uobject);
	if (!u_object)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		TSharedPtr<ISettingsSection> section = SettingsModule->RegisterSettings(UTF8_TO_TCHAR(container_name),
			UTF8_TO_TCHAR(category_name),
			UTF8_TO_TCHAR(section_name),
			FText::FromString(UTF8_TO_TCHAR(display_name)),
			FText::FromString(UTF8_TO_TCHAR(description)),
			u_object);

		if (!section.IsValid())
			return PyErr_Format(PyExc_Exception, "unable to register settings");
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "unable to find the Settings Module");
	}

	Py_RETURN_NONE;
}

PyObject * py_unreal_engine_show_viewer(PyObject * self, PyObject * args)
{
	char *container_name;
	char *category_name;
	char *section_name;

	if (!PyArg_ParseTuple(args, "sss:register_settings", &container_name, &category_name, &section_name))
		return nullptr;

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->ShowViewer(container_name, category_name, section_name);
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "unable to find the Settings Module");
	}

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_unregister_settings(PyObject * self, PyObject * args)
{
	char *container_name;
	char *category_name;
	char *section_name;

	if (!PyArg_ParseTuple(args, "sss:unregister_settings", &container_name, &category_name, &section_name))
		return nullptr;

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings(UTF8_TO_TCHAR(container_name),
			UTF8_TO_TCHAR(category_name),
			UTF8_TO_TCHAR(section_name));
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "unable to find the Settings Module");
	}

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_all_viewport_clients(PyObject * self, PyObject * args)
{
	TArray<FEditorViewportClient *> clients = GEditor->AllViewportClients;
	PyObject *py_list = PyList_New(0);
	for (FEditorViewportClient *client : clients)
	{
		PyList_Append(py_list, py_ue_new_feditor_viewport_client(TSharedRef<FEditorViewportClient>(client)));
	}
	return py_list;
}

PyObject *py_unreal_engine_editor_sync_browser_to_assets(PyObject * self, PyObject * args)
{
	PyObject *py_items;
	PyObject *py_focus = nullptr;

	if (!PyArg_ParseTuple(args, "O|O:sync_browser_to_assets", &py_items, &py_focus))
		return nullptr;

	PyObject *py_iter = PyObject_GetIter(py_items);
	if (!py_iter)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of UObject or FAssetData");
	}

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	TArray<FAssetData> asset_data;
	TArray<UObject *> uobjects;

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		ue_PyFAssetData *py_data = py_ue_is_fassetdata(py_item);
		if (py_data)
		{
			asset_data.Add(py_data->asset_data);
		}
		else
		{
			UObject *u_object = ue_py_check_type<UObject>(py_item);
			if (!u_object)
			{
				return PyErr_Format(PyExc_Exception, "invalid item in iterable, must be UObject or FAssetData");
			}
			uobjects.Add(u_object);
		}
	}

	if (asset_data.Num() > 0)
	{
		ContentBrowserModule.Get().SyncBrowserToAssets(asset_data, false, py_focus && PyObject_IsTrue(py_focus));
	}

	if (uobjects.Num() > 0)
	{
		ContentBrowserModule.Get().SyncBrowserToAssets(uobjects, false, py_focus && PyObject_IsTrue(py_focus));
	}

	Py_DECREF(py_iter);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_export_assets(PyObject * self, PyObject * args)
{

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	PyObject * py_assets = nullptr;
	char *filename;

	if (!PyArg_ParseTuple(args, "Os:export_assets", &py_assets, &filename))
	{
		return nullptr;
	}

	TArray<UObject *> UObjects;
	PyObject *py_iter = PyObject_GetIter(py_assets);

	if (!py_iter)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable of UObject");
	}

	while (PyObject *py_item = PyIter_Next(py_iter))
	{
		UObject *Object = ue_py_check_type<UObject>(py_item);
		if (!Object)
		{
			Py_DECREF(py_iter);
			return PyErr_Format(PyExc_Exception, "argument is not an iterable of UObject");
		}
		UObjects.Add(Object);
	}

	Py_DECREF(py_iter);

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
#if ENGINE_MINOR_VERSION > 16
	AssetToolsModule.Get().ExportAssets(UObjects, FString(UTF8_TO_TCHAR(filename)));
#endif

	Py_RETURN_NONE;
}
#endif

