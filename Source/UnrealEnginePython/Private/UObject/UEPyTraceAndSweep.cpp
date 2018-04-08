#include "UEPyTraceAndSweep.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Wrappers/UEPyFHitResult.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

PyObject *py_ue_line_trace_single_by_channel(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_start;
	PyObject *py_obj_end;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "OOi:line_trace_single_by_channel", &py_obj_start, &py_obj_end, &channel))
	{
		return NULL;
	}

	ue_PyFVector *start = py_ue_is_fvector(py_obj_start);
	ue_PyFVector *end = py_ue_is_fvector(py_obj_end);

	if (!start || !end)
		return PyErr_Format(PyExc_Exception, "start and end location must be vectors");

	FHitResult hit;

	bool got_hit = world->LineTraceSingleByChannel(hit, start->vec, end->vec, (ECollisionChannel)channel);

	if (got_hit)
	{
		return py_ue_new_fhitresult(hit);
	}

	Py_RETURN_NONE;

}

PyObject *py_ue_line_trace_multi_by_channel(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_start;
	PyObject *py_obj_end;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "OOi:line_trace_multi_by_channel", &py_obj_start, &py_obj_end, &channel))
	{
		return NULL;
	}

	ue_PyFVector *start = py_ue_is_fvector(py_obj_start);
	ue_PyFVector *end = py_ue_is_fvector(py_obj_end);

	if (!start || !end)
		return PyErr_Format(PyExc_Exception, "start and end location must be vectors");

	TArray<struct FHitResult> hits;
	hits.Reset();

	PyObject *hits_list = PyList_New(0);

	bool got_hits = world->LineTraceMultiByChannel(hits, start->vec, end->vec, (ECollisionChannel)channel);

	if (got_hits)
	{
		for (int i = 0; i < hits.Num(); i++)
		{
			FHitResult hit = hits[i];
			PyList_Append(hits_list, py_ue_new_fhitresult(hit));
		}
	}
	return hits_list;

}

PyObject *py_ue_get_hit_result_under_cursor(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	int channel;
	PyObject *trace_complex = nullptr;
	int controller_id = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "i|Oi:get_hit_result_under_cursor", &channel, &trace_complex, &controller_id))
	{
		return nullptr;
	}


	bool complex = false;
	if (trace_complex && PyObject_IsTrue(trace_complex))
	{
		complex = true;
	}

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	FHitResult hit;

	bool got_hit = controller->GetHitResultUnderCursor((ECollisionChannel)channel, complex, hit);

	if (got_hit)
	{
		return py_ue_new_fhitresult(hit);
	}

	Py_RETURN_NONE;

}

PyObject *py_ue_draw_debug_line(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_obj_start;
	PyObject *py_obj_end;
	PyObject *py_color;
	float duration = 0;
	float thickness = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "OOO|ff:draw_debug_line", &py_obj_start, &py_obj_end, &py_color, &duration, &thickness))
	{
		return NULL;
	}

	ue_PyFVector *start = py_ue_is_fvector(py_obj_start);
	ue_PyFVector *end = py_ue_is_fvector(py_obj_end);

	if (!start || !end)
		return PyErr_Format(PyExc_Exception, "start and end location must be vectors");

	ue_PyFLinearColor *py_linear_color = py_ue_is_flinearcolor(py_color);
	if (!py_linear_color)
		return PyErr_Format(PyExc_Exception, "argument is not a FLinearColor");

	UKismetSystemLibrary::DrawDebugLine(world, start->vec, end->vec, py_linear_color->color, duration, thickness);

	Py_RETURN_NONE;
}
