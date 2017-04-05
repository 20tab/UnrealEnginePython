// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR
#include "EditorStyleSet.h"

TSharedPtr< FSlateStyleSet > FUEPyStyle::StyleSet = nullptr;

std::list<FPythonImageBrush *> *python_image_brushes;

void FUEPyStyle::Initialize()
{
	// Only register once
	if( StyleSet.IsValid() )
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet("UnrealEnginePython") );
	StyleSet->SetContentRoot(FPaths::GameContentDir());

	for (FPythonImageBrush *image_brush : *python_image_brushes) {
		StyleSet->Set(image_brush->property_name, new FSlateImageBrush(StyleSet->RootToContentDir(image_brush->image_name), image_brush->image_size));
	}

	FSlateStyleRegistry::RegisterSlateStyle( *StyleSet.Get() );
}

PyObject *py_unreal_engine_add_image_brush(PyObject *self, PyObject *args)
{
	char *property_name, *image_name;
	float width, height;
	if (!PyArg_ParseTuple(args, "ssff:add_image_brush", &property_name, &image_name, &width, &height)) {
		return NULL;
	}

	FPythonImageBrush *image_brush = new FPythonImageBrush;

	image_brush->property_name = FName(UTF8_TO_TCHAR(property_name));
	image_brush->image_name = FString(UTF8_TO_TCHAR(image_name));
	image_brush->image_size = FVector2D(width, height);

	if (!python_image_brushes)
		python_image_brushes = new std::list<FPythonImageBrush *>;

	python_image_brushes->push_back(image_brush);
	Py_RETURN_NONE;
}

void FUEPyStyle::Shutdown()
{
	if( StyleSet.IsValid() )
	{
		FSlateStyleRegistry::UnRegisterSlateStyle( *StyleSet.Get() );
		ensure( StyleSet.IsUnique() );
		StyleSet.Reset();
	}
}

const ISlateStyle& FUEPyStyle::Get()
{
	return *( StyleSet.Get() );
}

const FName& FUEPyStyle::GetStyleSetName()
{
	return StyleSet->GetStyleSetName();
}

#endif