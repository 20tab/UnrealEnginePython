// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PythonProjectEditor.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

class FPythonProjectEditorToolbar : public TSharedFromThis<FPythonProjectEditorToolbar>
{
public:
	FPythonProjectEditorToolbar(TSharedPtr<class FPythonProjectEditor> InPythonProjectEditor)
		: PythonProjectEditor(InPythonProjectEditor) {}

	void AddEditorToolbar(TSharedPtr<FExtender> Extender);

private:
	void FillEditorToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the Python editor tool that owns us */
	TWeakPtr<class FPythonProjectEditor> PythonProjectEditor;
};
