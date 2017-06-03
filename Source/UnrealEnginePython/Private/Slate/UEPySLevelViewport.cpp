#if WITH_EDITOR
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySLevelViewport.h"

#include "LevelEditor.h"
#include "Editor/LevelEditor/Public/ILevelEditor.h"

#define sw_level_viewport StaticCastSharedRef<SLevelViewport>(self->s_editor_viewport.s_compound_widget.s_widget.s_widget)

static PyObject *py_ue_slevel_viewport_get_world(ue_PySLevelViewport *self, PyObject * args) {

	ue_PyUObject *ret = ue_get_python_wrapper(sw_level_viewport->GetWorld());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

static PyObject *py_ue_slevel_viewport_set_show_bounds(ue_PySLevelViewport *self, PyObject * args) {
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_bounds", &py_bool)) {
		return NULL;
	}

	sw_level_viewport->GetViewportClient()->SetShowBounds(PyObject_IsTrue(py_bool) ? true : false);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_slevel_viewport_set_show_stats(ue_PySLevelViewport *self, PyObject * args) {
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_stats", &py_bool)) {
		return NULL;
	}

	sw_level_viewport->GetViewportClient()->SetShowStats(PyObject_IsTrue(py_bool) ? true : false);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_slevel_viewport_set_view_mode(ue_PySLevelViewport *self, PyObject * args) {
	int mode;
	if (!PyArg_ParseTuple(args, "i:set_view_mode", &mode)) {
		return NULL;
	}

	sw_level_viewport->GetViewportClient()->SetViewMode((EViewModeIndex)mode);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_slevel_viewport_set_exposure_settings(ue_PySPythonEditorViewport *self, PyObject * args) {
	PyObject *py_settings;
	if (!PyArg_ParseTuple(args, "O:set_exposure_settings", &py_settings)) {
		return NULL;
	}

	FExposureSettings *settings = ue_py_check_struct<FExposureSettings>(py_settings);
	if (!settings) {
		return PyErr_Format(PyExc_Exception, "argument is not a FExposureSettings");
	}

	sw_level_viewport->GetViewportClient()->ExposureSettings = *settings;

	Py_INCREF(self);
	return (PyObject *)self;
}

/*
PyObject *py_ue_spython_editor_viewport_simulate(ue_PySPythonEditorViewport *self, PyObject * args) {

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

static int ue_py_slevel_viewport_init(ue_PySLevelViewport *self, PyObject *args, PyObject *kwargs) {

	FLevelEditorModule &EditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	ue_py_slate_setup_farguments(SLevelViewport);

	arguments.ParentLevelEditor(EditorModule.GetFirstLevelEditor().ToSharedRef());

	ue_py_slate_farguments_optional_bool("realtime", Realtime);
	ue_py_slate_farguments_optional_enum("viewport_type", ViewportType, ELevelViewportType);
	
	ue_py_snew(SLevelViewport, s_editor_viewport.s_compound_widget.s_widget);

	EditorModule.GetFirstLevelEditor()->AddStandaloneLevelViewport(sw_level_viewport);

	return 0;
}

void ue_python_init_slevel_viewport(PyObject *ue_module) {

	ue_PySLevelViewportType.tp_init = (initproc)ue_py_slevel_viewport_init;

	ue_PySLevelViewportType.tp_base = &ue_PySEditorViewportType;

	if (PyType_Ready(&ue_PySLevelViewportType) < 0)
		return;

	Py_INCREF(&ue_PySLevelViewportType);
	PyModule_AddObject(ue_module, "SLevelViewport", (PyObject *)&ue_PySLevelViewportType);
}
#endif