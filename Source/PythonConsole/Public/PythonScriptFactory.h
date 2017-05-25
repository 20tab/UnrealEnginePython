#pragma once

#include "UnrealEd.h"
#include "PythonScriptFactory.generated.h"

UCLASS()
class UPythonScriptFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual UObject* FactoryCreateFile(UClass * Class, UObject *InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext *Warn, bool& bOutOperationCanceled) override;
};


