#include "UEPyInput.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerInput.h"


PyObject *py_ue_is_input_key_down(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *key;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "s|i:is_input_key_down", &key, &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	if (controller->IsInputKeyDown(key))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_was_input_key_just_pressed(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *key;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "s|i:was_input_key_just_pressed", &key, &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	if (controller->WasInputKeyJustPressed(key))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_is_action_pressed(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *key;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "s|i:is_action_pressed", &key, &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);
	UPlayerInput *input = controller->PlayerInput;
	if (!input)
		goto end;

	for (FInputActionKeyMapping mapping : input->GetKeysForAction(key))
	{
		if (controller->WasInputKeyJustPressed(mapping.Key))
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
	}

end:
	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_was_input_key_just_released(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *key;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "s|i:was_input_key_just_released", &key, &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	if (controller->WasInputKeyJustReleased(key))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_is_action_released(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *key;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "s|i:is_action_released", &key, &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);
	UPlayerInput *input = controller->PlayerInput;
	if (!input)
		goto end;

	for (FInputActionKeyMapping mapping : input->GetKeysForAction(key))
	{
		if (controller->WasInputKeyJustReleased(mapping.Key))
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
	}

end:
	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_enable_input(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|i:enable_input", &controller_id))
	{
		return NULL;
	}

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	if (self->ue_object->IsA<AActor>())
	{
		((AActor *)self->ue_object)->EnableInput(controller);
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		((UActorComponent *)self->ue_object)->GetOwner()->EnableInput(controller);
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_get_input_axis(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *axis_name;
	if (!PyArg_ParseTuple(args, "s:get_input_axis", &axis_name))
	{
		return NULL;
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>())
	{
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input)
	{
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	return Py_BuildValue("f", input->GetAxisValue(FName(UTF8_TO_TCHAR(axis_name))));

}

PyObject *py_ue_bind_input_axis(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *axis_name;
	if (!PyArg_ParseTuple(args, "s:bind_input_axis", &axis_name))
	{
		return NULL;
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>())
	{
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input)
	{
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	input->BindAxis(FName(UTF8_TO_TCHAR(axis_name)));

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_show_mouse_cursor(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|Oi:show_mouse_cursor", &is_true, &controller_id))
	{
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	controller->bShowMouseCursor = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_enable_click_events(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|Oi:enable_click_events", &is_true, &controller_id))
	{
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	controller->bEnableClickEvents = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_enable_mouse_over_events(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "|Oi:enable_mouse_over_events", &is_true, &controller_id))
	{
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (controller)
		controller->bEnableMouseOverEvents = enabled;

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_bind_action(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *action_name;
	int key;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "siO:bind_action", &action_name, &key, &py_callable))
	{
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "object is not a callable");
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>())
	{
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input)
	{
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	UPythonDelegate *py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewDelegate(input, py_callable, nullptr);

	FInputActionBinding input_action_binding(FName(UTF8_TO_TCHAR(action_name)), (const EInputEvent)key);
	input_action_binding.ActionDelegate.BindDelegate(py_delegate, &UPythonDelegate::PyInputHandler);
	input->AddActionBinding(input_action_binding);

	Py_RETURN_NONE;

}

PyObject *py_ue_bind_axis(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *axis_name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:bind_axis", &axis_name, &py_callable))
	{
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "object is not a callable");
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>())
	{
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		input = ((UActorComponent *)self->ue_object)->GetOwner()->InputComponent;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input)
	{
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	UPythonDelegate *py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewDelegate(input, py_callable, nullptr);

	FInputAxisBinding input_axis_binding(FName(UTF8_TO_TCHAR(axis_name)));
	input_axis_binding.AxisDelegate.BindDelegate(py_delegate, &UPythonDelegate::PyInputAxisHandler);
	input->AxisBindings.Add(input_axis_binding);

	Py_RETURN_NONE;

}

PyObject *py_ue_bind_key(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *key_name;
	int key;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "siO:bind_key", &key_name, &key, &py_callable))
	{
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "object is not a callable");
	}

	UInputComponent *input = nullptr;

	if (self->ue_object->IsA<AActor>())
	{
		input = ((AActor *)self->ue_object)->InputComponent;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		if (!component->GetOwner())
			return PyErr_Format(PyExc_Exception, "component is still not mapped to an Actor");
		input = component->GetOwner()->InputComponent;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	}

	if (!input)
	{
		return PyErr_Format(PyExc_Exception, "no input manager for this uobject");
	}

	UPythonDelegate *py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewDelegate(input, py_callable, nullptr);

	FInputKeyBinding input_key_binding(FKey(UTF8_TO_TCHAR(key_name)), (const EInputEvent)key);
	input_key_binding.KeyDelegate.BindDelegate(py_delegate, &UPythonDelegate::PyInputHandler);
	input->KeyBindings.Add(input_key_binding);

	Py_RETURN_NONE;

}

PyObject *py_ue_bind_pressed_key(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	char *key_name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:bind_pressed_key", &key_name, &py_callable))
	{
		return NULL;
	}
	return py_ue_bind_key(self, Py_BuildValue("siO", key_name, EInputEvent::IE_Pressed, py_callable));
}

PyObject *py_ue_bind_released_key(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	char *key_name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:bind_released_key", &key_name, &py_callable))
	{
		return NULL;
	}
	return py_ue_bind_key(self, Py_BuildValue("siO", key_name, EInputEvent::IE_Released, py_callable));
}


PyObject *py_unreal_engine_get_engine_defined_action_mappings(PyObject * self, PyObject * args)
{
	PyObject *py_list = PyList_New(0);
	TArray<FInputActionKeyMapping> mappings = UPlayerInput::GetEngineDefinedActionMappings();
	for (FInputActionKeyMapping mapping : mappings)
	{
		PyObject *py_mapping = PyDict_New();
		PyDict_SetItemString(py_mapping, (char *)"action_name", PyUnicode_FromString(TCHAR_TO_UTF8(*mapping.ActionName.ToString())));
		PyDict_SetItemString(py_mapping, (char *)"key", PyUnicode_FromString(TCHAR_TO_UTF8(*mapping.Key.ToString())));
		PyDict_SetItemString(py_mapping, (char *)"alt", mapping.bAlt ? Py_True : Py_False);
		PyDict_SetItemString(py_mapping, (char *)"cmd", mapping.bCmd ? Py_True : Py_False);
		PyDict_SetItemString(py_mapping, (char *)"ctrl", mapping.bCtrl ? Py_True : Py_False);
		PyDict_SetItemString(py_mapping, (char *)"shift", mapping.bShift ? Py_True : Py_False);
		PyList_Append(py_list, py_mapping);
	}
	return py_list;
}

PyObject *py_ue_input_axis(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_fkey;
	float delta;
	float delta_time;
	int num_samples = 1;
	PyObject *py_gamepad = nullptr;

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "Off|iO:input_axis", &py_fkey, &delta, &delta_time, &num_samples, &py_gamepad))
	{
		return nullptr;;
	}

	APlayerController *controller = ue_py_check_type<APlayerController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "object is not a APlayerController");

	FKey *key = ue_py_check_struct<FKey>(py_fkey);
	if (!key)
		return PyErr_Format(PyExc_Exception, "argument is not a FKey");

	if (controller->InputAxis(*key, delta, delta_time, num_samples, py_gamepad && PyObject_IsTrue(py_gamepad)))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

PyObject *py_ue_input_key(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_fkey;
	int event_type;
	float amount = 0.0;
	PyObject *py_gamepad = nullptr;

	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "Oi|fO:input_key", &py_fkey, &event_type, &amount, &py_gamepad))
	{
		return nullptr;;
	}

	APlayerController *controller = ue_py_check_type<APlayerController>(self);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "object is not a APlayerController");

	FKey *key = ue_py_check_struct<FKey>(py_fkey);
	if (!key)
		return PyErr_Format(PyExc_Exception, "argument is not a FKey");

	if (controller->InputKey(*key, (EInputEvent)event_type, amount, py_gamepad && PyObject_IsTrue(py_gamepad)))
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

