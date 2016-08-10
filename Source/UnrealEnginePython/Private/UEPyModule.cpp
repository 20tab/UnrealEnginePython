#include "UnrealEnginePythonPrivatePCH.h"

#include "Kismet/KismetSystemLibrary.h"


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
	{ NULL, NULL },
};





#define ue_py_check(py_u) if (!py_u->ue_object || !py_u->ue_object->IsValidLowLevel() || py_u->ue_object->IsPendingKillOrUnreachable())\
							return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state")

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

	UFloatProperty *u_float_property = Cast<UFloatProperty>(u_property);
	if (u_float_property)
		return Py_BuildValue("f", u_float_property->GetPropertyValue(u_property->ContainerPtrToValuePtr<float>(self->ue_object)));

	UObjectProperty *u_obj_property = Cast<UObjectProperty>(u_property);
	if (u_obj_property) {
		UObject *u_obj = u_obj_property->GetPropertyValue(u_property->ContainerPtrToValuePtr<UObject>(self->ue_object));
		if (!u_obj)
			goto end;
		PyObject *py_obj = (PyObject *)ue_get_python_wrapper(u_obj);
		if (!py_obj)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(py_obj);
		return py_obj;
	}

	return PyErr_Format(PyExc_Exception, "unsupported value type for property %s", property_name);

end:
	Py_INCREF(Py_None);
	return Py_None;

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

	if (PyFloat_Check(property_value)) {
		UFloatProperty *u_float_property = Cast<UFloatProperty>(u_property);
		if (u_float_property)
			u_float_property->SetPropertyValue(u_property->ContainerPtrToValuePtr<float>(self->ue_object), PyFloat_AsDouble(property_value));
	}

	else if (PyUnicode_Check(property_value)) {
		UStrProperty *u_str_property = Cast<UStrProperty>(u_property);
		if (u_str_property)
			u_str_property->SetPropertyValue(u_property->ContainerPtrToValuePtr<FString>(self->ue_object), FString(UTF8_TO_TCHAR(PyUnicode_AsUTF8(property_value))));
	}

	else {
		return PyErr_Format(PyExc_Exception, "unsupported property value");
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

static PyObject *py_ue_get_actor_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorLocation();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorLocation();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

static PyObject *py_ue_get_actor_forward(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorForwardVector();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorForwardVector();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

static PyObject *py_ue_get_actor_right(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorRightVector();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorRightVector();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

static PyObject *py_ue_get_actor_up(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorUpVector();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorUpVector();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.X, vec3.Y, vec3.Z);

}

static PyObject *py_ue_get_actor_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FRotator vec3;

	if (self->ue_object->IsA<AActor>()) {
		vec3 = ((AActor *)self->ue_object)->GetActorRotation();
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		vec3 = ((UActorComponent *)self->ue_object)->GetOwner()->GetActorRotation();
		goto ret;
	}


	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	return Py_BuildValue("fff", vec3.Pitch, vec3.Yaw, vec3.Roll);

}

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

static PyObject *py_ue_get_input_axis(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *axis_name;
	if (!PyArg_ParseTuple(args, "s:get_input_axis", &axis_name)) {
		return NULL;
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>()) {
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input) {
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	return Py_BuildValue("f", input->GetAxisValue(FName(UTF8_TO_TCHAR(axis_name))));

}

static PyObject *py_ue_bind_input_axis(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *axis_name;
	if (!PyArg_ParseTuple(args, "s:bind_input_axis", &axis_name)) {
		return NULL;
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>()) {
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input) {
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	input->BindAxis(FName(UTF8_TO_TCHAR(axis_name)));

	Py_INCREF(Py_None);
	return Py_None;

}



static PyObject *py_ue_quit_game(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	UKismetSystemLibrary::QuitGame(world, world->GetFirstPlayerController(), EQuitPreference::Quit);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_simple_move_to_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:simple_move_to_location", &x, &y, &z)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>()) {
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor) {
			if (actor->IsA<APawn>()) {
				pawn = (APawn *)actor;
			}
		}
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a pawn");

	AController *controller = pawn->GetController();
	if (!controller)
		return PyErr_Format(PyExc_Exception, "Pawn has no controller");

	world->GetNavigationSystem()->SimpleMoveToLocation(controller, FVector(x, y, z));

	Py_INCREF(Py_None);
	return Py_None;
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


static PyObject *py_ue_is_input_key_down(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	char *key;
	if (!PyArg_ParseTuple(args, "s:is_input_key_down", &key)) {
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();

	if (controller->IsInputKeyDown(key)) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

static PyObject *py_ue_enable_input(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	/*
	TODO manage controller index
	char *axis_name;
	if (!PyArg_ParseTuple(args, "s:get_input_axis", &axis_name)) {
		return NULL;
	}*/

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();
	if (!controller)
		return PyErr_Format(PyExc_Exception, "no player controller available");

	if (self->ue_object->IsA<AActor>()) {
		((AActor *)self->ue_object)->EnableInput(controller);
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		((UActorComponent *)self->ue_object)->GetOwner()->EnableInput(controller);
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	Py_INCREF(Py_None);
	return Py_None;

}


static PyObject *py_ue_set_actor_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float x, y, z;
	if (!PyArg_ParseTuple(args, "fff:set_actor_location", &x, &y, &z)) {
		return NULL;
	}

	if (self->ue_object->IsA<AActor>()) {
		((AActor *)self->ue_object)->SetActorLocation(FVector(x, y, z));
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		((UActorComponent *)self->ue_object)->GetOwner()->SetActorLocation(FVector(x, y, z));
		goto ret;
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *py_ue_set_actor_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	float pitch, yaw, roll;
	if (!PyArg_ParseTuple(args, "fff:set_actor_rotation", &pitch, &yaw, &roll)) {
		return NULL;
	}

	if (self->ue_object->IsA<AActor>()) {
		((AActor *)self->ue_object)->SetActorRotation(FRotator(pitch, yaw, roll));
		goto ret;
	}

	if (self->ue_object->IsA<UActorComponent>()) {
		((UActorComponent *)self->ue_object)->GetOwner()->SetActorRotation(FRotator(pitch, yaw, roll));
		goto ret;
	}

	return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

ret:
	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *py_ue_find_object(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	char *name;
	if (!PyArg_ParseTuple(args, "s:find_object", &name)) {
		return NULL;
	}
	UObject *u_object = nullptr;

	for (TObjectIterator<UObject> Itr; Itr; ++Itr) {
		UObject *u_obj = *Itr;
		if (u_obj->GetName().Equals(UTF8_TO_TCHAR(name))) {
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

static PyObject *py_ue_show_mouse_cursor(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:show_mouse_cursor", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();
	if (controller)
		controller->bShowMouseCursor = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_enable_click_events(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:enable_click_events", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = world->GetFirstPlayerController();
	if (controller)
		controller->bEnableClickEvents = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_enable_mouse_over_events(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:enable_mouse_over_events", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	APlayerController *controller = world->GetFirstPlayerController();
	if (controller)
		controller->bEnableMouseOverEvents = enabled;

	Py_INCREF(Py_None);
	return Py_None;
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

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

	PyObject *ret = PyList_New(0);

	for (UActorComponent *component : actor->GetComponents()) {
		ue_PyUObject *py_obj = ue_get_python_wrapper(component);
		if (!py_obj)
			continue;
		PyList_Append(ret, (PyObject *)py_obj);
	}

	return ret;
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

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "i|O:get_hit_result_under_cursor", &channel, &trace_complex)) {
		return NULL;
	}


	bool complex = false;
	if (trace_complex && PyObject_IsTrue(trace_complex)) {
		complex = true;
	}

	APlayerController *controller = world->GetFirstPlayerController();
	
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to find first player controller");
	
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
static PyObject *py_ue_add_actor_root_component(ue_PyUObject *, PyObject *);

static PyMethodDef ue_PyUObject_methods[] = {
	{ "get_actor_location", (PyCFunction)py_ue_get_actor_location, METH_VARARGS, "" },
	{ "get_actor_forward", (PyCFunction)py_ue_get_actor_forward, METH_VARARGS, "" },
	{ "get_actor_right", (PyCFunction)py_ue_get_actor_right, METH_VARARGS, "" },
	{ "get_actor_up", (PyCFunction)py_ue_get_actor_up, METH_VARARGS, "" },
	{ "get_actor_rotation", (PyCFunction)py_ue_get_actor_rotation, METH_VARARGS, "" },
	{ "get_actor_velocity", (PyCFunction)py_ue_get_actor_velocity, METH_VARARGS, "" },
	{ "set_actor_location", (PyCFunction)py_ue_set_actor_location, METH_VARARGS, "" },
	{ "set_actor_rotation", (PyCFunction)py_ue_set_actor_rotation, METH_VARARGS, "" },
	{ "get_property", (PyCFunction)py_ue_get_property, METH_VARARGS, "" },
	{ "set_property", (PyCFunction)py_ue_set_property, METH_VARARGS, "" },
	{ "properties", (PyCFunction)py_ue_properties, METH_VARARGS, "" },
	{ "call", (PyCFunction)py_ue_call, METH_VARARGS, "" },
	{ "get_owner", (PyCFunction)py_ue_get_owner, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_get_name, METH_VARARGS, "" },
	{ "get_full_name", (PyCFunction)py_ue_get_full_name, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "get_actor_label", (PyCFunction)py_ue_get_actor_label, METH_VARARGS, "" },
	{ "find_actor_by_label", (PyCFunction)py_ue_find_actor_by_label, METH_VARARGS, "" },
#endif
	{ "find_object", (PyCFunction)py_ue_find_object, METH_VARARGS, "" },
	{ "all_objects", (PyCFunction)py_ue_all_objects, METH_VARARGS, "" },
	{ "all_actors", (PyCFunction)py_ue_all_actors, METH_VARARGS, "" },
	{ "get_input_axis", (PyCFunction)py_ue_get_input_axis, METH_VARARGS, "" },
	{ "bind_input_axis", (PyCFunction)py_ue_bind_input_axis, METH_VARARGS, "" },
	{ "enable_input", (PyCFunction)py_ue_enable_input, METH_VARARGS, "" },
	{ "get_class", (PyCFunction)py_ue_get_class, METH_VARARGS, "" },
	{ "actor_components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },
	{ "quit_game", (PyCFunction)py_ue_quit_game, METH_VARARGS, "" },
	{ "simple_move_to_location", (PyCFunction)py_ue_simple_move_to_location, METH_VARARGS, "" },
	{ "is_input_key_down", (PyCFunction)py_ue_is_input_key_down, METH_VARARGS, "" },
	{ "actor_has_component_of_type", (PyCFunction)py_ue_actor_has_component_of_type, METH_VARARGS, "" },
	{ "actor_destroy", (PyCFunction)py_ue_actor_destroy, METH_VARARGS, "" },
	{ "actor_spawn", (PyCFunction)py_ue_actor_spawn, METH_VARARGS, "" },
	{ "actor_has_tag", (PyCFunction)py_ue_actor_has_tag, METH_VARARGS, "" },
	{ "get_actor_bounds", (PyCFunction)py_ue_get_actor_bounds, METH_VARARGS, "" },
	{ "line_trace_single_by_channel", (PyCFunction)py_ue_line_trace_single_by_channel, METH_VARARGS, "" },
	{ "line_trace_multi_by_channel", (PyCFunction)py_ue_line_trace_multi_by_channel, METH_VARARGS, "" },
	{ "get_hit_result_under_cursor", (PyCFunction)py_ue_get_hit_result_under_cursor, METH_VARARGS, "" },
	{ "show_mouse_cursor", (PyCFunction)py_ue_show_mouse_cursor, METH_VARARGS, "" },
	{ "enable_click_events", (PyCFunction)py_ue_enable_click_events, METH_VARARGS, "" },
	{ "enable_mouse_over_events", (PyCFunction)py_ue_enable_mouse_over_events, METH_VARARGS, "" },
	{ "destructible_apply_damage", (PyCFunction)py_ue_destructible_apply_damage, METH_VARARGS, "" },
	{ "set_view_target", (PyCFunction)py_ue_set_view_target, METH_VARARGS, "" },
	{ "add_actor_component", (PyCFunction)py_ue_add_actor_component, METH_VARARGS, "" },
	{ "add_actor_root_component", (PyCFunction)py_ue_add_actor_root_component, METH_VARARGS, "" },
	{ "get_actor_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "set_simulate_physics", (PyCFunction)py_ue_set_simulate_physics, METH_VARARGS, "" },
	{ "get_world", (PyCFunction)py_ue_get_world, METH_VARARGS, "" },
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

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!self->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "uobject is not an AActor");
	}

	AActor *actor = (AActor *)self->ue_object;

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


static PyObject *py_ue_actor_spawn(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float x = 0, y = 0, z = 0;
	float pitch = 0, yaw = 0, roll = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O|ffffff:actor_spawn", &obj, &x, &y, &z, &pitch, &yaw, &roll)) {
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

	float x = 0, y = 0, z = 0;
	float pitch = 0, yaw = 0, roll = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *obj;
	if (!PyArg_ParseTuple(args, "O:set_view_target", &obj)) {
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

	world->GetFirstPlayerController()->SetViewTarget(actor);

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
		return;

	char *msg = NULL;
	PyObject *zero = PyUnicode_AsUTF8String(PyObject_Str(value));
	if (zero) {
		msg = PyBytes_AsString(zero);
	}

	if (!msg)
		return;

	UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(msg));

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

