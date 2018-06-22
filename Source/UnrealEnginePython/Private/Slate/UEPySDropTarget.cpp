#include "UEPySDropTarget.h"

#if WITH_EDITOR




static PyMethodDef ue_PySDropTarget_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySDropTargetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SDropTarget", /* tp_name */
	sizeof(ue_PySDropTarget), /* tp_basicsize */
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
	"Unreal Engine SDropTarget",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySDropTarget_methods,             /* tp_methods */
};

void ue_python_init_sdrop_target(PyObject *ue_module)
{

	ue_PySDropTargetType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySDropTargetType) < 0)
		return;

	Py_INCREF(&ue_PySDropTargetType);
	PyModule_AddObject(ue_module, "SDropTarget", (PyObject *)&ue_PySDropTargetType);
}
#endif
