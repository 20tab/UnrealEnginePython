
#include "UEPyFMaterialEditorUtilities.h"
#if WITH_EDITOR

#include "Materials/Material.h"
#include "Runtime/Engine/Classes/EdGraph/EdGraph.h"

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

static PyObject *py_ue_update_material_after_graph_change(PyObject *cls, PyObject * args)
{
	PyObject *py_graph;

	if (!PyArg_ParseTuple(args, "O:update_material_after_graph_change", &py_graph))
		return nullptr;

	UEdGraph *Graph = ue_py_check_type<UEdGraph>(py_graph);
	if (!Graph)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UEdGraph");
	}

	FMaterialEditorUtilities::UpdateMaterialAfterGraphChange(Graph);
	Py_RETURN_NONE;
}

static PyObject *py_ue_command_apply(PyObject *cls, PyObject * args)
{
	PyObject *py_material;

	if (!PyArg_ParseTuple(args, "O:command_apply", &py_material))
		return nullptr;

	UMaterial *Material = ue_py_check_type<UMaterial>(py_material);
	if (!Material)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UMaterial");
	}

	IAssetEditorInstance *Instance = FAssetEditorManager::Get().FindEditorForAsset(Material, false);
	if (!Instance)
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve editor for UMaterial");
	}

	IMaterialEditor *MaterialEditor = (IMaterialEditor *)Instance;

	MaterialEditor->GetToolkitCommands()->ExecuteAction(FMaterialEditorCommands::Get().Apply.ToSharedRef());

	Py_RETURN_NONE;

}


static PyMethodDef ue_PyFMaterialEditorUtilities_methods[] = {
	{ "paste_nodes_here", (PyCFunction)py_ue_paste_nodes_here, METH_VARARGS | METH_CLASS, "" },
	{ "update_material_after_graph_change", (PyCFunction)py_ue_update_material_after_graph_change, METH_VARARGS | METH_CLASS, "" },
	{ "command_apply", (PyCFunction)py_ue_command_apply, METH_VARARGS | METH_CLASS, "" },
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