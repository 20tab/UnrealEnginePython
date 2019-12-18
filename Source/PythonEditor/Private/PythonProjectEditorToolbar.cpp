// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonProjectEditorToolbar.h"
#include "LevelEditorActions.h"
#include "SourceCodeNavigation.h"
#include "EditorStyleSet.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxExtender.h"
#include "PythonProjectEditor.h"
#include "PythonProjectEditorCommands.h"


void FPythonProjectEditorToolbar::AddEditorToolbar(TSharedPtr<FExtender> Extender)
{
	check(PythonProjectEditor.IsValid());
	TSharedPtr<FPythonProjectEditor> PythonProjectEditorPtr = PythonProjectEditor.Pin();

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		PythonProjectEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP( this, &FPythonProjectEditorToolbar::FillEditorToolbar ) );
}

void FPythonProjectEditorToolbar::FillEditorToolbar(FToolBarBuilder& ToolbarBuilder)
{
	TSharedPtr<FPythonProjectEditor> PythonProjectEditorPtr = PythonProjectEditor.Pin();

	ToolbarBuilder.BeginSection(TEXT("FileManagement"));
	{
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().New);
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().NewDirectory);
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().Delete);
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().Save);
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().SaveAll);
	}
	ToolbarBuilder.EndSection();

	ToolbarBuilder.BeginSection(TEXT("CodeExcute"));
	{
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().Execute);
#if PLATFORM_MAC
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().ExecuteInMainThread);
#endif
		ToolbarBuilder.AddToolBarButton(FPythonProjectEditorCommands::Get().PEP8ize);
	}
	ToolbarBuilder.EndSection();

}
