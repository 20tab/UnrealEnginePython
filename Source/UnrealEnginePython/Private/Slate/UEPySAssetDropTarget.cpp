#include "UEPySAssetDropTarget.h"

#if WITH_EDITOR


static PyMethodDef ue_PySAssetDropTarget_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySAssetDropTargetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SAssetDropTarget", /* tp_name */
	sizeof(ue_PySAssetDropTarget), /* tp_basicsize */
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
	"Unreal Engine SAssetDropTarget",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySAssetDropTarget_methods,             /* tp_methods */
};

static int ue_py_sasset_drop_target_init(ue_PySAssetDropTarget *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SAssetDropTarget);

	ue_py_snew(SAssetDropTarget);
	return 0;
}

void ue_python_init_sasset_drop_target(PyObject *ue_module)
{

	ue_PySAssetDropTargetType.tp_init = (initproc)ue_py_sasset_drop_target_init;

	ue_PySAssetDropTargetType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySAssetDropTargetType) < 0)
		return;

	Py_INCREF(&ue_PySAssetDropTargetType);
	PyModule_AddObject(ue_module, "SAssetDropTarget", (PyObject *)&ue_PySAssetDropTargetType);
}
#endif
