#include "UnrealEnginePythonPrivatePCH.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "PythonGeneratedClass.h"

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

	if (!GEngine)
		goto end;

	PyObject *stringified = PyObject_Str(py_message);
	if (!stringified)
		return PyErr_Format(PyExc_Exception, "argument cannot be casted to string");
	char *message = PyUnicode_AsUTF8(stringified);

	GEngine->AddOnScreenDebugMessage(key, time_to_display, FColor::Green, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

	Py_DECREF(stringified);

end:
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_print_string(PyObject * self, PyObject * args) {

	PyObject *py_message;
	if (!PyArg_ParseTuple(args, "O:print_string", &py_message)) {
		return NULL;
	}

	if (!GEngine)
		goto end;

	PyObject *stringified = PyObject_Str(py_message);
	if (!stringified)
		return PyErr_Format(PyExc_Exception, "argument cannot be casted to string");
	char *message = PyUnicode_AsUTF8(stringified);

	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Cyan, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

	Py_DECREF(stringified);

end:
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_get_forward_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_forward_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetForwardVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}

PyObject *py_unreal_engine_get_right_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_right_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetRightVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}

PyObject *py_unreal_engine_get_up_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_up_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetUpVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
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

PyObject *py_unreal_engine_load_class(PyObject * self, PyObject * args) {
	char *name;
	char *filename = nullptr;
	if (!PyArg_ParseTuple(args, "s|s:load_class", &name, &filename)) {
		return NULL;
	}

	TCHAR *t_filename = nullptr;
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

PyObject *py_unreal_engine_find_struct(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_struct", &name)) {
		return NULL;
	}

	UStruct *u_struct = FindObject<UStruct>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (!u_struct)
		return PyErr_Format(PyExc_Exception, "unable to find class %s", name);

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

	TCHAR *t_filename = nullptr;
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

	UClass *u_class = (UClass *)py_obj;

	TCHAR *t_filename = nullptr;
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


PyObject *py_unreal_engine_color_to_linear(PyObject * self, PyObject * args) {
	uint8 r, g, b;
	uint8 a = 255;
	if (!PyArg_ParseTuple(args, "iii|i:color_to_linear", &r, &g, &b, &a)) {
		return NULL;
	}

	FLinearColor lcolor = FColor(r, g, b, a).ReinterpretAsLinear();
	return Py_BuildValue("fff", lcolor.R, lcolor.G, lcolor.B);
}

PyObject *py_unreal_engine_color_from_linear(PyObject * self, PyObject * args) {
	float r, g, b;
	float a = 1;
	if (!PyArg_ParseTuple(args, "fff|f:color_from_linear", &r, &g, &b, &a)) {
		return NULL;
	}

	FColor color = FLinearColor(r, g, b, a).ToFColor(true);
	return Py_BuildValue("iii", color.R, color.G, color.B);
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

PyObject *py_unreal_engine_vector_add_vector(PyObject * self, PyObject *args) {

	float x = 0, y = 0, z = 0;

	Py_ssize_t items = PyTuple_Size(args);

	if ((items % 3) != 0) {
		return PyErr_Format(PyExc_TypeError, "this function requires a 3-multiple number of args");
	}

	for (int i = 0; i < (int)items; i += 3) {
		PyObject *x_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(x_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
		x += (float)PyFloat_AsDouble(x_py);
		PyObject *y_py = PyTuple_GetItem(args, i + 1);
		if (!PyFloat_Check(y_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (y)");
		y += (float)PyFloat_AsDouble(y_py);
		PyObject *z_py = PyTuple_GetItem(args, i + 2);
		if (!PyFloat_Check(z_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (z)");
		z += (float)PyFloat_AsDouble(z_py);
	}

	return Py_BuildValue("fff", x, y, z);
}

PyObject *py_unreal_engine_vector_add_float(PyObject * self, PyObject *args) {

	float x = 0, y = 0, z = 0;

	Py_ssize_t items = PyTuple_Size(args);

	if (items < 3) {
		return PyErr_Format(PyExc_TypeError, "this function requires a xyz vector");
	}

	PyObject *x_py = PyTuple_GetItem(args, 0);
	if (!PyFloat_Check(x_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	x = (float)PyFloat_AsDouble(x_py);


	PyObject *y_py = PyTuple_GetItem(args, 1);
	if (!PyFloat_Check(y_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	y = (float)PyFloat_AsDouble(y_py);

	PyObject *z_py = PyTuple_GetItem(args, 2);
	if (!PyFloat_Check(z_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	z = (float)PyFloat_AsDouble(z_py);

	for (int i = 3; i < (int)items; i++) {
		PyObject *delta_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(delta_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values");
		float delta = (float)PyFloat_AsDouble(delta_py);
		x += delta;
		y += delta;
		z += delta;
	}

	return Py_BuildValue("fff", x, y, z);
}

PyObject *py_unreal_engine_vector_mul_vector(PyObject * self, PyObject *args) {

	float x = 1, y = 1, z = 1;

	Py_ssize_t items = PyTuple_Size(args);

	if ((items % 3) != 0) {
		return PyErr_Format(PyExc_TypeError, "this function requires a 3-multiple number of args");
	}

	for (int i = 0; i < (int)items; i += 3) {
		PyObject *x_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(x_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
		x *= (float)PyFloat_AsDouble(x_py);
		PyObject *y_py = PyTuple_GetItem(args, i + 1);
		if (!PyFloat_Check(y_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (y)");
		y *= (float)PyFloat_AsDouble(y_py);
		PyObject *z_py = PyTuple_GetItem(args, i + 2);
		if (!PyFloat_Check(z_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values (z)");
		z *= (float)PyFloat_AsDouble(z_py);
	}

	return Py_BuildValue("fff", x, y, z);
}

PyObject *py_unreal_engine_vector_mul_float(PyObject * self, PyObject *args) {

	float x = 0, y = 0, z = 0;

	Py_ssize_t items = PyTuple_Size(args);

	if (items < 3) {
		return PyErr_Format(PyExc_TypeError, "this function requires a xyz vector");
	}

	PyObject *x_py = PyTuple_GetItem(args, 0);
	if (!PyFloat_Check(x_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	x = (float)PyFloat_AsDouble(x_py);


	PyObject *y_py = PyTuple_GetItem(args, 1);
	if (!PyFloat_Check(y_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	y = (float)PyFloat_AsDouble(y_py);

	PyObject *z_py = PyTuple_GetItem(args, 2);
	if (!PyFloat_Check(z_py))
		return PyErr_Format(PyExc_TypeError, "this function supports only float values (x)");
	z = (float)PyFloat_AsDouble(z_py);

	for (int i = 3; i < (int)items; i++) {
		PyObject *delta_py = PyTuple_GetItem(args, i);
		if (!PyFloat_Check(delta_py))
			return PyErr_Format(PyExc_TypeError, "this function supports only float values");
		float delta = (float)PyFloat_AsDouble(delta_py);
		x *= delta;
		y *= delta;
		z *= delta;
	}

	return Py_BuildValue("fff", x, y, z);
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

	ue_PyUObject *ret = ue_get_python_wrapper(new_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_new_blueprint_class(PyObject * self, PyObject * args) {

	PyObject *py_parent;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:new_blueprint_class", &py_parent, &name)) {
		return NULL;
	}

	UObject *outer = GetTransientPackage();
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

	UBlueprintGeneratedClass *new_object = nullptr;

	if (Cast<UBlueprintGeneratedClass>(parent)) {
		UE_LOG(LogPython, Warning, TEXT("Parent class is blueprint generated"));
		new_object = NewObject<UBlueprintGeneratedClass>(outer, UTF8_TO_TCHAR(name), RF_Public);
	}
	else {
		UE_LOG(LogPython, Warning, TEXT("Parent class is NOT blueprint generated"));
		new_object = NewObject<UPythonGeneratedClass>(outer, UTF8_TO_TCHAR(name), RF_Public);
		new_object->ClassFlags |= CLASS_Native;
	}

	if (!new_object)
		return PyErr_Format(PyExc_Exception, "unable to create blueprint class");

	UBlueprint *blueprint = NewObject<UBlueprint>(outer, MakeUniqueObjectName(outer, UBlueprint::StaticClass(), UTF8_TO_TCHAR(name)), RF_Public);
	blueprint->GeneratedClass = new_object;
	new_object->ClassGeneratedBy = blueprint;
	

	auto ClassConstructor = [](const FObjectInitializer& ObjectInitializer) {
		auto _class = ObjectInitializer.GetClass();
		UE_LOG(LogPython, Warning, TEXT("CALLING CONSTRUCTOR OF %s"), *_class->GetName());
		_class->GetSuperClass()->ClassConstructor(ObjectInitializer);
	};

	new_object->ClassConstructor = ClassConstructor;
	new_object->SetSuperStruct(parent);

	new_object->PropertyLink = parent->PropertyLink;
	new_object->ClassWithin = parent->ClassWithin;
	new_object->ClassConfigName = parent->ClassConfigName;

	new_object->ClassFlags |= (parent->ClassFlags & (CLASS_Inherit | CLASS_ScriptInherit | CLASS_CompiledFromBlueprint));
	new_object->ClassCastFlags |= parent->ClassCastFlags;

	new_object->Bind();
	new_object->StaticLink(true);

	new_object->GetDefaultObject();

	ue_PyUObject *ret = ue_get_python_wrapper(new_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}
