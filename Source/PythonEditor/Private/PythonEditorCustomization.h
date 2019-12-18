// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UnrealEd.h"
#include "PythonEditorCustomization.generated.h"

USTRUCT()
struct FTextCustomization
{
	GENERATED_USTRUCT_BODY()

	FTextCustomization()
		: Font("")
		, Color(0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	UPROPERTY(EditAnywhere, Category=Text)
	FString Font;

	UPROPERTY(EditAnywhere, Category=Text)
	FLinearColor Color;
};

USTRUCT()
struct FControlCustomization
{
	GENERATED_USTRUCT_BODY()

	FControlCustomization()
		: Color(0.0f, 0.0f, 0.0f, 1.0f)
	{
	}

	UPROPERTY(EditAnywhere, Category=Controls)
	FLinearColor Color;
};

UCLASS(Config=Editor)
class UPythonEditorCustomization : public UObject
{
	GENERATED_UCLASS_BODY()

	static const FControlCustomization& GetControl(const FName& ControlCustomizationName);

	static const FTextCustomization& GetText(const FName& TextCustomizationName);

private:
	UPROPERTY(EditAnywhere, EditFixedSize, Category=Controls)
	TArray<FControlCustomization> Controls;

	UPROPERTY(EditAnywhere, EditFixedSize, Category=Text)
	TArray<FTextCustomization> Text;
};