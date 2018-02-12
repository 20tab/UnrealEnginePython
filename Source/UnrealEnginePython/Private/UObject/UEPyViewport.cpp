#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "Editor/LevelEditor/Public/ILevelViewport.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"
#endif

PyObject *py_unreal_engine_get_game_viewport_client(PyObject * self, PyObject * args)
{

	UGameViewportClient *viewport_client = GEngine->GameViewport;
	if (!viewport_client)
	{
		return PyErr_Format(PyExc_Exception, "no engine GameViewport found");
	}
	Py_RETURN_UOBJECT(GEngine->GameViewport);
}

#if WITH_EDITOR
PyObject *py_unreal_engine_get_editor_pie_game_viewport_client(PyObject * self, PyObject * args)
{

	UGameViewportClient *viewport_client = GEditor->GameViewport;
	if (!viewport_client)
	{
		return PyErr_Format(PyExc_Exception, "no editor GameViewport found");
	}
	Py_RETURN_UOBJECT(viewport_client);
}

PyObject *py_unreal_engine_editor_set_view_mode(PyObject * self, PyObject * args)
{

	int mode;

	if (!PyArg_ParseTuple(args, "i:editor_set_view_mode", &mode))
	{
		return NULL;
	}

	FLevelEditorModule &EditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!EditorModule.GetFirstActiveViewport().IsValid())
		return PyErr_Format(PyExc_Exception, "no active LevelEditor Viewport");

	FLevelEditorViewportClient &viewport_client = EditorModule.GetFirstActiveViewport()->GetLevelViewportClient();

	viewport_client.SetViewMode((EViewModeIndex)mode);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_editor_set_camera_speed(PyObject * self, PyObject * args)
{

	int speed;

	if (!PyArg_ParseTuple(args, "f:editor_set_camera_speed", &speed))
	{
		return NULL;
	}

	FLevelEditorModule &EditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!EditorModule.GetFirstActiveViewport().IsValid())
		return PyErr_Format(PyExc_Exception, "no active LevelEditor Viewport");

	FLevelEditorViewportClient &viewport_client = EditorModule.GetFirstActiveViewport()->GetLevelViewportClient();

	viewport_client.SetCameraSpeedSetting(speed);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_editor_set_view_location(PyObject * self, PyObject * args)
{

	PyObject *py_vector;

	if (!PyArg_ParseTuple(args, "O:editor_set_view_location", &py_vector))
	{
		return NULL;
	}

	ue_PyFVector *vector = py_ue_is_fvector(py_vector);
	if (!vector)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");

	FLevelEditorModule &EditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!EditorModule.GetFirstActiveViewport().IsValid())
		return PyErr_Format(PyExc_Exception, "no active LevelEditor Viewport");

	FLevelEditorViewportClient &viewport_client = EditorModule.GetFirstActiveViewport()->GetLevelViewportClient();

	viewport_client.SetViewLocation(vector->vec);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_editor_set_view_rotation(PyObject * self, PyObject * args)
{

	PyObject *py_rotator;

	if (!PyArg_ParseTuple(args, "O:editor_set_view_rotation", &py_rotator))
	{
		return NULL;
	}

	ue_PyFRotator *rotator = py_ue_is_frotator(py_rotator);
	if (!rotator)
		return PyErr_Format(PyExc_Exception, "argument is not a FRotator");

	FLevelEditorModule &EditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!EditorModule.GetFirstActiveViewport().IsValid())
		return PyErr_Format(PyExc_Exception, "no active LevelEditor Viewport");

	FLevelEditorViewportClient &viewport_client = EditorModule.GetFirstActiveViewport()->GetLevelViewportClient();

	viewport_client.SetViewRotation(rotator->rot);

	Py_RETURN_NONE;
}

#endif

PyObject *py_ue_add_viewport_widget_content(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_widget;
	int z_order = 0;

	if (!PyArg_ParseTuple(args, "O|i:add_viewport_widget_content", &py_widget, &z_order))
	{
		return NULL;
	}

	UGameViewportClient *viewport = ue_py_check_type<UGameViewportClient>(self);
	if (!viewport)
	{
		UWorld *world = ue_py_check_type<UWorld>(self);
		if (!world)
			return PyErr_Format(PyExc_Exception, "object is not a GameViewportClient or a UWorld");
		viewport = world->GetGameViewport();
		if (!viewport)
			return PyErr_Format(PyExc_Exception, "cannot retrieve GameViewportClient from UWorld");
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_widget);
	if (!py_swidget)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}
	// Do not increment reference count as it is assumed this function is used in a PyComponent/PyActor/ that can holds reference to
	// it in various ways
	// Py_INCREF(py_swidget);

	viewport->AddViewportWidgetContent(py_swidget->s_widget, z_order);

	Py_RETURN_NONE;
}

PyObject *py_ue_remove_viewport_widget_content(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_widget;

	if (!PyArg_ParseTuple(args, "O:remove_viewport_widget_content", &py_widget))
	{
		return NULL;
	}

	UGameViewportClient *viewport = ue_py_check_type<UGameViewportClient>(self);
	if (!viewport)
		return PyErr_Format(PyExc_Exception, "object is not a GameViewportClient");

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_widget);
	if (!py_swidget)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}
	Py_DECREF(py_swidget);

	viewport->RemoveViewportWidgetContent(py_swidget->s_widget);

	Py_RETURN_NONE;
}

PyObject *py_ue_remove_all_viewport_widgets(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	UGameViewportClient *viewport = ue_py_check_type<UGameViewportClient>(self);
	if (!viewport)
		return PyErr_Format(PyExc_Exception, "object is not a GameViewportClient");


	viewport->RemoveAllViewportWidgets();

	Py_RETURN_NONE;
}

