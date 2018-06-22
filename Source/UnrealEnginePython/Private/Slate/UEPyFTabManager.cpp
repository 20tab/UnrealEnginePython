
#include "UEPyFTabManager.h"

static PyMethodDef ue_PyFTabManager_methods[] = {
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyFTabManager_str(ue_PyFTabManager *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FTabManager %p srefs %d>",
		&self->tab_manager.Get(), self->tab_manager.GetSharedReferenceCount());
}

static PyTypeObject ue_PyFTabManagerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FTabManager", /* tp_name */
	sizeof(ue_PyFTabManager), /* tp_basicsize */
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
	(reprfunc)ue_PyFTabManager_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FTabManager",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFTabManager_methods,             /* tp_methods */
};


void ue_python_init_ftab_manager(PyObject *ue_module)
{
	ue_PyFTabManagerType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFTabManagerType) < 0)
		return;

	Py_INCREF(&ue_PyFTabManagerType);
	PyModule_AddObject(ue_module, "FTabManager", (PyObject *)&ue_PyFTabManagerType);
}

PyObject *py_ue_new_ftab_manager(TSharedRef<FTabManager> tab_manager)
{
	ue_PyFTabManager *ret = (ue_PyFTabManager *)PyObject_New(ue_PyFTabManager, &ue_PyFTabManagerType);
	new(&ret->tab_manager) TSharedRef<FTabManager>(tab_manager);
	return (PyObject *)ret;
}
