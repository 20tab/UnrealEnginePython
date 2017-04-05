// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#if WITH_EDITOR
#include "SlateStyle.h"

struct FPythonImageBrush {
	FName property_name;
	FString image_name;
	FVector2D image_size;
};

class FUEPyStyle
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

PyObject *py_unreal_engine_add_image_brush(PyObject *self, PyObject *args);

#endif