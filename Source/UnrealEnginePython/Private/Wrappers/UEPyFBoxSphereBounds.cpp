#include "UEPyFBoxSphereBounds.h"

static PyMethodDef ue_PyFBoxSphereBounds_methods[] = {
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_fboxspherebounds_get_boxextent(ue_PyFBoxSphereBounds *self, void *closure)
{
	return py_ue_new_fvector(self->bounds.BoxExtent);
}

static PyObject *py_ue_fboxspherebounds_get_origin(ue_PyFBoxSphereBounds *self, void *closure)
{
	return py_ue_new_fvector(self->bounds.Origin);
}

static PyObject *py_ue_fboxspherebounds_get_sphereradius(ue_PyFBoxSphereBounds *self, void *closure)
{
    return PyFloat_FromDouble(self->bounds.SphereRadius);
}

static int py_ue_fboxspherebounds_set_boxextent(ue_PyFBoxSphereBounds *self, PyObject *value, void *closure)
{
	if (ue_PyFVector *py_vec = py_ue_is_fvector(value))
	{
		self->bounds.BoxExtent = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a vector");
	return -1;
}

static int py_ue_fboxspherebounds_set_origin(ue_PyFBoxSphereBounds *self, PyObject *value, void *closure)
{
	if (ue_PyFVector *py_vec = py_ue_is_fvector(value))
	{
		self->bounds.Origin = py_vec->vec;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a vector");
	return -1;
}

static int py_ue_fboxspherebounds_set_sphereradius(ue_PyFBoxSphereBounds *self, PyObject *value, void *closure)
{
	if (value && PyNumber_Check(value))
	{
		PyObject *f_value = PyNumber_Float(value);
        self->bounds.SphereRadius = PyFloat_AsDouble(f_value);
		Py_DECREF(f_value);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not numeric");
	return -1;
}

static PyGetSetDef ue_PyFBoxSphereBounds_getseters[] = {
    {(char *) "box_extent", (getter)py_ue_fboxspherebounds_get_boxextent, (setter)py_ue_fboxspherebounds_set_boxextent, (char *)"", NULL},
    {(char *) "origin", (getter)py_ue_fboxspherebounds_get_origin, (setter)py_ue_fboxspherebounds_set_origin, (char *)"", NULL},
    {(char *) "sphere_radius", (getter)py_ue_fboxspherebounds_get_sphereradius, (setter)py_ue_fboxspherebounds_set_sphereradius, (char *)"", NULL},
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFBoxSphereBounds_str(ue_PyFBoxSphereBounds *self)
{
    FVector& e = self->bounds.BoxExtent;
    FVector& o = self->bounds.Origin;

    return PyUnicode_FromFormat("<unreal_engine.FBoxSphereBounds {'box_extent': (%S, %S, %S), 'origin': (%S, %S, %S), sphere_radius: %S}>",
        PyFloat_FromDouble(e.X),
        PyFloat_FromDouble(e.Y),
        PyFloat_FromDouble(e.Z),
        PyFloat_FromDouble(o.X),
        PyFloat_FromDouble(o.Y),
        PyFloat_FromDouble(o.Z),
        PyFloat_FromDouble(self->bounds.SphereRadius)
	);
}

PyTypeObject ue_PyFBoxSphereBoundsType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FBoxSphereBounds", /* tp_name */
	sizeof(ue_PyFBoxSphereBounds), /* tp_basicsize */
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
	(reprfunc)ue_PyFBoxSphereBounds_str, /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FBoxSphereBounds", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFBoxSphereBounds_methods, /* tp_methods */
	0,
	ue_PyFBoxSphereBounds_getseters,
};

static int ue_py_fboxspherebounds_init(ue_PyFBoxSphereBounds *self, PyObject *args, PyObject *kwargs)
{
	PyObject *py_boxextent = nullptr;
	PyObject *py_origin = nullptr;
    float radius = 0;
	PyObject *py_sphereradius = nullptr;
	if (!PyArg_ParseTuple(args, "|OOf:__init__", &py_boxextent, &py_origin, &radius))
	{
		return -1;
	}

	if (py_boxextent)
	{
		if (ue_PyFVector *py_vec = py_ue_is_fvector(py_boxextent))
		{
            self->bounds.BoxExtent = py_vec->vec;
		}
		else
		{
            PyErr_SetString(PyExc_Exception, "argument is not a FVector");
            return -1;
		}
	}

	if (py_origin)
	{
		if (ue_PyFVector *py_vec = py_ue_is_fvector(py_origin))
		{
            self->bounds.Origin = py_vec->vec;
		}
		else
		{
			PyErr_SetString(PyExc_Exception, "argument is not a FVector");
			return -1;
		}
	}

    self->bounds.SphereRadius = radius;
	return 0;
}

void ue_python_init_fboxspherebounds(PyObject *ue_module)
{
	ue_PyFBoxSphereBoundsType.tp_new = PyType_GenericNew;
	ue_PyFBoxSphereBoundsType.tp_init = (initproc)ue_py_fboxspherebounds_init;
	if (PyType_Ready(&ue_PyFBoxSphereBoundsType) < 0)
		return;

	Py_INCREF(&ue_PyFBoxSphereBoundsType);
	PyModule_AddObject(ue_module, "FBoxSphereBounds", (PyObject *)&ue_PyFBoxSphereBoundsType);
}

PyObject *py_ue_new_fboxspherebounds(FBoxSphereBounds bounds)
{
    ue_PyFBoxSphereBounds *ret = (ue_PyFBoxSphereBounds *)PyObject_New(ue_PyFBoxSphereBounds, &ue_PyFBoxSphereBoundsType);
    ret->bounds = bounds;
	return (PyObject *)ret;
}

ue_PyFBoxSphereBounds *py_ue_is_fboxspherebounds(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFBoxSphereBoundsType))
		return nullptr;
	return (ue_PyFBoxSphereBounds *)obj;
}

bool py_ue_boxspherebounds_arg(PyObject *args, FBoxSphereBounds &b)
{
	if (PyTuple_Size(args) == 1)
	{
		PyObject *arg = PyTuple_GetItem(args, 0);
		ue_PyFBoxSphereBounds *py_b = py_ue_is_fboxspherebounds(arg);
		if (!py_b)
		{
			PyErr_Format(PyExc_TypeError, "argument is not a FBoxSphereBounds");
			return false;
		}
		b = py_b->bounds;
		return true;
	}

    float bx, by, bz, ox, oy, oz, r;
	if (!PyArg_ParseTuple(args, "fffffff", &bx, &by, &bz, &ox, &oy, &oz, &r))
		return false;

    b.BoxExtent = FVector(bx, by, bz);
    b.Origin = FVector(ox, oy, oz);
    b.SphereRadius = r;
	return true;
}
