#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_get_actor_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorLocation());

}

PyObject *py_ue_get_actor_scale(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorScale3D());

}

PyObject *py_ue_get_actor_forward(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorForwardVector());

}

PyObject *py_ue_get_actor_right(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorRightVector());

}

PyObject *py_ue_get_actor_up(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	return py_ue_new_fvector(actor->GetActorUpVector());

}

PyObject *py_ue_get_actor_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	FRotator rot = actor->GetActorRotation();

	return py_ue_new_frotator(rot);

}

PyObject *py_ue_set_actor_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	actor->SetActorLocation(vec);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_add_actor_world_offset(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	actor->AddActorWorldOffset(vec);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_add_actor_local_offset(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	actor->AddActorLocalOffset(vec);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_add_actor_world_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	actor->AddActorWorldRotation(rot);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_add_actor_local_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	actor->AddActorLocalRotation(rot);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_ue_set_actor_scale(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");

	actor->SetActorScale3D(vec);

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_set_actor_rotation(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;

	AActor *actor = ue_get_actor(self);
	if (!actor)
		return PyErr_Format(PyExc_Exception, "uobject is not an actor or a component");
	actor->SetActorRotation(rot);
	Py_INCREF(Py_None);
	return Py_None;

}


PyObject *py_ue_get_world_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetComponentLocation();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_world_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FRotator rot = ((USceneComponent *)self->ue_object)->GetComponentRotation();
		return py_ue_new_frotator(rot);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_world_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetComponentScale();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->RelativeLocation;
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FRotator rot = ((USceneComponent *)self->ue_object)->RelativeRotation;
		return py_ue_new_frotator(rot);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_relative_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->RelativeScale3D;
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_forward_vector(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetForwardVector();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_up_vector(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetUpVector();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_get_right_vector(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	if (self->ue_object->IsA<USceneComponent>()) {
		FVector vec3 = ((USceneComponent *)self->ue_object)->GetRightVector();
		return py_ue_new_fvector(vec3);
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetWorldLocation(vec);
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetWorldRotation(rot);
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_world_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetWorldScale3D(vec);
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_location(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetRelativeLocation(vec);
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_rotation(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return NULL;
	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetRelativeRotation(rot);
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}

PyObject *py_ue_set_relative_scale(ue_PyUObject *self, PyObject * args) {
	ue_py_check(self);
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	if (self->ue_object->IsA<USceneComponent>()) {
		((USceneComponent *)self->ue_object)->SetRelativeScale3D(vec);
		Py_INCREF(Py_None);
		return Py_None;
	}
	return PyErr_Format(PyExc_Exception, "uobject is not a USceneComponent");
}