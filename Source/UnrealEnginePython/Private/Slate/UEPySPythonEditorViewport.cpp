
#include "UEPySPythonEditorViewport.h"

#if WITH_EDITOR

#include "Components/DirectionalLightComponent.h"
#include "Wrappers/UEPyFEditorViewportClient.h"

static PyObject *py_ue_spython_editor_viewport_get_world(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	Py_RETURN_UOBJECT(py_SPythonEditorViewport->GetPythonWorld());
}

static PyObject *py_ue_spython_editor_viewport_get_editor_viewport_client(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	return py_ue_new_feditor_viewport_client(py_SPythonEditorViewport->GetViewportClient().ToSharedRef());
}

static PyObject *py_ue_spython_editor_viewport_set_show_bounds(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_bounds", &py_bool))
	{
		return NULL;
	}


	py_SPythonEditorViewport->GetViewportClient()->SetShowBounds(PyObject_IsTrue(py_bool) ? true : false);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_show_stats(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_stats", &py_bool))
	{
		return NULL;
	}


	py_SPythonEditorViewport->GetViewportClient()->SetShowStats(PyObject_IsTrue(py_bool) ? true : false);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_view_mode(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	int mode;
	if (!PyArg_ParseTuple(args, "i:set_view_mode", &mode))
	{
		return nullptr;
	}


	py_SPythonEditorViewport->GetViewportClient()->SetViewMode((EViewModeIndex)mode);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_exposure_settings(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
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


	py_SPythonEditorViewport->GetViewportClient()->ExposureSettings = *settings;

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_view_location(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	float x = 0, y = 0, z = 0;

	FVector vec;

	if (PyTuple_Size(args) == 1)
	{
		ue_PyFVector *ue_py_vec = py_ue_is_fvector(PyTuple_GetItem(args, 0));
		if (!ue_py_vec)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FVector");
		}
		vec = ue_py_vec->vec;
	}
	else
	{
		if (!PyArg_ParseTuple(args, "fff", &x, &y, &z))
		{
			return nullptr;;
		}
		vec.X = x;
		vec.Y = y;
		vec.Z = z;
	}

	py_SPythonEditorViewport->GetViewportClient()->SetViewLocation(vec);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_view_rotation(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	float roll = 0, pitch = 0, yaw = 0;

	FRotator rot;

	if (PyTuple_Size(args) == 1)
	{
		ue_PyFRotator *ue_py_rot = py_ue_is_frotator(PyTuple_GetItem(args, 0));
		if (!ue_py_rot)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FRotator");
		}
		rot = ue_py_rot->rot;
	}
	else
	{
		if (!PyArg_ParseTuple(args, "fff", &roll, &pitch, &yaw))
		{
			return nullptr;;
		}
		rot.Roll = roll;
		rot.Pitch = pitch;
		rot.Yaw = yaw;
	}

	py_SPythonEditorViewport->GetViewportClient()->SetViewRotation(rot);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_simulate(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:simulate", &py_bool))
	{
		return NULL;
	}

	py_SPythonEditorViewport->Simulate(PyObject_IsTrue(py_bool) != 0);

	Py_RETURN_NONE;
}

static PyObject *py_ue_spython_editor_viewport_set_light_color(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	PyObject *py_obj;

	if (!PyArg_ParseTuple(args, "O", &py_obj))
		return nullptr;

	ue_PyFColor *py_fcolor = py_ue_is_fcolor(py_obj);
	if (!py_fcolor)
		return PyErr_Format(PyExc_Exception, "argument is not a FColor");

	py_SPythonEditorViewport->GetPreviewScene()->SetLightColor(py_fcolor->color);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_light_direction(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	float roll = 0, pitch = 0, yaw = 0;

	FRotator rot;

	if (PyTuple_Size(args) == 1)
	{
		ue_PyFRotator *ue_py_rot = py_ue_is_frotator(PyTuple_GetItem(args, 0));
		if (!ue_py_rot)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FRotator");
		}
		rot = ue_py_rot->rot;
	}
	else
	{
		if (!PyArg_ParseTuple(args, "fff", &roll, &pitch, &yaw))
		{
			return nullptr;;
		}
		rot.Roll = roll;
		rot.Pitch = pitch;
		rot.Yaw = yaw;
	}

	py_SPythonEditorViewport->GetPreviewScene()->SetLightDirection(rot);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_sky_brightness(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	float brightness;

	if (!PyArg_ParseTuple(args, "f", &brightness))
		return nullptr;

	py_SPythonEditorViewport->GetPreviewScene()->SetSkyBrightness(brightness);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_set_light_brightness(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	float brightness;

	if (!PyArg_ParseTuple(args, "f", &brightness))
		return nullptr;

	py_SPythonEditorViewport->GetPreviewScene()->SetLightBrightness(brightness);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_spython_editor_viewport_get_light(ue_PySPythonEditorViewport *self, PyObject * args)
{
	ue_py_slate_cast(SPythonEditorViewport);
	UDirectionalLightComponent *light = py_SPythonEditorViewport->GetPreviewScene()->DirectionalLight;

	Py_RETURN_UOBJECT(light);
}

static PyMethodDef ue_PySPythonEditorViewport_methods[] = {
	{ "simulate", (PyCFunction)py_ue_spython_editor_viewport_simulate, METH_VARARGS, "" },
	{ "get_world", (PyCFunction)py_ue_spython_editor_viewport_get_world, METH_VARARGS, "" },
	{ "get_editor_viewport_client", (PyCFunction)py_ue_spython_editor_viewport_get_editor_viewport_client, METH_VARARGS, "" },
	{ "set_show_bounds", (PyCFunction)py_ue_spython_editor_viewport_set_show_bounds, METH_VARARGS, "" },
	{ "set_show_stats", (PyCFunction)py_ue_spython_editor_viewport_set_show_stats, METH_VARARGS, "" },
	{ "set_view_mode", (PyCFunction)py_ue_spython_editor_viewport_set_view_mode, METH_VARARGS, "" },
	{ "set_exposure_settings", (PyCFunction)py_ue_spython_editor_viewport_set_exposure_settings, METH_VARARGS, "" },
	{ "set_view_location", (PyCFunction)py_ue_spython_editor_viewport_set_view_location, METH_VARARGS, "" },
	{ "set_view_rotation", (PyCFunction)py_ue_spython_editor_viewport_set_view_rotation, METH_VARARGS, "" },
	{ "set_light_color", (PyCFunction)py_ue_spython_editor_viewport_set_light_color, METH_VARARGS, "" },
	{ "set_light_direction", (PyCFunction)py_ue_spython_editor_viewport_set_light_direction, METH_VARARGS, "" },
	{ "set_light_brightness", (PyCFunction)py_ue_spython_editor_viewport_set_light_brightness, METH_VARARGS, "" },
	{ "set_sky_brightness", (PyCFunction)py_ue_spython_editor_viewport_set_sky_brightness, METH_VARARGS, "" },
	{ "get_light", (PyCFunction)py_ue_spython_editor_viewport_get_light, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

class FPythonEditorViewportClient : public FEditorViewportClient
{
public:
	FPythonEditorViewportClient(FEditorModeTools * InModeTools, FPreviewScene * InPreviewScene, const TWeakPtr<SEditorViewport> & InEditorViewportWidget) :
		FEditorViewportClient(InModeTools, InPreviewScene, InEditorViewportWidget)
	{

		EngineShowFlags.SetSelection(true);
		EngineShowFlags.SetSelectionOutline(true);

		bAltPressed = false;

		SelectedActor = nullptr;
	}

	virtual void ProcessClick(FSceneView & InView, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
	{
		if (!HitProxy)
		{
			GEditor->SelectNone(true, true, true);
			SelectedActor = nullptr;
			return;
		}

		if (HitProxy->IsA(HActor::StaticGetType()))
		{
			HActor *h_actor = (HActor *)HitProxy;
			GEditor->SelectNone(true, true, true);
			GEditor->SelectActor(h_actor->Actor, true, true);

			SelectedActor = h_actor->Actor;

			SetWidgetMode(FWidget::WM_Translate);
		}
		else if (HitProxy->IsA(HWidgetAxis::StaticGetType()))
		{
			// TODO do something ?
		}
		else
		{
			GEditor->SelectNone(true, true, true);
			SelectedActor = nullptr;
		}
	}

	virtual FWidget::EWidgetMode GetWidgetMode() const
	{
		if (SelectedActor == nullptr)
			return FWidget::WM_None;
		return FEditorViewportClient::GetWidgetMode();
	}

	virtual FMatrix GetWidgetCoordSystem() const
	{
		if (!SelectedActor)
			return FMatrix::Identity;
		return FRotationMatrix(SelectedActor->GetActorTransform().Rotator());
	}

	virtual FVector GetWidgetLocation() const
	{
		if (!SelectedActor)
			return FVector(0, 0, 0);
		return SelectedActor->GetActorLocation();
	}

	virtual bool InputWidgetDelta(FViewport * InViewport, EAxisList::Type CurrentAxis, FVector & Drag, FRotator & Rot, FVector &Scale)
	{
		if (SelectedActor && !bAltPressed)
		{
			SelectedActor->AddActorWorldOffset(Drag);
			SelectedActor->AddActorWorldRotation(Rot);
			SelectedActor->SetActorScale3D(SelectedActor->GetActorScale3D() + Scale);
			Invalidate();
			return true;
		}

		return false;
	}

	virtual void TrackingStarted(const FInputEventState & InInpuState, bool bIsDragginWidget, bool bNudge)
	{
		bAltPressed = InInpuState.IsAltButtonPressed();
	}

	virtual void ResetCamera()
	{
		if (!SelectedActor)
			return;
		FocusViewportOnBox(SelectedActor->GetComponentsBoundingBox());
	}

protected:

	bool bAltPressed;
	AActor *SelectedActor;
};

UWorld *SPythonEditorViewport::GetPythonWorld()
{
	return GetWorld();
}

TSharedRef<FEditorViewportClient> SPythonEditorViewport::MakeEditorViewportClient()
{

	PreviewScene = new FPreviewScene();

	TSharedPtr<FEditorViewportClient> client = MakeShareable(new FPythonEditorViewportClient(nullptr, PreviewScene, SharedThis(this)));

	client->SetRealtime(true);

	client->SetViewportType(ELevelViewportType::LVT_Perspective);

	FExposureSettings settings;
	settings.bFixed = true;
#if ENGINE_MINOR_VERSION > 18
	settings.FixedEV100 = 0;
#else
	settings.LogOffset = 0;
#endif

	client->ExposureSettings = settings;

	return client.ToSharedRef();
}

TSharedPtr<FExtender> SPythonEditorViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender()));
	return Result;
}

TSharedRef<SEditorViewport> SPythonEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<SWidget> SPythonEditorViewport::MakeViewportToolbar()
{
	return SNew(SCommonEditorViewportToolbarBase, SharedThis(this));
}

void SPythonEditorViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SEditorViewport::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bSimulate)
	{
		GetWorld()->Tick(ELevelTick::LEVELTICK_All, InDeltaTime);
	}
}

PyTypeObject ue_PySPythonEditorViewportType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonEditorViewport", /* tp_name */
	sizeof(ue_PySPythonEditorViewport), /* tp_basicsize */
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
	"Unreal Engine SPythonEditorViewport",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonEditorViewport_methods,             /* tp_methods */
};

static int ue_py_spython_editor_viewport_init(ue_PySPythonEditorViewport *self, PyObject *args, PyObject *kwargs)
{
	ue_py_snew_simple(SPythonEditorViewport);
	return 0;
}

void ue_python_init_spython_editor_viewport(PyObject *ue_module)
{

	ue_PySPythonEditorViewportType.tp_init = (initproc)ue_py_spython_editor_viewport_init;

	ue_PySPythonEditorViewportType.tp_base = &ue_PySEditorViewportType;

	if (PyType_Ready(&ue_PySPythonEditorViewportType) < 0)
		return;

	Py_INCREF(&ue_PySPythonEditorViewportType);
	PyModule_AddObject(ue_module, "SPythonEditorViewport", (PyObject *)&ue_PySPythonEditorViewportType);
}
#endif