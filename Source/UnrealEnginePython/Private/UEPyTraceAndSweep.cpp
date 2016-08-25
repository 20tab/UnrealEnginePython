#include "UnrealEnginePythonPrivatePCH.h"

#include "Kismet/KismetSystemLibrary.h"

PyObject *py_ue_line_trace_single_by_channel(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float x1 = 0, y1 = 0, z1 = 0;
	float x2 = 0, y2 = 0, z2 = 0;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "ffffffi:line_trace_single_by_channel", &x1, &y1, &z1, &x2, &y2, &z2, &channel)) {
		return NULL;
	}

	FHitResult hit;

	bool got_hit = world->LineTraceSingleByChannel(hit, FVector(x1, y1, z1), FVector(x2, y2, z2), (ECollisionChannel)channel);

	if (got_hit) {
		AActor *actor = hit.GetActor();
		if (actor) {
			PyObject *ret = (PyObject *)ue_get_python_wrapper(actor);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			return Py_BuildValue("Offffff", ret, hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z, hit.ImpactNormal.X, hit.ImpactNormal.Y, hit.ImpactNormal.Z);
		}
	}

	return Py_BuildValue("Offffff", Py_None, 0, 0, 0, 0, 0, 0);

}

PyObject *py_ue_line_trace_multi_by_channel(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float x1 = 0, y1 = 0, z1 = 0;
	float x2 = 0, y2 = 0, z2 = 0;
	int channel;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "ffffffi:line_trace_multi_by_channel", &x1, &y1, &z1, &x2, &y2, &z2, &channel)) {
		return NULL;
	}

	TArray<struct FHitResult> hits;
	hits.Reset();

	PyObject *hits_list = PyList_New(0);


	bool got_hits = world->LineTraceMultiByChannel(hits, FVector(x1, y1, z1), FVector(x2, y2, z2), (ECollisionChannel)channel);

	if (got_hits) {
		for (int i = 0; i < hits.Num(); i++) {
			FHitResult hit = hits[i];
			AActor *actor = hit.GetActor();
			if (actor) {
				PyObject *ret = (PyObject *)ue_get_python_wrapper(actor);
				if (ret) {
					PyList_Append(hits_list, Py_BuildValue("Offffff", ret, hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z, hit.ImpactNormal.X, hit.ImpactNormal.Y, hit.ImpactNormal.Z));
				}
			}
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
		AActor *actor = hit.GetActor();
		if (actor) {
			PyObject *ret = (PyObject *)ue_get_python_wrapper(actor);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			return Py_BuildValue("Offffff", ret, hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z, hit.ImpactNormal.X, hit.ImpactNormal.Y, hit.ImpactNormal.Z);
		}
	}

	return Py_BuildValue("Offffff", Py_None, 0, 0, 0, 0, 0, 0);

}

PyObject *py_ue_draw_debug_line(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float x1 = 0, y1 = 0, z1 = 0;
	float x2 = 0, y2 = 0, z2 = 0;
	uint8 r = 1, g = 0, b = 0;
	float duration = 0;

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");


	if (!PyArg_ParseTuple(args, "ffffff|iiif:draw_debug_line", &x1, &y1, &z1, &x2, &y2, &z2, &r, &g, &b, &duration)) {
		return NULL;
	}

	UKismetSystemLibrary::DrawDebugLine(world, FVector(x1, y1, z1), FVector(x2, y2, z2), FColor(r, g, b), false, duration);

	Py_INCREF(Py_None);
	return Py_None;
}
