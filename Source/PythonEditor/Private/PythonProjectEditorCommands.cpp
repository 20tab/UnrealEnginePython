// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonProjectEditorCommands.h"
#include "PythonEditorStyle.h"

#define LOCTEXT_NAMESPACE "PythonProjectEditorCommands"


FPythonProjectEditorCommands::FPythonProjectEditorCommands()
	: TCommands<FPythonProjectEditorCommands>("PythonEditor", LOCTEXT("General", "General"), NAME_None, FPythonEditorStyle::GetStyleSetName())
{
}


void FPythonProjectEditorCommands::RegisterCommands()
{
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	UI_COMMAND(New, "New", "New Python Script.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));
	UI_COMMAND(NewDirectory, "New Directory", "New Directory.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::N));
	UI_COMMAND(Delete, "Delete", "Delete Python Script.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::D));

	UI_COMMAND(Save, "Save", "Save the currently active document.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::S));
	UI_COMMAND(SaveAll, "Save All", "Save all open documents.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::S));
	UI_COMMAND(Execute, "Execute", "Execute Current Python File.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::Enter));
#if PLATFORM_MAC
	UI_COMMAND(ExecuteInMainThread, "Execute In Main Thread", "Execute Current Python File in the Main Thread.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::Enter));
#endif
	UI_COMMAND(PEP8ize, "PEP8-ize", "Enforce PEP8 to the current code.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::P));
#else
	UI_COMMAND(New, "New", "New Python Script.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::N));
	UI_COMMAND(NewDirectory, "New Directory", "New Directory.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control | EModifierKey::Shift, EKeys::N));
	UI_COMMAND(Delete, "Delete", "Delete Python Script.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::D));

	UI_COMMAND(Save, "Save", "Save the currently active document.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::S));
	UI_COMMAND(SaveAll, "Save All", "Save all open documents.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control | EModifierKey::Shift, EKeys::S));
	UI_COMMAND(Execute, "Execute", "Execute Current Python File.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control, EKeys::Enter));
#if PLATFORM_MAC
	UI_COMMAND(ExecuteInMainThread, "Execute In Main Thread", "Execute Current Python File in the Main Thread.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control | EModifierKey::Shift, EKeys::Enter));
#endif
	UI_COMMAND(PEP8ize, "PEP8-ize", "Enforce PEP8 to the current code.", EUserInterfaceActionType::Button, FInputGesture(EModifierKey::Control | EModifierKey::Shift, EKeys::P));
#endif

}


#undef LOCTEXT_NAMESPACE
