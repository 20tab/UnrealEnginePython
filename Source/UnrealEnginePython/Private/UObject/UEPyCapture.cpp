#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/MovieSceneCapture/Public/MovieSceneCapture.h"


PyObject *py_ue_capture_initialize(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_widget = nullptr;

	if (!PyArg_ParseTuple(args, "|O:capture_initialize", &py_widget))
	{
		return nullptr;
	}

	UMovieSceneCapture *capture = ue_py_check_type<UMovieSceneCapture>(self);
	if (!capture)
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneCapture");

#if WITH_EDITOR
	if (py_widget)
	{
		ue_PySWidget *s_widget = py_ue_is_swidget(py_widget);
		if (!s_widget)
			return PyErr_Format(PyExc_Exception, "argument is not a SWidget");


		if (s_widget->s_widget->GetType().Compare(FName("SPythonEditorViewport")) == 0)
		{
			TSharedRef<SPythonEditorViewport> s_viewport = StaticCastSharedRef<SPythonEditorViewport>(s_widget->s_widget);
			capture->Initialize(s_viewport->GetSceneViewport());
			capture->StartWarmup();
		}
		else
		{
			return PyErr_Format(PyExc_Exception, "argument is not a supported Viewport-based SWidget");
		}

	}
	else
	{
		for (const FWorldContext &Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				if (Context.GameViewport)
				{
					UE_LOG(LogPython, Error, TEXT("Found Viewport at %p"), Context.GameViewport);
					FSlatePlayInEditorInfo *SlatePIEInfo = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
					 
					capture->Initialize(SlatePIEInfo->SlatePlayInEditorWindowViewport);
				}
			}
		}
	}
#endif
	Py_RETURN_NONE;
}

PyObject *py_ue_capture_start(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UMovieSceneCapture *capture = ue_py_check_type<UMovieSceneCapture>(self);
	if (!capture)
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneCapture");

	capture->StartCapture();

	Py_RETURN_NONE;
}

PyObject *py_ue_capture_load_from_config(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UMovieSceneCapture *capture = ue_py_check_type<UMovieSceneCapture>(self);
	if (!capture)
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneCapture");

	capture->LoadFromConfig();

	Py_RETURN_NONE;
}

PyObject *py_ue_capture_stop(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UMovieSceneCapture *capture = ue_py_check_type<UMovieSceneCapture>(self);
	if (!capture)
		return PyErr_Format(PyExc_Exception, "uobject is not a UMovieSceneCapture");

	capture->Finalize();
	capture->Close();

	Py_RETURN_NONE;
}
