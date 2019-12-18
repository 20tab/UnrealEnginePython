// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Runtime/SlateCore/Public/Styling/SlateStyle.h"

class FPythonEditorStyle
{
public:

	static void Initialize();
	static void Shutdown();

	static const ISlateStyle& Get();

	static const FName& GetStyleSetName();

private:

	/** Singleton instances of this style. */
	static TSharedPtr< class FSlateStyleSet > StyleSet;	
};