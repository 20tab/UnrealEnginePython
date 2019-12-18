// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorCustomization.h"

UPythonEditorCustomization::UPythonEditorCustomization(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const FControlCustomization& UPythonEditorCustomization::GetControl(const FName& ControlCustomizationName)
{
	static FControlCustomization Default;

	return Default;
}

const FTextCustomization& UPythonEditorCustomization::GetText(const FName& TextCustomizationName)
{
	static FTextCustomization Default;

	return Default;
}