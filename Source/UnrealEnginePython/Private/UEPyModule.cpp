#include "UnrealEnginePythonPrivatePCH.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "UEPyTransform.h"
#include "UEPyInput.h"
#include "UEPyNavigation.h"
#include "UEPySpline.h"
#include "UEPyMovements.h"
#include "UEPyAttaching.h"
#include "UEPySkeletal.h"



DEFINE_LOG_CATEGORY(LogPython);

PyDoc_STRVAR(unreal_engine_py_doc, "Unreal Engine Python module.");

static PyModuleDef unreal_engine_module = {
	PyModuleDef_HEAD_INIT,
	"unreal_engine",
	unreal_engine_py_doc,
	-1,
	NULL,
};



static PyObject *init_unreal_engine(void) {
	return PyModule_Create(&unreal_engine_module);
}

static PyObject *py_unreal_engine_log(PyObject * self, PyObject * args) {
	char *message;
	if (!PyArg_ParseTuple(args, "s:log", &message)) {
		return NULL;
	}
	UE_LOG(LogPython, Log, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_unreal_engine_log_warning(PyObject * self, PyObject * args) {
	char *message;
	if (!PyArg_ParseTuple(args, "s:log_warning", &message)) {
		return NULL;
	}
	UE_LOG(LogPython, Warning, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_unreal_engine_log_error(PyObject * self, PyObject * args) {
	char *message;
	if (!PyArg_ParseTuple(args, "s:log_error", &message)) {
		return NULL;
	}
	UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(message));
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_unreal_engine_add_on_screen_debug_message(PyObject * self, PyObject * args) {
	int key;
	float time_to_display;
	char *message;
	if (!PyArg_ParseTuple(args, "ifs:add_on_screen_debug_message", &key, &time_to_display, &message)) {
		return NULL;
	}

	if (!GEngine)
		goto end;

	GEngine->AddOnScreenDebugMessage(key, time_to_display, FColor::Green, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

end:
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_unreal_engine_print_string(PyObject * self, PyObject * args) {

	char *message;
	if (!PyArg_ParseTuple(args, "s:print_string", &message)) {
		return NULL;
	}

	if (!GEngine)
		goto end;

	GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Cyan, FString::Printf(TEXT("%s"), UTF8_TO_TCHAR(message)));

end:
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_unreal_engine_get_forward_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_forward_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetForwardVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}

static PyObject *py_unreal_engine_get_right_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_right_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetRightVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}

static PyObject *py_unreal_engine_get_up_vector(PyObject * self, PyObject * args) {
	float roll, pitch, yaw;
	if (!PyArg_ParseTuple(args, "fff:get_up_vector", &roll, &pitch, &yaw)) {
		return NULL;
	}
	FVector vec = UKismetMathLibrary::GetUpVector(FRotator(pitch, yaw, roll));
	return Py_BuildValue("fff", vec.X, vec.Y, vec.Z);
}


static PyObject *py_unreal_engine_find_class(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_class", &name)) {
		return NULL;
	}

	UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (u_class) {
		ue_PyUObject *ret = ue_get_python_wrapper(u_class);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_unreal_engine_find_object(PyObject * self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:find_object", &name)) {
		return NULL;
	}

	UObject *u_object = FindObject<UObject>(ANY_PACKAGE, UTF8_TO_TCHAR(name));

	if (u_object) {
		ue_PyUObject *ret = ue_get_python_wrapper(u_object);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

#if WITH_EDITOR
static PyObject *py_unreal_engine_get_editor_world(PyObject * self, PyObject * args) {

	if (!GEditor)
		return PyErr_Format(PyExc_Exception, "no GEditor found");

	UWorld *world = GEditor->GetEditorWorldContext().World();
	ue_PyUObject *ret = ue_get_python_wrapper(world);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}
#endif


static PyObject *py_unreal_engine_vector_add_vector(PyObject * self, PyObject *args) {

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

static PyObject *py_unreal_engine_vector_add_float(PyObject * self, PyObject *args) {

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

static PyObject *py_unreal_engine_vector_mul_vector(PyObject * self, PyObject *args) {

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

static PyObject *py_unreal_engine_vector_mul_float(PyObject * self, PyObject *args) {

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

static PyMethodDef unreal_engine_methods[] = {
	{ "log", py_unreal_engine_log, METH_VARARGS, "" },
	{ "log_warning", py_unreal_engine_log_warning, METH_VARARGS, "" },
	{ "log_error", py_unreal_engine_log_error, METH_VARARGS, "" },
	{ "add_on_screen_debug_message", py_unreal_engine_add_on_screen_debug_message, METH_VARARGS, "" },
	{ "print_string", py_unreal_engine_print_string, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "get_editor_world()", py_unreal_engine_get_editor_world, METH_VARARGS, "" },
#endif
	{ "find_class", py_unreal_engine_find_class, METH_VARARGS, "" },
	{ "find_object", py_unreal_engine_find_object, METH_VARARGS, "" },
	{ "vector_add_vector", py_unreal_engine_vector_add_vector, METH_VARARGS, "" },
	{ "vector_add_float", py_unreal_engine_vector_add_float, METH_VARARGS, "" },
	{ "vector_mul_vector", py_unreal_engine_vector_mul_vector, METH_VARARGS, "" },
	{ "vector_mul_float", py_unreal_engine_vector_mul_float, METH_VARARGS, "" },

	{ "get_forward_vector", py_unreal_engine_get_forward_vector, METH_VARARGS, "" },
	{ "get_up_vector", py_unreal_engine_get_up_vector, METH_VARARGS, "" },
	{ "get_right_vector", py_unreal_engine_get_right_vector, METH_VARARGS, "" },

	{ NULL, NULL },
};


static PyObject *py_ue_call(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *call_args;
	if (!PyArg_ParseTuple(args, "s:call", &call_args)) {
		return NULL;
	}

	FOutputDeviceNull od_null;
	if (!self->ue_object->CallFunctionByNameWithArguments(UTF8_TO_TCHAR(call_args), od_null, NULL, true)) {
		return PyErr_Format(PyExc_Exception, "error while calling \"%s\"", call_args);
	}

	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *py_ue_get_property(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *property_name;
	if (!PyArg_ParseTuple(args, "s:get_property", &property_name)) {
		return NULL;
	}

	UProperty *u_property = self->ue_object->GetClass()->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	return ue_py_convert_property(u_property, (uint8 *)self->ue_object);
}

static PyObject *py_ue_set_property(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *property_name;
	PyObject *property_value;
	if (!PyArg_ParseTuple(args, "sO:set_property", &property_name, &property_value)) {
		return NULL;
	}

	UProperty *u_property = self->ue_object->GetClass()->FindPropertyByName(FName(UTF8_TO_TCHAR(property_name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", property_name);

	
	if (!ue_py_convert_pyobject(property_value, u_property, (uint8 *)self->ue_object)) {
		return PyErr_Format(PyExc_Exception, "unable to set property %s", property_name);
	}

	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *py_ue_properties(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *ret = PyList_New(0);

	for (TFieldIterator<UProperty> PropIt(self->ue_object->GetClass()); PropIt; ++PropIt)
	{
		UProperty* property = *PropIt;
		PyObject *property_name = PyUnicode_FromString(TCHAR_TO_UTF8(*property->GetName()));
		PyList_Append(ret, property_name);
		Py_DECREF(property_name);
	}

	return ret;

}

static PyObject *py_ue_get_owner(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<UActorComponent>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not a component");
	}

	UActorComponent *component = (UActorComponent *)self->ue_object;

	ue_PyUObject *ret = ue_get_python_wrapper(component->GetOwner());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

static PyObject *py_ue_get_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);


	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetName())));
}

static PyObject *py_ue_get_full_name(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);


return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->ue_object->GetFullName())));
}

#if WITH_EDITOR
static PyObject *py_ue_get_actor_label(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (self->ue_object->IsA<AActor>()) {
		AActor *actor = (AActor *)self->ue_object;
		return PyUnicode_FromString(TCHAR_TO_UTF8(*(actor->GetActorLabel())));
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)self->ue_object;
		return PyUnicode_FromString(TCHAR_TO_UTF8(*(component->GetOwner()->GetActorLabel())));
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
}
#endif


static PyObject *py_ue_get_actor_velocity(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetVelocity();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetVelocity();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}



static PyObject *py_ue_quit_game(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	// no need to support multiple controllers
	APlayerController *controller = world->GetFirstPlayerController();
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve the first controller");

	UKismetSystemLibrary::QuitGame(world, controller, EQuitPreference::Quit);

	Py_INCREF(Py_None);
	return Py_None;
}

// mainly used for testing
static PyObject *py_ue_world_tick(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float delta_time;
	if (!PyArg_ParseTuple(args, "f:world_tick", &delta_time)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	world->Tick(LEVELTICK_All, delta_time);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_create_player(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	int controller_id;
	PyObject *spawn_pawn;
	if (!PyArg_ParseTuple(args, "i|O:create_player", &controller_id, &spawn_pawn)) {
		return NULL;
	}

	bool b_spawn_pawn = true;

	if (spawn_pawn && !PyObject_IsTrue(spawn_pawn)) {
		b_spawn_pawn = false;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::CreatePlayer(world, controller_id, b_spawn_pawn);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to create a new player fro controller %d", controller_id);

	return PyLong_FromLong(controller->PlayerState->PlayerId);
}

static PyObject *py_ue_get_num_players(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	AGameMode *game_mode = world->GetAuthGameMode();
	if (!game_mode)
		return PyErr_Format(PyExc_Exception, "unable to retrieve GameMode from world");

	return PyLong_FromLong(game_mode->NumPlayers);
}

static PyObject *py_ue_get_num_spectators(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	AGameMode *game_mode = world->GetAuthGameMode();
	if (!game_mode)
		return PyErr_Format(PyExc_Exception, "unable to retrieve GameMode from world");

	return PyLong_FromLong(game_mode->NumSpectators);
}



static PyObject *py_ue_get_world(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	ue_PyUObject *ret = ue_get_python_wrapper(world);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

}






static PyObject *py_ue_find_function(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_function", &name)) {
		return NULL;
	}

	UFunction *function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(name)));
	if (!function)
		goto end;

	UE_LOG(LogPython, Warning, TEXT("Func %d %d"), function->NumParms, function->ReturnValueOffset);

	ue_PyUObject *ret = ue_get_python_wrapper((UObject *)function);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;

end:
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_call_function(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UFunction *function = nullptr;

	if (PyTuple_Size(args) < 1) {
		return PyErr_Format(PyExc_TypeError, "this function requires at least an argument");
	}

	PyObject *func_id = PyTuple_GetItem(args, 0);

	if (PyUnicode_Check(func_id)) {
		function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(PyUnicode_AsUTF8(func_id))));
	}


	if (!function)
		return PyErr_Format(PyExc_Exception, "unable to find function");

	uint8 *buffer = (uint8 *)FMemory_Alloca(function->ParmsSize);
	int argn = 1;

	TFieldIterator<UProperty> PArgs(function);
	for (; PArgs && ((PArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm); ++PArgs) {
		UProperty *prop = *PArgs;
		PyObject *py_arg = PyTuple_GetItem(args, argn);
		if (!py_arg) {
			return PyErr_Format(PyExc_TypeError, "not enough arguments");
		}
		if (!ue_py_convert_pyobject(py_arg, prop, buffer)) {
			return PyErr_Format(PyExc_TypeError, "unable to convert pyobject to property");
		}
		argn++;
	}

	self->ue_object->ProcessEvent(function, buffer);

	TFieldIterator<UProperty> Props(function);
	for (; Props; ++Props) {
		UProperty *prop = *Props;
		if (prop->GetPropertyFlags() & CPF_ReturnParm) {
			return ue_py_convert_property(prop, buffer);
		}
	}

	Py_INCREF(Py_None);
	return Py_None;
}

#if WITH_EDITOR
static PyObject *py_ue_find_actor_by_label(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_actor_by_label", &name)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	UObject *u_object = nullptr;

	for (TActorIterator<AActor> Itr(world); Itr; ++Itr) {
		AActor *u_obj = *Itr;
		if (u_obj->GetActorLabel().Equals(UTF8_TO_TCHAR(name))) {
			u_object = u_obj;
			break;
		}
	}

	if (u_object) {
		ue_PyUObject *ret = ue_get_python_wrapper(u_object);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}

	Py_INCREF(Py_None);
	return Py_None;
}
#endif

static PyObject *py_ue_get_class(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	ue_PyUObject *ret = ue_get_python_wrapper(self->ue_object->GetClass());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

static PyObject *py_ue_all_objects(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *ret = PyList_New(0);

	for (TObjectIterator<UObject> Itr; Itr; ++Itr) {
		UObject *u_obj = *Itr;
		if (u_obj->GetWorld() != world)
			continue;
		ue_PyUObject *py_obj = ue_get_python_wrapper(u_obj);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);
	}
	return ret;
}

static PyObject *py_ue_all_actors(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *ret = PyList_New(0);

	for (TActorIterator<AActor> Itr(world); Itr; ++Itr) {
		UObject *u_obj = *Itr;
		ue_PyUObject *py_obj = ue_get_python_wrapper(u_obj);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);

	}


	return ret;
}


static PyObject *py_ue_set_simulate_physics(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:set_simulate_physics", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>()) {
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	if (!primitive) {
		return PyErr_Format(PyExc_Exception, "unable to set physics for the object");
	}

	primitive->SetSimulatePhysics(enabled);

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *py_ue_actor_components(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	PyObject *ret = PyList_New(0);

	for (UActorComponent *component : actor->GetComponents()) {
		ue_PyUObject *py_obj = ue_get_python_wrapper(component);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);
	}

	return ret;
}

static PyObject *py_ue_get_actor_component(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve Actor from uobject");

	char *name;
	if (!PyArg_ParseTuple(args, "s:get_actor_component", &name)) {
		return NULL;
	}

	for (UActorComponent *component : actor->GetComponents()) {
		if (component->GetName().Equals(UTF8_TO_TCHAR(name))) {
			ue_PyUObject *py_obj = ue_get_python_wrapper(component);
			if (!py_obj)
				return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
			Py_INCREF(py_obj);
			return (PyObject *) py_obj;
		}
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_actor_destroy(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	actor->Destroy();

	Py_INCREF(Py_None);
	return Py_None;


}

static PyObject *py_ue_actor_has_tag(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *tag;
	if (!PyArg_ParseTuple(args, "s:actor_has_tag", &tag)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (actor->ActorHasTag(FName(UTF8_TO_TCHAR(tag)))) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;


}

static PyObject *py_ue_get_actor_bounds(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);


	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	FVector origin;
	FVector extent;

	actor->GetActorBounds(false, origin, extent);

	return Py_BuildValue("ffffff", origin.X, origin.Y, origin.Z, extent.X, extent.Y, extent.Z);

}

static PyObject *py_ue_line_trace_single_by_channel(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float x1 = 0, y1 = 0, z1 = 0;
	float x2 = 0, y2 = 0, z2 = 0;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "ffffffi:line_trace_single_by_channel", &x1, &y1, &z1, &x2, &y2, &z2, &channel)) {
		return NULL;
	}

	FHitResult hit;

	bool got_hit = world->LineTraceSingleByChannel(hit, FVector(x1, y1, z1), FVector(x2, y2, z2), (ECollisionChannel)channel);

	if (got_hit) {
		PyObject *ret = (PyObject *)ue_get_python_wrapper(hit.GetActor());
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		return Py_BuildValue("Offffff", ret, hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z, hit.ImpactNormal.X, hit.ImpactNormal.Y, hit.ImpactNormal.Z);
	}

	return Py_BuildValue("Offffff", Py_None, 0, 0, 0, 0, 0, 0);

}

static PyObject *py_ue_line_trace_multi_by_channel(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float x1 = 0, y1 = 0, z1 = 0;
	float x2 = 0, y2 = 0, z2 = 0;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "ffffffi:line_trace_multi_by_channel", &x1, &y1, &z1, &x2, &y2, &z2, &channel)) {
		return NULL;
	}

	TArray<struct FHitResult> hits;
	hits.Reset();

	PyObject *hits_list = PyList_New(0);


	bool got_hits = world->LineTraceMultiByChannel(hits, FVector(x1, y1, z1), FVector(x2, y2, z2), (ECollisionChannel)channel);

	if (got_hits) {
		for (int i = 0; i < hits.Num(); i++) {
			FHitResult hit = hits[i];
			PyObject *ret = (PyObject *)ue_get_python_wrapper(hit.GetActor());
			if (ret) {
				PyList_Append(hits_list, Py_BuildValue("Offffff", ret, hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z, hit.ImpactNormal.X, hit.ImpactNormal.Y, hit.ImpactNormal.Z));
			}
		}
	}

	return hits_list;

}

static PyObject *py_ue_get_hit_result_under_cursor(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	int channel;
	PyObject *trace_complex = NULL;
	int controller_id = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "i|Oi:get_hit_result_under_cursor", &channel, &trace_complex, &controller_id)) {
		return NULL;
	}


	bool complex = false;
	if (trace_complex && PyObject_IsTrue(trace_complex)) {
		complex = true;
	}

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	FHitResult hit;

	bool got_hit = controller->GetHitResultUnderCursor((ECollisionChannel)channel, complex, hit);

	if (got_hit) {
		PyObject *ret = (PyObject *)ue_get_python_wrapper(hit.GetActor());
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		return Py_BuildValue("Offffff", ret, hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z, hit.ImpactNormal.X, hit.ImpactNormal.Y, hit.ImpactNormal.Z);
	}

	return Py_BuildValue("Offffff", Py_None, 0, 0, 0, 0, 0, 0);

}


static PyObject *py_ue_is_a(ue_PyUObject *, PyObject *);
static PyObject *py_ue_actor_has_component_of_type(ue_PyUObject *, PyObject *);
static PyObject *py_ue_actor_spawn(ue_PyUObject *, PyObject *);
static PyObject *py_ue_destructible_apply_damage(ue_PyUObject *, PyObject *);
static PyObject *py_ue_set_view_target(ue_PyUObject *, PyObject *);
static PyObject *py_ue_add_actor_component(ue_PyUObject *, PyObject *);
static PyObject *py_ue_get_actor_component_by_type(ue_PyUObject *, PyObject *);
static PyObject *py_ue_get_actor_components_by_type(ue_PyUObject *, PyObject *);
static PyObject *py_ue_add_actor_root_component(ue_PyUObject *, PyObject *);
static PyObject *py_ue_play_sound_at_location(ue_PyUObject *, PyObject *);
static PyObject *py_ue_get_overlapping_actors(ue_PyUObject *, PyObject *);

static PyMethodDef ue_PyUObject_methods[] = {

	// Transform

	{ "get_actor_location", (PyCFunction)py_ue_get_actor_location, METH_VARARGS, "" },
	{ "get_actor_rotation", (PyCFunction)py_ue_get_actor_rotation, METH_VARARGS, "" },
	{ "get_actor_scale", (PyCFunction)py_ue_get_actor_scale, METH_VARARGS, "" },

	{ "get_actor_forward", (PyCFunction)py_ue_get_actor_forward, METH_VARARGS, "" },
	{ "get_actor_right", (PyCFunction)py_ue_get_actor_right, METH_VARARGS, "" },
	{ "get_actor_up", (PyCFunction)py_ue_get_actor_up, METH_VARARGS, "" },

	
	{ "set_actor_location", (PyCFunction)py_ue_set_actor_location, METH_VARARGS, "" },
	{ "set_actor_rotation", (PyCFunction)py_ue_set_actor_rotation, METH_VARARGS, "" },
	{ "set_actor_scale", (PyCFunction)py_ue_set_actor_scale, METH_VARARGS, "" },
	

	{ "get_world_location", (PyCFunction)py_ue_get_world_location, METH_VARARGS, "" },
	{ "get_world_rotation", (PyCFunction)py_ue_get_world_rotation, METH_VARARGS, "" },
	{ "get_world_scale", (PyCFunction)py_ue_get_world_scale, METH_VARARGS, "" },
	{ "get_relative_location", (PyCFunction)py_ue_get_relative_location, METH_VARARGS, "" },
	{ "get_relative_rotation", (PyCFunction)py_ue_get_relative_rotation, METH_VARARGS, "" },
	{ "get_relative_scale", (PyCFunction)py_ue_get_relative_scale, METH_VARARGS, "" },
	
	{ "set_world_location", (PyCFunction)py_ue_set_world_location, METH_VARARGS, "" },
	{ "set_world_rotation", (PyCFunction)py_ue_set_world_rotation, METH_VARARGS, "" },
	{ "set_world_scale", (PyCFunction)py_ue_set_world_scale, METH_VARARGS, "" },
	{ "set_relative_location", (PyCFunction)py_ue_set_relative_location, METH_VARARGS, "" },
	{ "set_relative_rotation", (PyCFunction)py_ue_set_relative_rotation, METH_VARARGS, "" },
	{ "set_relative_scale", (PyCFunction)py_ue_set_relative_scale, METH_VARARGS, "" },

	{ "get_forward_vector", (PyCFunction)py_ue_get_forward_vector, METH_VARARGS, "" },
	{ "get_up_vector", (PyCFunction)py_ue_get_up_vector, METH_VARARGS, "" },
	{ "get_right_vector", (PyCFunction)py_ue_get_right_vector, METH_VARARGS, "" },

	// UObject

	{ "get_property", (PyCFunction)py_ue_get_property, METH_VARARGS, "" },
	{ "set_property", (PyCFunction)py_ue_set_property, METH_VARARGS, "" },
	{ "properties", (PyCFunction)py_ue_properties, METH_VARARGS, "" },
	{ "get_properties", (PyCFunction)py_ue_properties, METH_VARARGS, "" },

	{ "is_a", (PyCFunction)py_ue_is_a, METH_VARARGS, "" },

	{ "call", (PyCFunction)py_ue_call, METH_VARARGS, "" },
	{ "get_owner", (PyCFunction)py_ue_get_owner, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_get_name, METH_VARARGS, "" },
	{ "get_full_name", (PyCFunction)py_ue_get_full_name, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "get_actor_label", (PyCFunction)py_ue_get_actor_label, METH_VARARGS, "" },
	{ "find_actor_by_label", (PyCFunction)py_ue_find_actor_by_label, METH_VARARGS, "" },
#endif
	

	{ "find_function", (PyCFunction)py_ue_find_function, METH_VARARGS, "" },
	{ "call_function", (PyCFunction)py_ue_call_function, METH_VARARGS, "" },


	{ "all_objects", (PyCFunction)py_ue_all_objects, METH_VARARGS, "" },
	{ "all_actors", (PyCFunction)py_ue_all_actors, METH_VARARGS, "" },

	// Input

	{ "get_input_axis", (PyCFunction)py_ue_get_input_axis, METH_VARARGS, "" },
	{ "bind_input_axis", (PyCFunction)py_ue_bind_input_axis, METH_VARARGS, "" },
	{ "enable_input", (PyCFunction)py_ue_enable_input, METH_VARARGS, "" },
	{ "show_mouse_cursor", (PyCFunction)py_ue_show_mouse_cursor, METH_VARARGS, "" },
	{ "enable_click_events", (PyCFunction)py_ue_enable_click_events, METH_VARARGS, "" },
	{ "enable_mouse_over_events", (PyCFunction)py_ue_enable_mouse_over_events, METH_VARARGS, "" },
	{ "was_input_key_just_pressed", (PyCFunction)py_ue_was_input_key_just_pressed, METH_VARARGS, "" },
	{ "was_input_key_just_released", (PyCFunction)py_ue_was_input_key_just_released, METH_VARARGS, "" },
	{ "is_action_pressed", (PyCFunction)py_ue_is_action_pressed, METH_VARARGS, "" },
	{ "is_action_released", (PyCFunction)py_ue_is_action_released, METH_VARARGS, "" },
	{ "is_input_key_down", (PyCFunction)py_ue_is_input_key_down, METH_VARARGS, "" },
	

	// Movements

	{ "add_controller_pitch_input", (PyCFunction)py_ue_add_controller_pitch_input, METH_VARARGS, "" },
	{ "add_controller_yaw_input", (PyCFunction)py_ue_add_controller_yaw_input, METH_VARARGS, "" },
	{ "get_control_rotation", (PyCFunction)py_ue_get_control_rotation, METH_VARARGS, "" },
	{ "add_movement_input", (PyCFunction)py_ue_add_movement_input, METH_VARARGS, "" },

	{ "jump", (PyCFunction)py_ue_jump, METH_VARARGS, "" },
	{ "stop_jumping", (PyCFunction)py_ue_stop_jumping, METH_VARARGS, "" },
	{ "crouch", (PyCFunction)py_ue_crouch, METH_VARARGS, "" },
	{ "uncrouch", (PyCFunction)py_ue_uncrouch, METH_VARARGS, "" },
	{ "launch", (PyCFunction)py_ue_launch, METH_VARARGS, "" },
	{ "is_jumping", (PyCFunction)py_ue_is_jumping, METH_VARARGS, "" },
	{ "is_crouched", (PyCFunction)py_ue_is_crouched, METH_VARARGS, "" },
	{ "is_falling", (PyCFunction)py_ue_is_falling, METH_VARARGS, "" },
	{ "is_flying", (PyCFunction)py_ue_is_falling, METH_VARARGS, "" },
	{ "can_jump", (PyCFunction)py_ue_can_jump, METH_VARARGS, "" },
	{ "can_crouch", (PyCFunction)py_ue_can_crouch, METH_VARARGS, "" },


	{ "get_class", (PyCFunction)py_ue_get_class, METH_VARARGS, "" },
	{ "get_actor_components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },
	{ "components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },
	{ "get_components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },

	{ "quit_game", (PyCFunction)py_ue_quit_game, METH_VARARGS, "" },
	{ "simple_move_to_location", (PyCFunction)py_ue_simple_move_to_location, METH_VARARGS, "" },
	
	{ "actor_has_component_of_type", (PyCFunction)py_ue_actor_has_component_of_type, METH_VARARGS, "" },

	{ "actor_destroy", (PyCFunction)py_ue_actor_destroy, METH_VARARGS, "" },
	{ "actor_spawn", (PyCFunction)py_ue_actor_spawn, METH_VARARGS, "" },
	{ "actor_has_tag", (PyCFunction)py_ue_actor_has_tag, METH_VARARGS, "" },
	{ "get_actor_bounds", (PyCFunction)py_ue_get_actor_bounds, METH_VARARGS, "" },

	{ "line_trace_single_by_channel", (PyCFunction)py_ue_line_trace_single_by_channel, METH_VARARGS, "" },
	{ "line_trace_multi_by_channel", (PyCFunction)py_ue_line_trace_multi_by_channel, METH_VARARGS, "" },
	{ "get_hit_result_under_cursor", (PyCFunction)py_ue_get_hit_result_under_cursor, METH_VARARGS, "" },
	
	{ "destructible_apply_damage", (PyCFunction)py_ue_destructible_apply_damage, METH_VARARGS, "" },

	{ "set_view_target", (PyCFunction)py_ue_set_view_target, METH_VARARGS, "" },

	{ "add_actor_component", (PyCFunction)py_ue_add_actor_component, METH_VARARGS, "" },
	{ "add_actor_root_component", (PyCFunction)py_ue_add_actor_root_component, METH_VARARGS, "" },
	{ "get_actor_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },
	{ "get_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },

	{ "set_simulate_physics", (PyCFunction)py_ue_set_simulate_physics, METH_VARARGS, "" },
	{ "get_world", (PyCFunction)py_ue_get_world, METH_VARARGS, "" },

	{ "get_world_location_at_distance_along_spline", (PyCFunction)py_ue_get_world_location_at_distance_along_spline, METH_VARARGS, "" },
	{ "get_spline_length", (PyCFunction)py_ue_get_spline_length, METH_VARARGS, "" },

	{ "get_actor_velocity", (PyCFunction)py_ue_get_actor_velocity, METH_VARARGS, "" },

	{ "play_sound_at_location", (PyCFunction)py_ue_play_sound_at_location, METH_VARARGS, "" },

	{ "world_tick", (PyCFunction)py_ue_world_tick, METH_VARARGS, "" },

	{ "create_player", (PyCFunction)py_ue_create_player, METH_VARARGS, "" },
	{ "get_num_players", (PyCFunction)py_ue_get_num_players, METH_VARARGS, "" },
	{ "get_num_spectators", (PyCFunction)py_ue_get_num_spectators, METH_VARARGS, "" },

	{ "get_overlapping_actors", (PyCFunction)py_ue_get_overlapping_actors, METH_VARARGS, "" },


	// Attaching

	{ "get_socket_location", (PyCFunction)py_ue_get_socket_location, METH_VARARGS, "" },
	{ "get_socket_rotation", (PyCFunction)py_ue_get_socket_rotation, METH_VARARGS, "" },
	{ "get_socket_transform", (PyCFunction)py_ue_get_socket_transform, METH_VARARGS, "" },
	{ "get_socket_world_transform", (PyCFunction)py_ue_get_socket_world_transform, METH_VARARGS, "" },
	{ "get_socket_actor_transform", (PyCFunction)py_ue_get_socket_actor_transform, METH_VARARGS, "" },
	{ "get_attached_actors", (PyCFunction)py_ue_get_attached_actors, METH_VARARGS, "" },
	{ "get_all_child_actors", (PyCFunction)py_ue_get_all_child_actors, METH_VARARGS, "" },
	{ "attach_to_actor", (PyCFunction)py_ue_attach_to_actor, METH_VARARGS, "" },
	{ "attach_to_component", (PyCFunction)py_ue_attach_to_component, METH_VARARGS, "" },

	// Skeletal
	{ "get_anim_instance", (PyCFunction)py_ue_get_anim_instance, METH_VARARGS, "" },


	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyUObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UObject",             /* tp_name */
	sizeof(ue_PyUObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,                         /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine generic UObject",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyUObject_methods,             /* tp_methods */
};

static PyObject *py_ue_add_actor_component(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:add_actor_component", &obj, &name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a class");
	}

	UActorComponent *component = NewObject<UActorComponent>(actor, (UClass *)py_obj->ue_object, FName(UTF8_TO_TCHAR(name)));
	if (!component)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	component->RegisterComponent();

	PyObject *ret = (PyObject *)ue_get_python_wrapper(component);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

static PyObject *py_ue_play_sound_at_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *sound;
	float x, y, z;
	float volume = 1;
	float pitch = 1;
	float start = 0;

	if (!PyArg_ParseTuple(args, "Offf|fff:play_sound_at_location", &sound, &x, &y, &z, &volume, &pitch, &start)) {
		return NULL;
	}


	USoundBase *sound_object = nullptr;
	if (PyObject_IsInstance(sound, (PyObject *)&ue_PyUObjectType)) {
		ue_PyUObject *py_sound = (ue_PyUObject *)sound;
		if (py_sound->ue_object->IsA<USoundBase>()) {
			sound_object = (USoundBase *)py_sound->ue_object;
		}
	}
	else if (PyUnicode_Check(sound)) {
		sound_object = FindObject<USoundBase>(ANY_PACKAGE, UTF8_TO_TCHAR(PyUnicode_AsUTF8(sound)));
	}

	if (!sound_object)
		return PyErr_Format(PyExc_Exception, "invalid sound object");

	UGameplayStatics::PlaySoundAtLocation(self->ue_object, sound_object, FVector(x, y, z), volume, pitch, start);

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *py_ue_add_actor_root_component(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	char *name;
	if (!PyArg_ParseTuple(args, "Os:add_actor_root_component", &obj, &name)) {
		return NULL;
	}

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<UClass>()) {
		return PyErr_Format(PyExc_Exception, "argument is not a class");
	}

	USceneComponent *component = NewObject<USceneComponent>(actor, (UClass *)py_obj->ue_object, FName(UTF8_TO_TCHAR(name)));
	if (!component)
		return PyErr_Format(PyExc_Exception, "unable to create component");

	actor->SetRootComponent(component);

	component->RegisterComponent();

	PyObject *ret = (PyObject *)ue_get_python_wrapper(component);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

static PyObject *py_ue_actor_has_component_of_type(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:actor_has_component_of_type", &obj)) {
		return NULL;
	}

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	if (actor->GetComponentByClass((UClass *)py_obj->ue_object)) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;

}

static PyObject *py_ue_get_actor_component_by_type(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:get_actor_component_by_type", &obj)) {
		return NULL;
	}

	ue_PyUObject *py_obj = nullptr;

	if (PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		py_obj = (ue_PyUObject *)obj;
	}
	// shortcut for finding class by string
	else if (PyUnicode_Check(obj)) {
		char *class_name = PyUnicode_AsUTF8(obj);
		UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(class_name));

		if (u_class) {
			py_obj = ue_get_python_wrapper(u_class);
			if (py_obj)
				Py_INCREF(py_obj);
		}
	}

	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	UActorComponent *component = actor->GetComponentByClass((UClass *)py_obj->ue_object);
	if (component) {
		PyObject *ret = (PyObject *)ue_get_python_wrapper(component);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(ret);
		return ret;
	}

	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *py_ue_get_actor_components_by_type(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:get_actor_components_by_type", &obj)) {
		return NULL;
	}

	ue_PyUObject *py_obj = nullptr;

	if (PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		py_obj = (ue_PyUObject *)obj;
	}
	// shortcut for finding class by string
	else if (PyUnicode_Check(obj)) {
		char *class_name = PyUnicode_AsUTF8(obj);
		UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(class_name));

		if (u_class) {
			py_obj = ue_get_python_wrapper(u_class);
			if (py_obj)
				Py_INCREF(py_obj);
		}
	}

	if (!py_obj)
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");

	if (!py_obj->ue_object->IsA<UClass>())
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");

	PyObject *components = PyList_New(0);

	for(UActorComponent *component: actor->GetComponentsByClass((UClass *)py_obj->ue_object)) {
		ue_PyUObject *item = ue_get_python_wrapper(component);
		if (item)
			PyList_Append(components, (PyObject *)item);
	}

	return components;

}


static PyObject *py_ue_actor_spawn(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float x = 0, y = 0, z = 0;
	float roll = 0, pitch = 0, yaw = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O|ffffff:actor_spawn", &obj, &x, &y, &z, &roll, &pitch, &yaw)) {
		return NULL;
	}

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	FVector const location = FVector(x, y, z);
	FRotator const rotation = FRotator(pitch, yaw, roll);

	AActor *actor = world->SpawnActor((UClass *)py_obj->ue_object, &location, &rotation);

	PyObject *ret = (PyObject *)ue_get_python_wrapper(actor);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;

}

static PyObject *py_ue_set_view_target(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *obj;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "O|i:set_view_target", &obj, &controller_id)) {
		return NULL;
	}

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "argument is not an actor");
	}

	AActor *actor = (AActor *)py_obj->ue_object;

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	controller->SetViewTarget(actor);

	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *py_ue_is_a(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:is_a", &obj)) {
		return NULL;
	}

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (self->ue_object->IsA((UClass *)py_obj->ue_object)) {
		Py_INCREF(Py_True);
		return Py_True;
	}


	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_get_overlapping_actors(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "cannot retrieve actor from UObject");

	PyObject *class_filter = nullptr;
	if (!PyArg_ParseTuple(args, "|O:get_overlapping_actors", &class_filter)) {
		return NULL;
	}
	
	UClass *filtering = AActor::StaticClass();

	if (class_filter) {
		
		if (!PyObject_IsInstance(class_filter, (PyObject *)&ue_PyUObjectType))
			return PyErr_Format(PyExc_Exception, "argument is not a UObject");

		ue_PyUObject *py_obj = (ue_PyUObject *)class_filter;

		if (!py_obj->ue_object->IsA((UClass *)py_obj->ue_object))
			return PyErr_Format(PyExc_Exception, "argument is not a UClass");

		filtering = (UClass *)py_obj->ue_object;
	}

	PyObject *py_overlapping_actors = PyList_New(0);

	TArray<AActor *> overalpping_actors;
	actor->GetOverlappingActors(overalpping_actors, filtering);

	for (AActor *overlapping_actor : overalpping_actors) {
		ue_PyUObject *item = ue_get_python_wrapper(overlapping_actor);
		if (item)
			PyList_Append(py_overlapping_actors, (PyObject *)item);
	}
	return py_overlapping_actors;
}

static PyObject *py_ue_destructible_apply_damage(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float damage_amount = 0;
	float impulse_strength = 0;
	float x = 0, y = 0, z = 0;
	float ix = 0, iy = 0, iz = 0;
	if (!PyArg_ParseTuple(args, "ffffffff:destructible_apply_damage", &damage_amount, &impulse_strength, &x, &y, &z, &ix, &iy, &iz)) {
		return NULL;
	}

	UDestructibleComponent *destructible = nullptr;
	AActor *actor = nullptr;

	if (self->ue_object->IsA<UDestructibleComponent>()) {
		destructible = (UDestructibleComponent *)self->ue_object;
	}
	else if (self->ue_object->IsA<AActor>()) {
		actor = (AActor *)self->ue_object;
		destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		actor = (AActor *)self->ue_object->GetOuter();
		if (actor) {
			destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
		}
	}

	if (destructible) {
		destructible->ApplyDamage(damage_amount, FVector(x, y, z), FVector(ix, iy, iz), impulse_strength);
	}
	else {
		return PyErr_Format(PyExc_Exception, "UObject is not a destructible");
	}

	Py_INCREF(Py_False);
	return Py_False;
}

void unreal_engine_init_py_module() {
	PyImport_AppendInittab("unreal_engine", init_unreal_engine);
	PyObject *new_unreal_engine_module = PyImport_AddModule("unreal_engine");

	PyObject *unreal_engine_dict = PyModule_GetDict(new_unreal_engine_module);

	PyMethodDef *unreal_engine_function;
	for (unreal_engine_function = unreal_engine_methods; unreal_engine_function->ml_name != NULL; unreal_engine_function++) {
		PyObject *func = PyCFunction_New(unreal_engine_function, NULL);
		PyDict_SetItemString(unreal_engine_dict, unreal_engine_function->ml_name, func);
		Py_DECREF(func);
	}


	ue_PyUObjectType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ue_PyUObjectType) < 0)
		return;

	Py_INCREF(&ue_PyUObjectType);
	PyModule_AddObject(new_unreal_engine_module, "UObject", (PyObject *)&ue_PyUObjectType);


	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_CAMERA", PyLong_FromLong(ECollisionChannel::ECC_Camera));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_DESTRUCTIBLE", PyLong_FromLong(ECollisionChannel::ECC_Destructible));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_PAWN", PyLong_FromLong(ECollisionChannel::ECC_Pawn));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_PHYSICS_BODY", PyLong_FromLong(ECollisionChannel::ECC_PhysicsBody));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_VEHICLE", PyLong_FromLong(ECollisionChannel::ECC_Vehicle));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_VISIBILITY", PyLong_FromLong(ECollisionChannel::ECC_Visibility));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_WORLD_DYNAMIC", PyLong_FromLong(ECollisionChannel::ECC_WorldDynamic));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_WORLD_STATIC", PyLong_FromLong(ECollisionChannel::ECC_WorldStatic));

	PyDict_SetItemString(unreal_engine_dict, "ATTACHMENT_RULE_KEEP_RELATIVE", PyLong_FromLong((int)EAttachmentRule::KeepRelative));
	PyDict_SetItemString(unreal_engine_dict, "ATTACHMENT_RULE_KEEP_WORLD", PyLong_FromLong((int)EAttachmentRule::KeepWorld));
	PyDict_SetItemString(unreal_engine_dict, "ATTACHMENT_RULE_SNAP_TO_TARGET", PyLong_FromLong((int)EAttachmentRule::SnapToTarget));
}


ue_PyUObject *ue_get_python_wrapper(UObject *ue_obj) {
	if (!ue_obj || !ue_obj->IsValidLowLevel() || ue_obj->IsPendingKillOrUnreachable())
		return nullptr;
	UPyObject *ue_py_object = FindObject<UPyObject>(ue_obj, TEXT("__PyObject"), true);
	if (!ue_py_object) {
		ue_py_object = NewObject<UPyObject>(ue_obj, FName(TEXT("__PyObject")));
		if (!ue_py_object) {
			return nullptr;
		}
		ue_py_object->py_object = (ue_PyUObject *)PyObject_New(ue_PyUObject, &ue_PyUObjectType);
		if (!ue_py_object->py_object) {
			ue_py_object->ConditionalBeginDestroy();
			return nullptr;
		}
		((ue_PyUObject *)ue_py_object->py_object)->ue_object = ue_obj;


		Py_INCREF(ue_py_object->py_object);
	}

	return ue_py_object->py_object;
}

void unreal_engine_py_log_error() {
	PyObject *type = NULL;
	PyObject *value = NULL;
	PyObject *traceback = NULL;

	PyErr_Fetch(&type, &value, &traceback);
	PyErr_NormalizeException(&type, &value, &traceback);

	if (!value)
		goto end;

	char *msg = NULL;
	PyObject *zero = PyUnicode_AsUTF8String(PyObject_Str(value));
	if (zero) {
		msg = PyBytes_AsString(zero);
	}

	if (!msg)
		goto end;

	UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(msg));

	// taken from uWSGI ;)
	if (!traceback) goto end;

	PyObject *traceback_module = PyImport_ImportModule("traceback");
	if (!traceback_module) {
		return;
	}

	PyObject *traceback_dict = PyModule_GetDict(traceback_module);
	PyObject *format_exception = PyDict_GetItemString(traceback_dict, "format_exception");

	if (format_exception) {
		PyObject *ret = PyObject_CallFunctionObjArgs(format_exception, type, value, traceback, NULL);
		if (!ret)
			goto end;
		if (PyList_Check(ret)) {
			for (int i = 0; i < PyList_Size(ret); i++) {
				PyObject *item = PyList_GetItem(ret, i);
				if (item) {
					UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(PyUnicode_AsUTF8(PyObject_Str(item))));
				}
			}
		}
		else {
			UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(PyUnicode_AsUTF8(PyObject_Str(ret))));
		}
	}

end:
	PyErr_Clear();
}

UWorld *ue_get_uworld(ue_PyUObject *py_obj) {
	if (py_obj->ue_object->IsA<AActor>()) {
		AActor *actor = (AActor *)py_obj->ue_object;
		return actor->GetWorld();
	}

	if (py_obj->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)py_obj->ue_object;
		return component->GetWorld();
	}

	return nullptr;
}

AActor *ue_get_actor(ue_PyUObject *py_obj) {
	if (py_obj->ue_object->IsA<AActor>()) {
		return (AActor *)py_obj->ue_object;
	}

	if (py_obj->ue_object->IsA<UActorComponent>()) {
		UActorComponent *tmp_component = (UActorComponent *)py_obj->ue_object;
		return tmp_component->GetOwner();
	}
	return nullptr;
}

PyObject *ue_py_convert_property(UProperty *prop, uint8 *buffer) {
	if (auto casted_prop = Cast<UBoolProperty>(prop)) {
		bool value = casted_prop->GetPropertyValue_InContainer(buffer);
		if (value) {
			Py_INCREF(Py_True);
			return Py_True;
		}
		Py_INCREF(Py_False);
		return Py_False;
	}

	if (auto casted_prop = Cast<UIntProperty>(prop)) {
		int value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyLong_FromLong(value);
	}

	if (auto casted_prop = Cast<UFloatProperty>(prop)) {
		float value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyFloat_FromDouble(value);
	}

	if (auto casted_prop = Cast<UStrProperty>(prop)) {
		FString value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value));
	}

	if (auto casted_prop = Cast<UObjectProperty>(prop)) {
		auto value = casted_prop->GetPropertyValue_InContainer(buffer);
		if (value) {
			ue_PyUObject *ret = ue_get_python_wrapper(value);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			Py_INCREF(ret);
			return (PyObject *)ret;
		}
		goto end;
	}

	if (auto casted_prop = Cast<UClassProperty>(prop)) {
		auto value = casted_prop->GetPropertyValue_InContainer(buffer);
		if (value) {
			ue_PyUObject *ret = ue_get_python_wrapper(value);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			Py_INCREF(ret);
			return (PyObject *)ret;
		}
		goto end;
	}

	return PyErr_Format(PyExc_Exception, "unsupported value type for property %s", TCHAR_TO_UTF8(*prop->GetName()));

end:
	Py_INCREF(Py_None);
	return Py_None;
}

bool ue_py_convert_pyobject(PyObject *py_obj, UProperty *prop, uint8 *buffer) {
	
	if (PyBool_Check(py_obj)) {
		auto casted_prop = Cast<UBoolProperty>(prop);
		if (!casted_prop)
			return false;
		if (PyObject_IsTrue(py_obj)) {
			casted_prop->SetPropertyValue_InContainer(buffer, true);
		}
		else {
			casted_prop->SetPropertyValue_InContainer(buffer, false);
		}
		return true;
	}

	if (PyLong_Check(py_obj)) {
		auto casted_prop = Cast<UIntProperty>(prop);
		if (!casted_prop)
			return false;
		casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsLong(py_obj));
		return true;
	}

	if (PyFloat_Check(py_obj)) {
		auto casted_prop = Cast<UFloatProperty>(prop);
		if (!casted_prop)
			return false;
		casted_prop->SetPropertyValue_InContainer(buffer, PyFloat_AsDouble(py_obj));
		return true;
	}

	if (PyUnicode_Check(py_obj)) {
		auto casted_prop = Cast<UStrProperty>(prop);
		if (!casted_prop)
			return false;
		casted_prop->SetPropertyValue_InContainer(buffer, FString(UTF8_TO_TCHAR(PyUnicode_AsUTF8(py_obj))));
		return true;
	}

	if (PyObject_IsInstance(py_obj, (PyObject *)&ue_PyUObjectType)) {
		ue_PyUObject *ue_obj = (ue_PyUObject *)py_obj;
		if (ue_obj->ue_object->IsA<UClass>()) {
			auto casted_prop = Cast<UClassProperty>(prop);
			if (!casted_prop)
				return false;
			casted_prop->SetPropertyValue_InContainer(buffer, (UClass *)ue_obj->ue_object);
			return true;
		}

		if (ue_obj->ue_object->IsA<UObjectProperty>()) {
			auto casted_prop = Cast<UObjectProperty>(prop);
			if (!casted_prop)
				return false;
			casted_prop->SetPropertyValue_InContainer(buffer, ue_obj->ue_object);
			return true;
		}
	}

	return false;

}

ue_PyUObject *ue_is_pyuobject(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType))
		return nullptr;
	return (ue_PyUObject *)obj;
}
