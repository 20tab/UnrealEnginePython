#include "UnrealEnginePythonPrivatePCH.h"


PyObject *py_ue_pawn_get_controller(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	if (!self->ue_object->IsA<APawn>())
	{
		return PyErr_Format(PyExc_Exception, "uobject is not an APawn");
	}

	APawn *pawn = (APawn *)self->ue_object;

	ue_PyUObject *ret = ue_get_python_wrapper(pawn->GetController());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}



