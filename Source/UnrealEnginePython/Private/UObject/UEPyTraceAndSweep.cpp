#include "UnrealEnginePythonPrivatePCH.h"

#include "Kismet/KismetSystemLibrary.h"

PyObject *py_ue_line_trace_single_by_channel(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_obj_start;
	PyObject *py_obj_end;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "OOi:line_trace_single_by_channel", &py_obj_start, &py_obj_end, &channel)) {
		return NULL;
	}

	ue_PyFVector *start = py_ue_is_fvector(py_obj_start);
	ue_PyFVector *end = py_ue_is_fvector(py_obj_end);

	if (!start || !end)
		return PyErr_Format(PyExc_Exception, "start and end location must be vectors");

	FHitResult hit;

	bool got_hit = world->LineTraceSingleByChannel(hit, start->vec, end->vec, (ECollisionChannel)channel);

	if (got_hit) {
		return py_ue_new_fhitresult(hit);
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_line_trace_multi_by_channel(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_obj_start;
	PyObject *py_obj_end;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "OOOi:line_trace_multi_by_channel", &py_obj_start, &py_obj_end, &channel)) {
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

	if (got_hits) {
		for (int i = 0; i < hits.Num(); i++) {
			FHitResult hit = hits[i];
			PyList_Append(hits_list, py_ue_new_fhitresult(hit));
		}
	}
	return hits_list;

}

PyObject *py_ue_get_hit_result_under_cursor(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	int channel;
	PyObject *trace_complex = NULL;
	int controller_id = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "i|Oi:get_hit_result_under_cursor", &channel, &trace_complex, &controller_id)) {
		return NULL;
	}


	bool complex = false;
	if (trace_complex && PyObject_IsTrue(trace_complex)) {
		complex = true;
	}

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	FHitResult hit;

	bool got_hit = controller->GetHitResultUnderCursor((ECollisionChannel)channel, complex, hit);

	if (got_hit) {
		return py_ue_new_fhitresult(hit);
	}

	Py_INCREF(Py_None);
	return Py_None;

}

PyObject *py_ue_draw_debug_line(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	PyObject *py_obj_start;
	PyObject *py_obj_end;
	uint8 r = 1, g = 0, b = 0;
	float duration = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "OO|iiif:draw_debug_line", &py_obj_start, &py_obj_end, &r, &g, &b, &duration)) {
		return NULL;
	}

	ue_PyFVector *start = py_ue_is_fvector(py_obj_start);
	ue_PyFVector *end = py_ue_is_fvector(py_obj_end);

	if (!start || !end)
		return PyErr_Format(PyExc_Exception, "start and end location must be vectors");

	UKismetSystemLibrary::DrawDebugLine(world, start->vec, end->vec, FColor(r, g, b), false, duration);

	Py_INCREF(Py_None);
	return Py_None;
}
