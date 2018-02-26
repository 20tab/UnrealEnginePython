
#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "UEPyFMaterialEditorUtilities.h"




static PyObject *py_ue_paste_nodes_here(PyObject *cls, PyObject * args)
{
	PyObject *py_graph;
	float x;
	float y;

	if (!PyArg_ParseTuple(args, "O(ff):paste_nodes_here", &py_graph, &x, &y))
		return nullptr;

	UEdGraph *Graph = ue_py_check_type<UEdGraph>(py_graph);
	if (!Graph)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UEdGraph");
	}

	FMaterialEditorUtilities::PasteNodesHere(Graph, FVector2D(x, y));
	Py_RETURN_NONE;
}


static PyMethodDef ue_PyFMaterialEditorUtilities_methods[] = {
	{ "paste_nodes_here", (PyCFunction)py_ue_paste_nodes_here, METH_VARARGS | METH_CLASS, "" },
	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyFMaterialEditorUtilitiesType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FMaterialEditorUtilities", /* tp_name */
	sizeof(ue_PyFMaterialEditorUtilities), /* tp_basicsize */
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
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine MaterialEditorUtilities",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFMaterialEditorUtilities_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_fmaterial_editor_utilities_init(ue_PyFMaterialEditorUtilities *self, PyObject * args)
{
	PyErr_SetString(PyExc_Exception, "FMaterialEditorUtilities is a singleton");
	return -1;
}

void ue_python_init_fmaterial_editor_utilities(PyObject *ue_module)
{
	ue_PyFMaterialEditorUtilitiesType.tp_new = PyType_GenericNew;
	ue_PyFMaterialEditorUtilitiesType.tp_init = (initproc)py_ue_fmaterial_editor_utilities_init;

	if (PyType_Ready(&ue_PyFMaterialEditorUtilitiesType) < 0)
		return;

	Py_INCREF(&ue_PyFMaterialEditorUtilitiesType);
	PyModule_AddObject(ue_module, "FMaterialEditorUtilities", (PyObject *)&ue_PyFMaterialEditorUtilitiesType);
}

#endif