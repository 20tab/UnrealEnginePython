// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorPrivatePCH.h"


#define LOCTEXT_NAMESPACE "PythonProjectEditorCommands"


FPythonProjectEditorCommands::FPythonProjectEditorCommands()
	: TCommands<FPythonProjectEditorCommands>("PythonEditor", LOCTEXT("General", "General"), NAME_None, FPythonEditorStyle::GetStyleSetName())
{
}


void FPythonProjectEditorCommands::RegisterCommands()
{
	UI_COMMAND(New, "New", "New Python Script.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::N));
	UI_COMMAND(NewDirectory, "New Directory", "New Directory.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control | EModifierKey::Shift, EKeys::N));
	UI_COMMAND(Delete, "Delete", "Delete Python Script.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::D));

	UI_COMMAND(Save, "Save", "Save the currently active document.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::S));
	UI_COMMAND(SaveAll, "Save All", "Save all open documents.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control | EModifierKey::Shift, EKeys::S));
	UI_COMMAND(Execute, "Execute", "Execute Current Python File.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::Enter));
	UI_COMMAND(ExecuteInSandbox, "Execute In Sandbox", "Execute Current Python File in a Sandbox.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control | EModifierKey::Shift, EKeys::Enter));

}


#undef LOCTEXT_NAMESPACE
