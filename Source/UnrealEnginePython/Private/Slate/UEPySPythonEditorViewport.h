#pragma once

#include "UnrealEnginePython.h"


#include "UEPySEditorViewport.h"

#include "Editor/UnrealEd/Public/SEditorViewport.h"
#include "Editor/UnrealEd/Public/EditorViewportClient.h"
#include "Runtime/Engine/Public/PreviewScene.h"

extern PyTypeObject ue_PySEditorViewportType;

typedef struct {
	ue_PySEditorViewport s_editor_viewport;
	/* Type-specific fields go here. */
} ue_PySPythonEditorViewport;

void ue_python_init_spython_editor_viewport(PyObject *);

class SPythonEditorViewport : public SEditorViewport {

public:
	UWorld *GetPythonWorld();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

	void Simulate(bool start) {
		if (start) {
			if (!bSimulate) {
				bSimulate = true;
				GetWorld()->BeginPlay();
			}
		}
		else {
			bSimulate = false;
		}
	}

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	bool bSimulate;
};
