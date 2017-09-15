#include "UnrealEnginePythonPrivatePCH.h"



PyObject *py_ue_simple_move_to_location(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return NULL;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

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

	AController *controller = pawn->GetController();
	if (!controller)
		return PyErr_Format(PyExc_Exception, "Pawn has no controller");

	world->GetNavigationSystem()->SimpleMoveToLocation(controller, vec);

	Py_INCREF(Py_None);
	return Py_None;
}

