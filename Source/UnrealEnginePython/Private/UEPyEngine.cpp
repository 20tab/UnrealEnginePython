#include "UnrealEnginePythonPrivatePCH.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

PyObject *py_unreal_engine_log(PyObject * self, PyObject * args) {
	PyObject *py_message;
	if (!PyArg_ParseTuple(args, "O:log", &py_message)) {
		return NULL;
	}

	PyObject *stringified = PyObject_Str(py_message);
	if (!stringified)
		return PyErr_Format(PyExc_Exception, "argument cannot be casted to string");
	char *message = PyUnicode_AsUTF8(stringified);
	UE_LOG(LogPython, Log, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_DECREF(stringified);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_log_warning(PyObject * self, PyObject * args) {
	PyObject *py_message;
	if (!PyArg_ParseTuple(args, "O:log_warning", &py_message)) {
		return NULL;
	}

	PyObject *stringified = PyObject_Str(py_message);
	if (!stringified)
		return PyErr_Format(PyExc_Exception, "argument cannot be casted to string");
	char *message = PyUnicode_AsUTF8(stringified);
	UE_LOG(LogPython, Warning, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_DECREF(stringified);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_log_error(PyObject * self, PyObject * args) {
	PyObject *py_message;
	if (!PyArg_ParseTuple(args, "O:log_error", &py_message)) {
		return NULL;
	}

	PyObject *stringified = PyObject_Str(py_message);
	if (!stringified)
		return PyErr_Format(PyExc_Exception, "argument cannot be casted to string");
	char *message = PyUnicode_AsUTF8(stringified);
	UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_DECREF(stringified);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_add_on_screen_debug_message(PyObject * self, PyObject * args) {
	int key;
	float time_to_display;
	PyObject *py_message;
	if (!PyArg_ParseTuple(args, "ifO:add_on_screen_debug_message", &key, &time_to_display, &py_message)) {
		return NULL;
	}

	if (!GEngine) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *stringified = PyObject_Str(py_message);
	if (!stringified)
		return PyErr_Format(PyExc_Exception, "argument cannot be casted to string");
	char *message = PyUnicode_AsUTF8(stringified);

	GEngine->AddOnScreenDebugMessage(key, time_to_display, FColor::Green, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

	Py_DECREF(stringified);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_print_string(PyObject * self, PyObject * args) {

	PyObject *py_message;
	if (!PyArg_ParseTuple(args, "O:print_string", &py_message)) {
		return NULL;
	}

	if (!GEngine) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *stringified = PyObject_Str(py_message);
	if (!stringified)
		return PyErr_Format(PyExc_Exception, "argument cannot be casted to string");
	char *message = PyUnicode_AsUTF8(stringified);

	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Cyan, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

	Py_DECREF(stringified);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_get_forward_vector(PyObject * self, PyObject * args) {
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;
	FVector vec = UKismetMathLibrary::GetForwardVector(rot);
	return py_ue_new_fvector(vec);
}

PyObject *py_unreal_engine_get_right_vector(PyObject * self, PyObject * args) {
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;
	FVector vec = UKismetMathLibrary::GetRightVector(rot);
	return py_ue_new_fvector(vec);
}

PyObject *py_unreal_engine_get_up_vector(PyObject * self, PyObject * args) {
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;
	FVector vec = UKismetMathLibrary::GetUpVector(rot);
	return py_ue_new_fvector(vec);
}

PyObject *py_unreal_engine_get_content_dir(PyObject * self, PyObject * args) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*FPaths::GameContentDir()));
}

PyObject *py_unreal_engine_convert_relative_path_to_full(PyObject * self, PyObject * args) {
	char *path;
	if (!PyArg_ParseTuple(args, "s:convert_relative_path_to_full", &path)) {
		return NULL;
	}
	return PyUnicode_FromString(TCHAR_TO_UTF8(*FPaths::ConvertRelativePathToFull(UTF8_TO_TCHAR(path))));
}

PyObject *py_unreal_engine_find_class(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_class", &name)) {
		return NULL;
	}

	UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (!u_class)
		return PyErr_Format(PyExc_Exception, "unable to find class %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_class);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_find_enum(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_enum", &name)) {
		return NULL;
	}

	UEnum *u_enum = FindObject<UEnum>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (!u_enum)
		return PyErr_Format(PyExc_Exception, "unable to find enum %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_enum);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_unreal_engine_load_class(PyObject * self, PyObject * args) {
	char *name;
	char *filename = nullptr;
	if (!PyArg_ParseTuple(args, "s|s:load_class", &name, &filename)) {
		return NULL;
	}

	TCHAR *t_filename = (TCHAR *)0;
	if (filename)
		t_filename = UTF8_TO_TCHAR(filename);

	UObject *u_class = StaticLoadObject(UClass::StaticClass(), NULL, UTF8_TO_TCHAR(name), t_filename);

	if (!u_class)
		return PyErr_Format(PyExc_Exception, "unable to find class %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_class);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_load_enum(PyObject * self, PyObject * args) {
	char *name;
	char *filename = nullptr;
	if (!PyArg_ParseTuple(args, "s|s:load_enum", &name, &filename)) {
		return NULL;
	}

	TCHAR *t_filename = (TCHAR *)0;
	if (filename)
		t_filename = UTF8_TO_TCHAR(filename);

	UObject *u_enum = StaticLoadObject(UEnum::StaticClass(), NULL, UTF8_TO_TCHAR(name), t_filename);

	if (!u_enum)
		return PyErr_Format(PyExc_Exception, "unable to find enum %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_enum);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_find_struct(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_struct", &name)) {
		return NULL;
	}

	UStruct *u_struct = FindObject<UStruct>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "unable to find struct %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_struct);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}

PyObject *py_unreal_engine_load_struct(PyObject * self, PyObject * args) {
	char *name;
	char *filename = nullptr;
	if (!PyArg_ParseTuple(args, "s|s:load_struct", &name, &filename)) {
		return NULL;
	}

	TCHAR *t_filename = (TCHAR *)0;
	if (filename)
		t_filename = UTF8_TO_TCHAR(filename);

	UObject *u_struct = StaticLoadObject(UStruct::StaticClass(), NULL, UTF8_TO_TCHAR(name), t_filename);

	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "unable to find struct %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_struct);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}


PyObject *py_unreal_engine_load_object(PyObject * self, PyObject * args) {
	PyObject *obj;
	char *name;
	char *filename = nullptr;
	if (!PyArg_ParseTuple(args, "Os|s:load_object", &obj, &name, &filename)) {
		return NULL;
	}

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;
	if (!py_obj->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");
	}

	UClass *u_class = (UClass *)py_obj->ue_object;

	TCHAR *t_filename = (TCHAR *)0;
	if (filename)
		t_filename = UTF8_TO_TCHAR(filename);

	UObject *u_object = StaticLoadObject(u_class, NULL, UTF8_TO_TCHAR(name), t_filename);

	if (!u_object)
		return PyErr_Format(PyExc_Exception, "unable to load object %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}


PyObject *py_unreal_engine_find_object(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_object", &name)) {
		return NULL;
	}

	UObject *u_object = FindObject<UObject>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (!u_object)
		return PyErr_Format(PyExc_Exception, "unable to find object %s", name);

	ue_PyUObject *ret = ue_get_python_wrapper(u_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}


PyObject *py_unreal_engine_new_object(PyObject * self, PyObject * args) {

	PyObject *obj;
	PyObject *py_outer = NULL;
	char *name = nullptr;
	if (!PyArg_ParseTuple(args, "O|Os:new_object", &obj, &py_outer, &name)) {
		return NULL;
	}

	if (!ue_is_pyuobject(obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "uobject is not a UClass");

	UClass *obj_class = (UClass *)py_obj->ue_object;

	FName f_name = NAME_None;

	if (name) {
		f_name = FName(UTF8_TO_TCHAR(name));
	}

	UObject *outer = GetTransientPackage();

	if (py_outer && py_outer != Py_None) {
		if (!ue_is_pyuobject(py_outer)) {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}

		ue_PyUObject *py_outer_obj = (ue_PyUObject *)py_outer;

		outer = py_outer_obj->ue_object;
	}

	UObject *new_object = NewObject<UObject>(outer, obj_class, f_name, RF_Public | RF_Standalone);
	if (!new_object)
		return PyErr_Format(PyExc_Exception, "unable to create object");

	new_object->PostLoad();

	ue_PyUObject *ret = ue_get_python_wrapper(new_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}


PyObject *py_unreal_engine_new_class(PyObject * self, PyObject * args) {

	PyObject *py_parent;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:new_class", &py_parent, &name)) {
		return NULL;
	}

	UClass *parent = nullptr;

	if (py_parent != Py_None) {
		if (!ue_is_pyuobject(py_parent)) {
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");
		}
		ue_PyUObject *py_obj = (ue_PyUObject *)py_parent;
		if (!py_obj->ue_object->IsA<UClass>())
			return PyErr_Format(PyExc_Exception, "uobject is not a UClass");
		parent = (UClass *)py_obj->ue_object;
	}

	UClass *new_object = unreal_engine_new_uclass(name, parent);
	if (!new_object)
		return PyErr_Format(PyExc_Exception, "unable to create UClass");

	ue_PyUObject *ret = ue_get_python_wrapper(new_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_classes(PyObject * self, PyObject * args) {

	PyObject *ret = PyList_New(0);

	for (TObjectIterator<UClass> Itr; Itr; ++Itr) {
		ue_PyUObject *py_obj = ue_get_python_wrapper(*Itr);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);
	}
	return ret;
}

PyObject *py_unreal_engine_create_and_dispatch_when_ready(PyObject * self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:create_and_dispatch_when_ready", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_TypeError, "argument is not callable");

	Py_INCREF(py_callable);

	FGraphEventRef task = FFunctionGraphTask::CreateAndDispatchWhenReady([&]() {
		FScopePythonGIL gil;
		PyObject *ret = PyObject_CallObject(py_callable, nullptr);
		if (ret) {
			Py_DECREF(ret);
		}
		else {
			unreal_engine_py_log_error();
		}
		Py_DECREF(py_callable);
	}, TStatId(), nullptr, ENamedThreads::GameThread);

	FTaskGraphInterface::Get().WaitUntilTaskCompletes(task);
	// TODO Implement signal triggering in addition to WaitUntilTaskCompletes
	// FTaskGraphInterface::Get().TriggerEventWhenTaskCompletes

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_get_viewport_screenshot(PyObject *self, PyObject * args) {

	if (!GEngine->GameViewport) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *py_bool = nullptr;
	bool as_int_list = false;
	if (!PyArg_ParseTuple(args, "|O:get_viewport_screenshot", &py_bool)) {
		return NULL;
	}

	if (py_bool && PyObject_IsTrue(py_bool))
		as_int_list = true;

	FViewport *viewport = GEngine->GameViewport->Viewport;
	TArray<FColor> bitmap;

	bool success = GetViewportScreenShot(viewport, bitmap);

	if (!success) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if (as_int_list) {
		PyObject *bitmap_tuple = PyTuple_New(bitmap.Num() * 4);
		for (int i = 0; i < bitmap.Num(); i++) {
			PyTuple_SetItem(bitmap_tuple, i * 4, PyLong_FromLong(bitmap[i].R));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 1, PyLong_FromLong(bitmap[i].G));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 2, PyLong_FromLong(bitmap[i].B));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 3, PyLong_FromLong(bitmap[i].A));
		}
		return bitmap_tuple;
	}

	PyObject *bitmap_tuple = PyTuple_New(bitmap.Num());
	for (int i = 0; i < bitmap.Num(); i++) {
		PyTuple_SetItem(bitmap_tuple, i, py_ue_new_fcolor(bitmap[i]));
	}

	return bitmap_tuple;
}

PyObject *py_unreal_engine_get_viewport_size(PyObject *self, PyObject * args) {

	if (!GEngine->GameViewport) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	FViewport *viewport = GEngine->GameViewport->Viewport;
	PyObject *tuple_size = PyTuple_New(2);
	FIntPoint point = viewport->GetSizeXY();

	PyTuple_SetItem(tuple_size, 0, PyLong_FromLong(point.X));
	PyTuple_SetItem(tuple_size, 1, PyLong_FromLong(point.Y));

	return tuple_size;
}

#if WITH_EDITOR
PyObject *py_unreal_engine_editor_get_active_viewport_screenshot(PyObject *self, PyObject * args) {

	FViewport *viewport = GEditor->GetActiveViewport();
	if (!viewport) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *py_bool = nullptr;
	bool as_int_list = false;
	if (!PyArg_ParseTuple(args, "|O:editor_get_active_viewport_screenshot", &py_bool)) {
		return NULL;
	}

	if (py_bool && PyObject_IsTrue(py_bool))
		as_int_list = true;

	TArray<FColor> bitmap;

	bool success = GetViewportScreenShot(viewport, bitmap);

	if (!success) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if (as_int_list) {
		PyObject *bitmap_tuple = PyTuple_New(bitmap.Num() * 4);
		for (int i = 0; i < bitmap.Num(); i++) {
			PyTuple_SetItem(bitmap_tuple, i * 4, PyLong_FromLong(bitmap[i].R));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 1, PyLong_FromLong(bitmap[i].G));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 2, PyLong_FromLong(bitmap[i].B));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 3, PyLong_FromLong(bitmap[i].A));
		}
		return bitmap_tuple;
	}

	PyObject *bitmap_tuple = PyTuple_New(bitmap.Num());
	for (int i = 0; i < bitmap.Num(); i++) {
		PyTuple_SetItem(bitmap_tuple, i, py_ue_new_fcolor(bitmap[i]));
	}

	return bitmap_tuple;
}

PyObject *py_unreal_engine_editor_get_active_viewport_size(PyObject *self, PyObject * args) {


	FViewport *viewport = GEditor->GetActiveViewport();
	if (!viewport) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *tuple_size = PyTuple_New(2);

	FIntPoint point = viewport->GetSizeXY();

	PyTuple_SetItem(tuple_size, 0, PyLong_FromLong(point.X));
	PyTuple_SetItem(tuple_size, 1, PyLong_FromLong(point.Y));

	return tuple_size;
}

PyObject *py_unreal_engine_editor_get_pie_viewport_screenshot(PyObject *self, PyObject * args) {

	FViewport *viewport = GEditor->GetPIEViewport();
	if (!viewport) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *py_bool = nullptr;
	bool as_int_list = false;
	if (!PyArg_ParseTuple(args, "|O:editor_get_pie_viewport_screenshot", &py_bool)) {
		return NULL;
	}

	if (py_bool && PyObject_IsTrue(py_bool))
		as_int_list = true;

	TArray<FColor> bitmap;

	bool success = GetViewportScreenShot(viewport, bitmap);

	if (!success) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if (as_int_list) {
		PyObject *bitmap_tuple = PyTuple_New(bitmap.Num() * 4);
		for (int i = 0; i < bitmap.Num(); i++) {
			PyTuple_SetItem(bitmap_tuple, i * 4, PyLong_FromLong(bitmap[i].R));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 1, PyLong_FromLong(bitmap[i].G));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 2, PyLong_FromLong(bitmap[i].B));
			PyTuple_SetItem(bitmap_tuple, i * 4 + 3, PyLong_FromLong(bitmap[i].A));
		}
		return bitmap_tuple;
	}

	PyObject *bitmap_tuple = PyTuple_New(bitmap.Num());
	for (int i = 0; i < bitmap.Num(); i++) {
		PyTuple_SetItem(bitmap_tuple, i, py_ue_new_fcolor(bitmap[i]));
	}

	return bitmap_tuple;
}

PyObject *py_unreal_engine_editor_get_pie_viewport_size(PyObject *self, PyObject * args) {


	FViewport *viewport = GEditor->GetPIEViewport();
	if (!viewport) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	PyObject *tuple_size = PyTuple_New(2);
	FIntPoint point = viewport->GetSizeXY();

	PyTuple_SetItem(tuple_size, 0, PyLong_FromLong(point.X));
	PyTuple_SetItem(tuple_size, 1, PyLong_FromLong(point.Y));
	return tuple_size;
}
#endif


