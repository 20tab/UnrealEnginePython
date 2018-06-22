#include "UEPyPawn.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

PyObject *py_ue_pawn_get_controller(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<APawn>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an APawn");
	}

	APawn *pawn = (APawn *)self->ue_object;

	Py_RETURN_UOBJECT(pawn->GetController());
}



