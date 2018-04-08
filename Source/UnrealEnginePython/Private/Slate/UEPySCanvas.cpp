

#include "UEPySCanvas.h"


static PyObject *py_ue_scanvas_clear_children(ue_PySCanvas *self, PyObject * args)
{
	ue_py_slate_cast(SCanvas);
	py_SCanvas->ClearChildren();

	Py_RETURN_NONE;
}

static PyObject *py_ue_scanvas_add_slot(ue_PySCanvas *self, PyObject * args, PyObject *kwargs)
{
	ue_py_slate_cast(SCanvas);
	PyObject *py_content;
	int h_align = 0;
	int v_align = 0;

	PyObject *position = nullptr;
	PyObject *size = nullptr;

	char *kwlist[] = { (char *)"widget",
		(char *)"h_align",
		(char *)"v_align",
		(char *)"position",
		(char *)"size",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiOO:add_slot", kwlist,
		&py_content,
		&h_align,
		&v_align,
		&position,
		&size))
	{
		return NULL;
	}

	TSharedPtr<SWidget> child = py_ue_is_swidget<SWidget>(py_content);
	if (!child.IsValid())
		return nullptr;

	SCanvas::FSlot &fslot = py_SCanvas->AddSlot();
	fslot.AttachWidget(child.ToSharedRef());
	fslot.HAlign((EHorizontalAlignment)h_align);
	fslot.VAlign((EVerticalAlignment)v_align);

	if (position && PyTuple_Check(position))
	{
		if (PyTuple_Size(position) == 2)
		{
			PyObject *py_x = PyTuple_GetItem(position, 0);
			PyObject *py_y = PyTuple_GetItem(position, 1);
			if (PyNumber_Check(py_x))
			{
				PyObject *py_x_float = PyNumber_Float(py_x);
				float x = PyFloat_AsDouble(py_x_float);
				Py_DECREF(py_x_float);
				PyObject *py_y_float = PyNumber_Float(py_y);
				float y = PyFloat_AsDouble(py_y_float);
				Py_DECREF(py_y_float);
				fslot.Position(FVector2D(x, y));
			}
		}
	}

	if (size && PyTuple_Check(size))
	{
		if (PyTuple_Size(size) == 2)
		{
			PyObject *py_x = PyTuple_GetItem(size, 0);
			PyObject *py_y = PyTuple_GetItem(size, 1);
			if (PyNumber_Check(py_x))
			{
				PyObject *py_x_float = PyNumber_Float(py_x);
				float x = PyFloat_AsDouble(py_x_float);
				Py_DECREF(py_x_float);
				PyObject *py_y_float = PyNumber_Float(py_y);
				float y = PyFloat_AsDouble(py_y_float);
				Py_DECREF(py_y_float);
				fslot.Size(FVector2D(x, y));
			}
		}
	}

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySCanvas_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_scanvas_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ "clear_children", (PyCFunction)py_ue_scanvas_clear_children, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySCanvasType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SCanvas", /* tp_name */
	sizeof(ue_PySCanvas), /* tp_basicsize */
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
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SCanvas",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySCanvas_methods,             /* tp_methods */
};

static int ue_py_scanvas_init(ue_PySCanvas *self, PyObject *args, PyObject *kwargs)
{
	ue_py_snew_simple(SCanvas);
	return 0;
}

void ue_python_init_scanvas(PyObject *ue_module)
{

	ue_PySCanvasType.tp_base = &ue_PySPanelType;
	ue_PySCanvasType.tp_call = (ternaryfunc)py_ue_scanvas_add_slot;
	ue_PySCanvasType.tp_init = (initproc)ue_py_scanvas_init;

	if (PyType_Ready(&ue_PySCanvasType) < 0)
		return;

	Py_INCREF(&ue_PySCanvasType);
	PyModule_AddObject(ue_module, "SCanvas", (PyObject *)&ue_PySCanvasType);
}
