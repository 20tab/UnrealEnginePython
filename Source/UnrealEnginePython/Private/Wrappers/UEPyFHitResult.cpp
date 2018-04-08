#include "UEPyFHitResult.h"

#include "GameFramework/Actor.h"

static PyObject *py_ue_fhitresult_get_reversed_hit(ue_PyFHitResult *self, PyObject * args)
{
	return py_ue_new_fhitresult(FHitResult::GetReversedHit(self->hit));
}

static PyMethodDef ue_PyFHitResult_methods[] = {

	{ "get_reversed_hit", (PyCFunction)py_ue_fhitresult_get_reversed_hit, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_fhitresult_get_location(ue_PyFHitResult *self, void *closure)
{
	return py_ue_new_fvector(self->hit.Location);
}

static PyObject *py_ue_fhitresult_get_normal(ue_PyFHitResult *self, void *closure)
{
	return py_ue_new_fvector(self->hit.Normal);
}

static PyObject *py_ue_fhitresult_get_impact_point(ue_PyFHitResult *self, void *closure)
{
	return py_ue_new_fvector(self->hit.ImpactPoint);
}

static PyObject *py_ue_fhitresult_get_impact_normal(ue_PyFHitResult *self, void *closure)
{
	return py_ue_new_fvector(self->hit.ImpactNormal);
}

static PyObject *py_ue_fhitresult_get_distance(ue_PyFHitResult *self, void *closure)
{
	return PyFloat_FromDouble(self->hit.Distance);
}

static PyObject *py_ue_fhitresult_get_time(ue_PyFHitResult *self, void *closure)
{
	return PyFloat_FromDouble(self->hit.Time);
}

static PyObject *py_ue_fhitresult_get_bone_name(ue_PyFHitResult *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->hit.BoneName.ToString()));
}

static PyObject *py_ue_fhitresult_get_actor(ue_PyFHitResult *self, void *closure)
{
	AActor *actor = self->hit.Actor.Get();
	if (!actor)
	{
		Py_RETURN_NONE;
	}
	Py_RETURN_UOBJECT(actor);
}



static PyGetSetDef ue_PyFHitResult_getseters[] = {
	{(char *)"location", (getter)py_ue_fhitresult_get_location, NULL, (char *)"", NULL },
	{(char *)"normal", (getter)py_ue_fhitresult_get_normal, NULL, (char *)"", NULL },
	{(char *)"actor", (getter)py_ue_fhitresult_get_actor, NULL, (char *)"", NULL },
	{(char *)"distance", (getter)py_ue_fhitresult_get_distance, NULL, (char *)"", NULL },
	{(char *)"impact_point", (getter)py_ue_fhitresult_get_impact_point, NULL, (char *)"", NULL },
	{(char *)"impact_normal", (getter)py_ue_fhitresult_get_impact_normal, NULL, (char *)"", NULL },
	{(char *)"time", (getter)py_ue_fhitresult_get_time, NULL, (char *)"", NULL },
	{(char *)"bone_name", (getter)py_ue_fhitresult_get_bone_name, NULL, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFHitResult_str(ue_PyFHitResult *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FHitResult {'location': {'x': %S, 'y': %S, 'x': %S}, 'impact_point': {'x': %S, 'y': %S, 'x': %S}}>",
		PyFloat_FromDouble(self->hit.Location.X), PyFloat_FromDouble(self->hit.Location.Y), PyFloat_FromDouble(self->hit.Location.Z),
		PyFloat_FromDouble(self->hit.ImpactPoint.X), PyFloat_FromDouble(self->hit.ImpactPoint.Y), PyFloat_FromDouble(self->hit.ImpactPoint.Z));
}

static PyTypeObject ue_PyFHitResultType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FHitResult", /* tp_name */
	sizeof(ue_PyFHitResult), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	(reprfunc)ue_PyFHitResult_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FHitResult",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFHitResult_methods,             /* tp_methods */
	0,
	ue_PyFHitResult_getseters,
};

void ue_python_init_fhitresult(PyObject *ue_module)
{
	ue_PyFHitResultType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFHitResultType) < 0)
		return;

	Py_INCREF(&ue_PyFHitResultType);
	PyModule_AddObject(ue_module, "FHitResult", (PyObject *)&ue_PyFHitResultType);
}

PyObject *py_ue_new_fhitresult(FHitResult hit)
{
	ue_PyFHitResult *ret = (ue_PyFHitResult *)PyObject_New(ue_PyFHitResult, &ue_PyFHitResultType);
	ret->hit = hit;
	return (PyObject *)ret;
}

ue_PyFHitResult *py_ue_is_fhitresult(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFHitResultType))
		return nullptr;
	return (ue_PyFHitResult *)obj;
}
