// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorPrivatePCH.h"
#include "PythonProjectFactory.h"


#define LOCTEXT_NAMESPACE "PythonEditor"


UPythonProjectFactory::UPythonProjectFactory(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UPythonProject::StaticClass();
}


UObject* UPythonProjectFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPythonProject* NewPythonProject = NewObject<UPythonProject>(InParent, Class, Name, Flags);
	return NewPythonProject;
}


#undef LOCTEXT_NAMESPACE
