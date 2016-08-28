#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Editor/UnrealEd/Classes/Factories/Factory.h"


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
#endif

