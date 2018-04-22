#pragma once

#include "UEPySEditorViewport.h"

#if WITH_EDITOR

#include "Editor/UnrealEd/Public/SEditorViewport.h"
#include "Editor/UnrealEd/Public/EditorViewportClient.h"
#include "Runtime/Engine/Public/PreviewScene.h"
#include "Editor/UnrealEd/Public/SCommonEditorViewportToolbarBase.h"
#include "Editor/UnrealEd/Public/STransformViewportToolbar.h"

#include "EngineUtils.h"

extern PyTypeObject ue_PySEditorViewportType;

typedef struct
{
	ue_PySEditorViewport s_editor_viewport;
	/* Type-specific fields go here. */
} ue_PySPythonEditorViewport;

void ue_python_init_spython_editor_viewport(PyObject *);

class SPythonEditorViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{

public:
	UWorld * GetPythonWorld();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

	void Simulate(bool start)
	{
		if (start)
		{
			if (!bSimulate)
			{
				bSimulate = true;
				GetWorld()->BeginPlay();
			}
		}
		else
		{
			bSimulate = false;
		}
	}

	FPreviewScene * GetPreviewScene()
	{
		return PreviewScene;
	}

	TSharedPtr<FSceneViewport> GetSceneViewport()
	{
		return SceneViewport;
	}

	virtual void OnFloatingButtonClicked() override {};
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual TSharedRef<SEditorViewport> GetViewportWidget() override;

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;

	FPreviewScene *PreviewScene;

	bool bSimulate;
};

#endif