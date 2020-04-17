#include "UEPyCapture.h"

#include "Runtime/MovieSceneCapture/Public/MovieSceneCapture.h"

#if WITH_EDITOR

/*

This is taken as-is (more or less) from MovieSceneCaptureDialogModule.cpp
to automate sequencer capturing. The only relevant implementation is the support
for a queue of UMovieSceneCapture objects

*/

#include "AudioDevice.h"
#include "Editor/EditorEngine.h"
#include "Slate/SceneViewport.h"
#include "AutomatedLevelSequenceCapture.h"

#include "Slate/UEPySPythonEditorViewport.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameMode.h"
#include "Runtime/CoreUObject/Public/Serialization/ObjectReader.h"
#include "Runtime/CoreUObject/Public/Serialization/ObjectWriter.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Runtime/Core/Public/Containers/Ticker.h"


struct FInEditorMultiCapture : TSharedFromThis<FInEditorMultiCapture>
{

	static TWeakPtr<FInEditorMultiCapture> CreateInEditorMultiCapture(TArray<UMovieSceneCapture*> InCaptureObjects, PyObject *py_callable)
	{
		// FInEditorCapture owns itself, so should only be kept alive by itself, or a pinned (=> temporary) weakptr
		FInEditorMultiCapture* Capture = new FInEditorMultiCapture;
		Capture->CaptureObjects = InCaptureObjects;
		Capture->py_callable = py_callable;
		if (Capture->py_callable)
			Py_INCREF(Capture->py_callable);
		for (UMovieSceneCapture *SceneCapture : Capture->CaptureObjects)
		{
			SceneCapture->AddToRoot();
		}
		Capture->Dequeue();
		return Capture->AsShared();
	}

private:
	FInEditorMultiCapture()
	{
		CapturingFromWorld = nullptr;
	}

	void Die()
	{
		for (UMovieSceneCapture *SceneCapture : CaptureObjects)
		{
			SceneCapture->RemoveFromRoot();
		}
		OnlyStrongReference = nullptr;
		{
			FScopePythonGIL gil;
			Py_XDECREF(py_callable);
		}
	}

	void Dequeue()
	{

		if (CaptureObjects.Num() < 1)
		{
			Die();
			return;
		}

		CurrentCaptureObject = CaptureObjects[0];

		check(CurrentCaptureObject);

		CapturingFromWorld = nullptr;

		if (!OnlyStrongReference.IsValid())
			OnlyStrongReference = MakeShareable(this);

		ULevelEditorPlaySettings* PlayInEditorSettings = GetMutableDefault<ULevelEditorPlaySettings>();

		bScreenMessagesWereEnabled = GAreScreenMessagesEnabled;
		GAreScreenMessagesEnabled = false;

		if (!CurrentCaptureObject->Settings.bEnableTextureStreaming)
		{
			const int32 UndefinedTexturePoolSize = -1;
			IConsoleVariable* CVarStreamingPoolSize = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streaming.PoolSize"));
			if (CVarStreamingPoolSize)
			{
				BackedUpStreamingPoolSize = CVarStreamingPoolSize->GetInt();
				CVarStreamingPoolSize->Set(UndefinedTexturePoolSize, ECVF_SetByConsole);
			}

			IConsoleVariable* CVarUseFixedPoolSize = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streaming.UseFixedPoolSize"));
			if (CVarUseFixedPoolSize)
			{
				BackedUpUseFixedPoolSize = CVarUseFixedPoolSize->GetInt();
				CVarUseFixedPoolSize->Set(0, ECVF_SetByConsole);
			}
		}

		// cleanup from previous run
		BackedUpPlaySettings.Empty();

		FObjectWriter(PlayInEditorSettings, BackedUpPlaySettings);

		OverridePlaySettings(PlayInEditorSettings);

		//CurrentCaptureObject->AddToRoot();
		CurrentCaptureObject->OnCaptureFinished().AddRaw(this, &FInEditorMultiCapture::OnEnd);

		UGameViewportClient::OnViewportCreated().AddRaw(this, &FInEditorMultiCapture::OnStart);
		FEditorDelegates::EndPIE.AddRaw(this, &FInEditorMultiCapture::OnEndPIE);

		FAudioDevice* AudioDevice = GEngine->GetMainAudioDevice();
		if (AudioDevice != nullptr)
		{
			TransientMasterVolume = AudioDevice->GetTransientMasterVolume();
			AudioDevice->SetTransientMasterVolume(0.0f);
		}

		// play at the next tick
		FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FInEditorMultiCapture::PlaySession), 0);
	}

	bool PlaySession(float DeltaTime)
	{


		if (py_callable)
		{
			GEditor->RequestEndPlayMap();
			FScopePythonGIL gil;
			ue_PyUObject *py_capture = ue_get_python_uobject(CurrentCaptureObject);
			PyObject *py_ret = PyObject_CallFunction(py_callable, "O", py_capture);
			if (!py_ret)
			{
				unreal_engine_py_log_error();
			}
			Py_XDECREF(py_ret);
		}


		GEditor->RequestPlaySession(true, nullptr, false);
		return false;
	}

	void OverridePlaySettings(ULevelEditorPlaySettings* PlayInEditorSettings)
	{
		const FMovieSceneCaptureSettings& Settings = CurrentCaptureObject->GetSettings();

		PlayInEditorSettings->NewWindowWidth = Settings.Resolution.ResX;
		PlayInEditorSettings->NewWindowHeight = Settings.Resolution.ResY;
		PlayInEditorSettings->CenterNewWindow = true;
		PlayInEditorSettings->LastExecutedPlayModeType = EPlayModeType::PlayMode_InEditorFloating;

		TSharedRef<SWindow> CustomWindow = SNew(SWindow)
			.Title(FText::FromString("Movie Render - Preview"))
			.AutoCenter(EAutoCenter::PrimaryWorkArea)
			.UseOSWindowBorder(true)
			.FocusWhenFirstShown(false)
#if ENGINE_MINOR_VERSION > 15
			.ActivationPolicy(EWindowActivationPolicy::Never)
#endif
			.HasCloseButton(true)
			.SupportsMaximize(false)
			.SupportsMinimize(true)
			.MaxWidth(Settings.Resolution.ResX)
			.MaxHeight(Settings.Resolution.ResY)
			.SizingRule(ESizingRule::FixedSize);

		FSlateApplication::Get().AddWindow(CustomWindow);

		PlayInEditorSettings->CustomPIEWindow = CustomWindow;

		// Reset everything else
		PlayInEditorSettings->GameGetsMouseControl = false;
		PlayInEditorSettings->ShowMouseControlLabel = false;
		PlayInEditorSettings->ViewportGetsHMDControl = false;
#if ENGINE_MINOR_VERSION >= 17
		PlayInEditorSettings->ShouldMinimizeEditorOnVRPIE = true;
		PlayInEditorSettings->EnableGameSound = false;
#endif
		PlayInEditorSettings->bOnlyLoadVisibleLevelsInPIE = false;
		PlayInEditorSettings->bPreferToStreamLevelsInPIE = false;
		PlayInEditorSettings->PIEAlwaysOnTop = false;
		PlayInEditorSettings->DisableStandaloneSound = true;
		PlayInEditorSettings->AdditionalLaunchParameters = TEXT("");
		PlayInEditorSettings->BuildGameBeforeLaunch = EPlayOnBuildMode::PlayOnBuild_Never;
		PlayInEditorSettings->LaunchConfiguration = EPlayOnLaunchConfiguration::LaunchConfig_Default;
		PlayInEditorSettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
		PlayInEditorSettings->SetRunUnderOneProcess(true);
		PlayInEditorSettings->SetPlayNetDedicated(false);
		PlayInEditorSettings->SetPlayNumberOfClients(1);
	}

	void OnStart()
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
				if (SlatePlayInEditorSession)
				{
					CapturingFromWorld = Context.World();

					TSharedPtr<SWindow> Window = SlatePlayInEditorSession->SlatePlayInEditorWindow.Pin();

					const FMovieSceneCaptureSettings& Settings = CurrentCaptureObject->GetSettings();

					SlatePlayInEditorSession->SlatePlayInEditorWindowViewport->SetViewportSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

					FVector2D PreviewWindowSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

					// Keep scaling down the window size while we're bigger than half the destop width/height
					{
						FDisplayMetrics DisplayMetrics;
						FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

						while (PreviewWindowSize.X >= DisplayMetrics.PrimaryDisplayWidth*.5f || PreviewWindowSize.Y >= DisplayMetrics.PrimaryDisplayHeight*.5f)
						{
							PreviewWindowSize *= .5f;
						}
					}

					// Resize and move the window into the desktop a bit
					FVector2D PreviewWindowPosition(50, 50);
					Window->ReshapeWindow(PreviewWindowPosition, PreviewWindowSize);

					if (CurrentCaptureObject->Settings.GameModeOverride != nullptr)
					{
						CachedGameMode = CapturingFromWorld->GetWorldSettings()->DefaultGameMode;
						CapturingFromWorld->GetWorldSettings()->DefaultGameMode = CurrentCaptureObject->Settings.GameModeOverride;
					}

					CurrentCaptureObject->Initialize(SlatePlayInEditorSession->SlatePlayInEditorWindowViewport, Context.PIEInstance);
				}
				return;
			}
		}

	}

	void Shutdown()
	{
		FEditorDelegates::EndPIE.RemoveAll(this);
		UGameViewportClient::OnViewportCreated().RemoveAll(this);
		CurrentCaptureObject->OnCaptureFinished().RemoveAll(this);

		GAreScreenMessagesEnabled = bScreenMessagesWereEnabled;

		if (!CurrentCaptureObject->Settings.bEnableTextureStreaming)
		{
			IConsoleVariable* CVarStreamingPoolSize = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streaming.PoolSize"));
			if (CVarStreamingPoolSize)
			{
				CVarStreamingPoolSize->Set(BackedUpStreamingPoolSize, ECVF_SetByConsole);
			}

			IConsoleVariable* CVarUseFixedPoolSize = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streaming.UseFixedPoolSize"));
			if (CVarUseFixedPoolSize)
			{
				CVarUseFixedPoolSize->Set(BackedUpUseFixedPoolSize, ECVF_SetByConsole);
			}
		}

		if (CurrentCaptureObject->Settings.GameModeOverride != nullptr)
		{
			CapturingFromWorld->GetWorldSettings()->DefaultGameMode = CachedGameMode;
		}

		FObjectReader(GetMutableDefault<ULevelEditorPlaySettings>(), BackedUpPlaySettings);

		FAudioDevice* AudioDevice = GEngine->GetMainAudioDevice();
		if (AudioDevice != nullptr)
		{
			AudioDevice->SetTransientMasterVolume(TransientMasterVolume);
		}

		CurrentCaptureObject->Close();
		//CurrentCaptureObject->RemoveFromRoot();

	}
	void OnEndPIE(bool bIsSimulating)
	{
		Shutdown();

		Die();
	}

	void NextCapture(bool bIsSimulating)
	{

		FEditorDelegates::EndPIE.RemoveAll(this);

		// remove item from the TArray;
		CaptureObjects.RemoveAt(0);

		if (CaptureObjects.Num() > 0)
		{
			Dequeue();
		}
		else
		{
			Die();
		}
	}

	void OnEnd()
	{
		Shutdown();

		FEditorDelegates::EndPIE.AddRaw(this, &FInEditorMultiCapture::NextCapture);
		GEditor->RequestEndPlayMap();
	}

	TSharedPtr<FInEditorMultiCapture> OnlyStrongReference;
	UWorld* CapturingFromWorld;

	bool bScreenMessagesWereEnabled;
	float TransientMasterVolume;
	int32 BackedUpStreamingPoolSize;
	int32 BackedUpUseFixedPoolSize;
	TArray<uint8> BackedUpPlaySettings;
	UMovieSceneCapture* CurrentCaptureObject;

	TSubclassOf<AGameModeBase> CachedGameMode;
	TArray<UMovieSceneCapture*> CaptureObjects;

	PyObject *py_callable;
};

PyObject *py_unreal_engine_in_editor_capture(PyObject * self, PyObject * args)
{
	PyObject *py_scene_captures;
	PyObject *py_callable = nullptr;

	if (!PyArg_ParseTuple(args, "O|O:in_editor_capture", &py_scene_captures, &py_callable))
	{
		return nullptr;
	}

	TArray<UMovieSceneCapture *> Captures;

	UMovieSceneCapture *capture = ue_py_check_type<UMovieSceneCapture>(py_scene_captures);
	if (!capture)
	{
		PyObject *py_iter = PyObject_GetIter(py_scene_captures);
		if (!py_iter)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a UMovieSceneCapture or an iterable of UMovieSceneCapture");
		}
		while (PyObject *py_item = PyIter_Next(py_iter))
		{
			capture = ue_py_check_type<UMovieSceneCapture>(py_item);
			if (!capture)
			{
				Py_DECREF(py_iter);
				return PyErr_Format(PyExc_Exception, "argument is not an iterable of UMovieSceneCapture");
			}
			Captures.Add(capture);
		}
		Py_DECREF(py_iter);
	}
	else
	{
		Captures.Add(capture);
	}

	Py_BEGIN_ALLOW_THREADS
		FInEditorMultiCapture::CreateInEditorMultiCapture(Captures, py_callable);
	Py_END_ALLOW_THREADS

		Py_RETURN_NONE;
}

PyObject *py_ue_set_level_sequence_asset(ue_PyUObject *self, PyObject *args)
{
	ue_py_check(self);

	PyObject *py_sequence = nullptr;

	if (!PyArg_ParseTuple(args, "O:set_level_sequence_asset", &py_sequence))
	{
		return nullptr;
	}

	ULevelSequence *sequence = ue_py_check_type<ULevelSequence>(py_sequence);
	if (!sequence)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a ULevelSequence");
	}

	UAutomatedLevelSequenceCapture *capture = ue_py_check_type<UAutomatedLevelSequenceCapture>(self);
	if (!capture)
		return PyErr_Format(PyExc_Exception, "uobject is not a UAutomatedLevelSequenceCapture");

#if ENGINE_MINOR_VERSION < 20
	capture->SetLevelSequenceAsset(sequence->GetPathName());
#else
	capture->LevelSequenceAsset = FSoftObjectPath(sequence->GetPathName());
#endif
	Py_RETURN_NONE;
}
#endif

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

		TSharedPtr<SPythonEditorViewport> Viewport = py_ue_is_swidget<SPythonEditorViewport>(py_widget);
		if (Viewport.IsValid())
		{
			capture->Initialize(Viewport->GetSceneViewport());
			capture->StartWarmup();
		}
		else
		{
			return PyErr_Format(PyExc_Exception, "argument is not a supported Viewport-based SWidget");
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
