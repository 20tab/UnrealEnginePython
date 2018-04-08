#include "UEPySLevelViewport.h"

#if WITH_EDITOR




#include "LevelEditor.h"
#include "Editor/LevelEditor/Public/ILevelEditor.h"

static PyObject *py_ue_slevel_viewport_get_world(ue_PySLevelViewport *self, PyObject * args)
{
	ue_py_slate_cast(SLevelViewport);
	Py_RETURN_UOBJECT(py_SLevelViewport->GetWorld());
}

static PyObject *py_ue_slevel_viewport_set_show_bounds(ue_PySLevelViewport *self, PyObject * args)
{
	ue_py_slate_cast(SLevelViewport);
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_bounds", &py_bool))
	{
		return nullptr;
	}

	py_SLevelViewport->GetViewportClient()->SetShowBounds(PyObject_IsTrue(py_bool) ? true : false);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_slevel_viewport_set_show_stats(ue_PySLevelViewport *self, PyObject * args)
{
	ue_py_slate_cast(SLevelViewport);
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_stats", &py_bool))
	{
		return nullptr;
	}

	py_SLevelViewport->GetViewportClient()->SetShowStats(PyObject_IsTrue(py_bool) ? true : false);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_slevel_viewport_set_view_mode(ue_PySLevelViewport *self, PyObject * args)
{
	ue_py_slate_cast(SLevelViewport);
	int mode;
	if (!PyArg_ParseTuple(args, "i:set_view_mode", &mode))
	{
		return nullptr;
	}

	py_SLevelViewport->GetViewportClient()->SetViewMode((EViewModeIndex)mode);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_slevel_viewport_set_exposure_settings(ue_PySLevelViewport *self, PyObject * args)
{
	ue_py_slate_cast(SLevelViewport);
	PyObject *py_settings;
	if (!PyArg_ParseTuple(args, "O:set_exposure_settings", &py_settings))
	{
		return nullptr;
	}

	FExposureSettings *settings = ue_py_check_struct<FExposureSettings>(py_settings);
	if (!settings)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a FExposureSettings");
	}

	py_SLevelViewport->GetViewportClient()->ExposureSettings = *settings;

	Py_RETURN_SLATE_SELF;
}

/*
PyObject *py_ue_spython_editor_viewport_simulate(ue_PySLevelViewport *self, PyObject * args) {

	sw_python_editor_viewport->GetViewportClient()->SetIsSimulateInEditorViewport(true);

	Py_RETURN_NONE;

}*/

static PyMethodDef ue_PySLevelViewport_methods[] = {
	//{ "simulate", (PyCFunction)py_ue_slevel_viewport_simulate, METH_VARARGS, "" },
	{ "get_world", (PyCFunction)py_ue_slevel_viewport_get_world, METH_VARARGS, "" },
	{ "set_show_bounds", (PyCFunction)py_ue_slevel_viewport_set_show_bounds, METH_VARARGS, "" },
	{ "set_show_stats", (PyCFunction)py_ue_slevel_viewport_set_show_stats, METH_VARARGS, "" },
	{ "set_view_mode", (PyCFunction)py_ue_slevel_viewport_set_view_mode, METH_VARARGS, "" },
	{ "set_exposure_settings", (PyCFunction)py_ue_slevel_viewport_set_exposure_settings, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySLevelViewportType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SLevelViewport", /* tp_name */
	sizeof(ue_PySLevelViewport), /* tp_basicsize */
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
	"Unreal Engine SLevelViewport",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySLevelViewport_methods,             /* tp_methods */
};

static int ue_py_slevel_viewport_init(ue_PySLevelViewport *self, PyObject *args, PyObject *kwargs)
{

	FLevelEditorModule &EditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	ue_py_slate_setup_farguments(SLevelViewport);

	arguments.ParentLevelEditor(EditorModule.GetFirstLevelEditor().ToSharedRef());

	ue_py_slate_farguments_optional_bool("realtime", Realtime);
	ue_py_slate_farguments_optional_enum("viewport_type", ViewportType, ELevelViewportType);

	ue_py_snew(SLevelViewport);
	ue_py_slate_cast(SLevelViewport);

	EditorModule.GetFirstLevelEditor()->AddStandaloneLevelViewport(py_SLevelViewport);

	return 0;
}

void ue_python_init_slevel_viewport(PyObject *ue_module)
{

	ue_PySLevelViewportType.tp_init = (initproc)ue_py_slevel_viewport_init;

	ue_PySLevelViewportType.tp_base = &ue_PySEditorViewportType;

	if (PyType_Ready(&ue_PySLevelViewportType) < 0)
		return;

	Py_INCREF(&ue_PySLevelViewportType);
	PyModule_AddObject(ue_module, "SLevelViewport", (PyObject *)&ue_PySLevelViewportType);
}
#endif