#pragma once

#include "UnrealEd.h"
#include "PythonScriptFactory.generated.h"

UCLASS()
class UPythonScriptFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual UObject* FactoryCreateNew(UClass * Class, UObject *InParent, FName Name, EObjectFlags Flags, UObject *Context, FFeedbackContext *Warn) override;
};

