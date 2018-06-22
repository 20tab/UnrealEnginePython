#include "UEPyTransform.h"

#include "GameFramework/Actor.h"
#include "Wrappers/UEPyFHitResult.h"

static bool check_vector_args(PyObject *args, FVector &vec, bool &sweep, bool &teleport_physics)
{
	PyObject *py_vec = nullptr;
	float x = 0, y = 0, z = 0;
	PyObject *py_sweep = nullptr;
	PyObject *py_teleport_physics = nullptr;

	if (!PyArg_ParseTuple(args, "O|OO", &py_vec, &py_sweep, &py_teleport_physics))
	{
		return false;
	}

	ue_PyFVector *ue_py_vec = py_ue_is_fvector(py_vec);
	if (!ue_py_vec)
	{
		if (!PyArg_ParseTuple(args, "fff|OO", &x, &y, &z, &py_sweep, &py_teleport_physics))
		{
			return false;
		}
		vec.X = x;
		vec.Y = y;
		vec.Z = z;
	}
	else
	{
		vec = ue_py_vec->vec;
	}

	sweep = (py_sweep && PyObject_IsTrue(py_sweep));
	teleport_physics = (py_teleport_physics && PyObject_IsTrue(py_teleport_physics));

	return true;
}

static bool check_rotation_args(PyObject *args, FQuat &quat, bool &sweep, bool &teleport_physics)
{
	PyObject *py_rotation = nullptr;
	float roll = 0, pitch = 0, yaw = 0;
	PyObject *py_sweep = nullptr;
	PyObject *py_teleport_physics = nullptr;

	if (!PyArg_ParseTuple(args, "O|OO", &py_rotation, &py_sweep, &py_teleport_physics))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "fff|OO", &roll, &pitch, &yaw, &py_sweep, &py_teleport_physics))
		{
			return false;
		}
	}

	if (py_rotation)
	{
		ue_PyFQuat *ue_py_quat = py_ue_is_fquat(py_rotation);
		if (ue_py_quat)
		{
			quat = ue_py_quat->quat;
		}
		else
		{
			ue_PyFRotator *ue_py_rot = py_ue_is_frotator(py_rotation);
			if (!ue_py_rot)
			{
				PyErr_SetString(PyExc_Exception, "argument is not a FQuat or FRotator");
				return false;
			}
			quat = ue_py_rot->rot.Quaternion();
		}
	}
	else
	{
		quat = FQuat(FRotator(pitch, yaw, roll));
	}

	sweep = (py_sweep && PyObject_IsTrue(py_sweep));
	teleport_physics = (py_teleport_physics && PyObject_IsTrue(py_teleport_physics));

	return true;
}

static bool check_rotation_args_no_sweep(PyObject *args, FQuat &quat, bool &teleport_physics)
{
	PyObject *py_rotation = nullptr;
	float roll = 0, pitch = 0, yaw = 0;
	PyObject *py_teleport_physics = nullptr;

	if (!PyArg_ParseTuple(args, "O|O", &py_rotation, &py_teleport_physics))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "fff|O", &roll, &pitch, &yaw, &py_teleport_physics))
		{
			return false;
		}
	}

	if (py_rotation)
	{
		ue_PyFQuat *ue_py_quat = py_ue_is_fquat(py_rotation);
		if (ue_py_quat)
		{
			quat = ue_py_quat->quat;
		}
		else
		{
			ue_PyFRotator *ue_py_rot = py_ue_is_frotator(py_rotation);
			if (!ue_py_rot)
			{
				PyErr_SetString(PyExc_Exception, "argument is not a FQuat or FRotator");
				return false;
			}
			quat = ue_py_rot->rot.Quaternion();
		}
	}
	else
	{
		quat = FQuat(FRotator(pitch, yaw, roll));
	}

	teleport_physics = (py_teleport_physics && PyObject_IsTrue(py_teleport_physics));

	return true;
}

PyObject *py_ue_get_actor_location(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorLocation());

}

PyObject *py_ue_get_actor_scale(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorScale3D());

}

PyObject *py_ue_get_actor_transform(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_ftransform(actor->GetActorTransform());

}

PyObject *py_ue_get_actor_forward(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorForwardVector());

}

PyObject *py_ue_get_actor_right(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorRightVector());

}

PyObject *py_ue_get_actor_up(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorUpVector());

}

PyObject *py_ue_get_actor_rotation(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	FRotator rot = actor->GetActorRotation();

	return py_ue_new_frotator(rot);

}

PyObject *py_ue_set_actor_location(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FVector vec;
	bool sweep;
	bool teleport_physics;
	if (!check_vector_args(args, vec, sweep, teleport_physics))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	FHitResult hit;
	bool success = false;

	success = actor->SetActorLocation(vec, sweep, &hit, teleport_physics ? ETeleportType::TeleportPhysics : ETeleportType::None);

	if (!sweep)
	{
		if (success)
		{
			Py_RETURN_TRUE;
		}
		Py_RETURN_FALSE;
	}

	return Py_BuildValue("(OO)", success ? Py_True : Py_False, py_ue_new_fhitresult(hit));
}

PyObject *py_ue_add_actor_world_offset(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FVector vec;
	bool sweep;
	bool teleport_physics;
	if (!check_vector_args(args, vec, sweep, teleport_physics))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	FHitResult hit;

	actor->AddActorWorldOffset(vec, sweep, &hit, teleport_physics ? ETeleportType::TeleportPhysics : ETeleportType::None);

	if (!sweep)
	{
		Py_RETURN_NONE;
	}

	return py_ue_new_fhitresult(hit);
}

PyObject *py_ue_add_actor_local_offset(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FVector vec;
	bool sweep;
	bool teleport_physics;
	if (!check_vector_args(args, vec, sweep, teleport_physics))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	FHitResult hit;
	actor->AddActorLocalOffset(vec, sweep, &hit, teleport_physics ? ETeleportType::TeleportPhysics : ETeleportType::None);

	if (!sweep)
	{
		Py_RETURN_NONE;
	}

	return py_ue_new_fhitresult(hit);

}

PyObject *py_ue_add_actor_world_rotation(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FQuat quat;
	bool sweep;
	bool teleport_physics;
	if (!check_rotation_args(args, quat, sweep, teleport_physics))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	FHitResult hit;

	actor->AddActorWorldRotation(quat, sweep, &hit, teleport_physics ? ETeleportType::TeleportPhysics : ETeleportType::None);

	if (!sweep)
	{
		Py_RETURN_NONE;
	}

	return py_ue_new_fhitresult(hit);
}

PyObject *py_ue_add_actor_local_rotation(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FQuat quat;
	bool sweep;
	bool teleport_physics;
	if (!check_rotation_args(args, quat, sweep, teleport_physics))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	FHitResult hit;


	actor->AddActorLocalRotation(quat, sweep, &hit, teleport_physics ? ETeleportType::TeleportPhysics : ETeleportType::None);

	if (!sweep)
	{
		Py_RETURN_NONE;
	}

	return py_ue_new_fhitresult(hit);

}

PyObject *py_ue_set_actor_scale(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");


	actor->SetActorScale3D(vec);


	Py_RETURN_NONE;

}

PyObject *py_ue_set_actor_rotation(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FQuat quat;
	bool teleport_physics;
	if (!check_rotation_args_no_sweep(args, quat, teleport_physics))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	bool success = false;


	success = actor->SetActorRotation(quat, teleport_physics ? ETeleportType::TeleportPhysics : ETeleportType::None);


	if (success)
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}


PyObject *py_ue_set_actor_transform(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FTransform t;
	if (!py_ue_transform_arg(args, t))
		return nullptr;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");


	actor->SetActorTransform(t);


	Py_RETURN_NONE;
}


PyObject *py_ue_get_world_location(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetComponentLocation();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_world_rotation(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FRotator rot = ((USceneComponent *)self->ue_object)->GetComponentRotation();
		return py_ue_new_frotator(rot);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_world_scale(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetComponentScale();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_location(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FVector vec3 = ((USceneComponent *)self->ue_object)->RelativeLocation;
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_rotation(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FRotator rot = ((USceneComponent *)self->ue_object)->RelativeRotation;
		return py_ue_new_frotator(rot);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_scale(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FVector vec3 = ((USceneComponent *)self->ue_object)->RelativeScale3D;
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_transform(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	USceneComponent *component = ue_py_check_type<USceneComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	FTransform t = component->GetRelativeTransform();
	return py_ue_new_ftransform(t);
}

PyObject *py_ue_get_world_transform(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	USceneComponent *component = ue_py_check_type<USceneComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	FTransform t = component->GetComponentTransform();
	return py_ue_new_ftransform(t);
}

PyObject *py_ue_get_forward_vector(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetForwardVector();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_up_vector(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetUpVector();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_right_vector(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>())
	{
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetRightVector();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_location(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	FVector vec;
	bool sweep;
	bool teleport_physics;
	if (!check_vector_args(args, vec, sweep, teleport_physics))
		return nullptr;

	FHitResult hit;

	if (self->ue_object->IsA<USceneComponent>())
	{

		((USceneComponent *)self->ue_object)->SetWorldLocation(vec, sweep, &hit, teleport_physics ? ETeleportType::TeleportPhysics : ETeleportType::None);

		if (!sweep)
		{
			Py_RETURN_NONE;
		}
		return py_ue_new_fhitresult(hit);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_rotation(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;
	if (self->ue_object->IsA<USceneComponent>())
	{
	
		((USceneComponent *)self->ue_object)->SetWorldRotation(rot);
	
		Py_RETURN_NONE;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_scale(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	if (self->ue_object->IsA<USceneComponent>())
	{
	
		((USceneComponent *)self->ue_object)->SetWorldScale3D(vec);
	
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_transform(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	FTransform t;

	if (!py_ue_transform_arg(args, t))
		return nullptr;

	USceneComponent *component = ue_py_check_type<USceneComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	
	component->SetWorldTransform(t);
	

	Py_RETURN_NONE;
}

PyObject *py_ue_set_relative_transform(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);

	FTransform t;

	if (!py_ue_transform_arg(args, t))
		return nullptr;

	USceneComponent *component = ue_py_check_type<USceneComponent>(self);
	if (!component)
		return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");

	
	component->SetRelativeTransform(t);

	Py_RETURN_NONE;
}

PyObject *py_ue_set_relative_location(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	if (self->ue_object->IsA<USceneComponent>())
	{
	
		((USceneComponent *)self->ue_object)->SetRelativeLocation(vec);
		
		Py_RETURN_NONE;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_rotation(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;
	if (self->ue_object->IsA<USceneComponent>())
	{
	
		((USceneComponent *)self->ue_object)->SetRelativeRotation(rot);
		
		Py_RETURN_NONE;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_scale(ue_PyUObject *self, PyObject * args)
{
	ue_py_check(self);
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	if (self->ue_object->IsA<USceneComponent>())
	{
		
		((USceneComponent *)self->ue_object)->SetRelativeScale3D(vec);
		
		Py_RETURN_NONE;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}
