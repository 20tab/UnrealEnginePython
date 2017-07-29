#pragma once

#include "UnrealEd.h"
#include "PyFactory.generated.h"

UCLASS()
class UPyFactory : public UFactory
{
	GENERATED_BODY()

public:

	virtual UObject* FactoryCreateFile(UClass * Class, UObject *InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext *Warn, bool& bOutOperationCanceled) override {
		return PyFactoryCreateFile(Class, InParent, InName.ToString(), Filename);
	}

	virtual UClass* ResolveSupportedClass() override {
		if (SupportedClass)
			return SupportedClass;
		return PyResolveSupportedClass();
	}

	UFUNCTION(BlueprintImplementableEvent)
	UObject* PyFactoryCreateFile(class UClass * Class, UObject *InParent, const FString & InName, const FString & Filename);

	UFUNCTION(BlueprintImplementableEvent)
	UClass* PyResolveSupportedClass();

};


