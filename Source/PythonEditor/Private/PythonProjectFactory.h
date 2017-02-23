// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PythonProjectFactory.generated.h"

UCLASS()
class UPythonProjectFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
