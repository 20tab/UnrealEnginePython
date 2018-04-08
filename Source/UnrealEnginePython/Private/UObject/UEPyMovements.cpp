#include "UEPyMovements.h"

#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Wrappers/UEPyFVector.h"
#include "Wrappers/UEPyFRotator.h"
#include "GameFramework/CharacterMovementComponent.h"

PyObject *py_ue_add_controller_yaw_input(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	float val;
	if (!PyArg_ParseTuple(args, "f:add_controller_yaw_input", &val))
	{
		return NULL;
	}

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>())
	{
		pawn = (APawn *)self->ue_object;
	}
	else if (UActorComponent *component = ue_py_check_type<UActorComponent>(self))
	{
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<APawn>())
			{
				pawn = (APawn *)actor;
			}
		}
	}
	else if (APlayerController *player = ue_py_check_type<APlayerController>(self))
	{
		player->AddYawInput(val);
		Py_RETURN_NONE;
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a Pawn or a PlayerController");

	pawn->AddControllerYawInput(val);

	Py_RETURN_NONE;
}

PyObject *py_ue_add_controller_pitch_input(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	float val;
	if (!PyArg_ParseTuple(args, "f:add_controller_pitch_input", &val))
	{
		return NULL;
	}

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>())
	{
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<APawn>())
			{
				pawn = (APawn *)actor;
			}
		}
	}
	else if (APlayerController *player = ue_py_check_type<APlayerController>(self))
	{
		player->AddPitchInput(val);
		Py_RETURN_NONE;
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a Pawn or a PlayerController");

	pawn->AddControllerPitchInput(val);

	Py_RETURN_NONE;
}

PyObject *py_ue_add_controller_roll_input(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	float val;
	if (!PyArg_ParseTuple(args, "f:add_controller_roll_input", &val))
	{
		return NULL;
	}

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>())
	{
		pawn = (APawn *)self->ue_object;
	}
	else if (UActorComponent *component = ue_py_check_type<UActorComponent>(self))
	{
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<APawn>())
			{
				pawn = (APawn *)actor;
			}
		}
	}
	else if (APlayerController *player = ue_py_check_type<APlayerController>(self))
	{
		player->AddRollInput(val);
		Py_RETURN_NONE;
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a Pawn or a PlayerController");

	pawn->AddControllerRollInput(val);

	Py_RETURN_NONE;
}

PyObject *py_ue_add_movement_input(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_movement;
	float scale = 1;
	PyObject *py_force = nullptr;
	bool force = false;
	if (!PyArg_ParseTuple(args, "O|fO:add_movement_input", &py_obj_movement, &scale, &py_force))
	{
		return NULL;
	}

	if (py_force && PyObject_IsTrue(py_force))
	{
		force = true;
	}

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>())
	{
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<APawn>())
			{
				pawn = (APawn *)actor;
			}
		}
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a pawn");

	ue_PyFVector *movement = py_ue_is_fvector(py_obj_movement);

	if (!movement)
		return PyErr_Format(PyExc_Exception, "movement input must be a FVector");

	pawn->AddMovementInput(movement->vec, scale, force);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_get_control_rotation(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	APawn *pawn = nullptr;

	if (self->ue_object->IsA<APawn>())
	{
		pawn = (APawn *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<APawn>())
			{
				pawn = (APawn *)actor;
			}
		}
	}

	if (!pawn)
		return PyErr_Format(PyExc_Exception, "uobject is not a pawn");

	FRotator rot = pawn->GetControlRotation();
	return py_ue_new_frotator(rot);
}

PyObject *py_ue_jump(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	character->Jump();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_crouch(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	character->Crouch();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_stop_jumping(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	character->StopJumping();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_uncrouch(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	character->UnCrouch();

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_ue_launch(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_force;
	PyObject *py_xy_override = nullptr;
	PyObject *py_z_override = nullptr;
	bool xy_override = false;
	bool z_override;
	if (!PyArg_ParseTuple(args, "O|OO:launch", &py_obj_force, &py_xy_override, &z_override))
	{
		return NULL;
	}

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	if (py_xy_override && PyObject_IsTrue(py_xy_override))
	{
		xy_override = true;
	}

	if (py_z_override && PyObject_IsTrue(py_z_override))
	{
		z_override = true;
	}

	ue_PyFVector *force = py_ue_is_fvector(py_obj_force);

	if (!force)
		return PyErr_Format(PyExc_Exception, "launch force must be a FVector");

	character->LaunchCharacter(force->vec, xy_override, z_override);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_is_jumping(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	if (character->IsJumpProvidingForce())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_is_crouched(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	if (character->bIsCrouched)
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_is_falling(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	UMovementComponent *movement = character->GetMovementComponent();
	if (movement && movement->IsA<UCharacterMovementComponent>())
	{
		UCharacterMovementComponent *character_movement = (UCharacterMovementComponent *)movement;
		if (character_movement->IsFalling())
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_is_flying(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	UMovementComponent *movement = character->GetMovementComponent();
	if (movement && movement->IsA<UCharacterMovementComponent>())
	{
		UCharacterMovementComponent *character_movement = (UCharacterMovementComponent *)movement;
		if (character_movement->IsFlying())
		{
			Py_INCREF(Py_True);
			return Py_True;
		}
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_can_jump(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	if (character->CanJump())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}

PyObject *py_ue_can_crouch(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ACharacter *character = nullptr;

	if (self->ue_object->IsA<ACharacter>())
	{
		character = (ACharacter *)self->ue_object;
	}
	else if (self->ue_object->IsA<UActorComponent>())
	{
		UActorComponent *component = (UActorComponent *)self->ue_object;
		AActor *actor = component->GetOwner();
		if (actor)
		{
			if (actor->IsA<ACharacter>())
			{
				character = (ACharacter *)actor;
			}
		}
	}

	if (!character)
		return PyErr_Format(PyExc_Exception, "uobject is not a character");

	if (character->CanCrouch())
	{
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}