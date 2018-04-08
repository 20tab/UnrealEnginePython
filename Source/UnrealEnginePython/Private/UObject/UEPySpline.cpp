#include "UEPySpline.h"


#include "Components/SplineComponent.h"


PyObject *py_ue_get_spline_length(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	USplineComponent *spline = nullptr;

	if (self->ue_object->IsA<USplineComponent>())
	{
		spline = (USplineComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a USplineComponent");
	}

	if (!spline)
	{
		return PyErr_Format(PyExc_Exception, "unable to get spline object");
	}

	return PyFloat_FromDouble(spline->GetSplineLength());
}


PyObject *py_ue_get_world_location_at_distance_along_spline(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);
	float distance = 0;

	if (!PyArg_ParseTuple(args, "f:get_world_location_at_distance_along_spline", &distance))
	{
		return NULL;
	}

	USplineComponent *spline = nullptr;

	if (self->ue_object->IsA<USplineComponent>())
	{
		spline = (USplineComponent *)self->ue_object;
	}
	else
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a USplineComponent");
	}

	if (!spline)
	{
		return PyErr_Format(PyExc_Exception, "unable to get spline object");
	}


	FVector location = spline->GetWorldLocationAtDistanceAlongSpline(distance);
	return py_ue_new_fvector(location);
}



