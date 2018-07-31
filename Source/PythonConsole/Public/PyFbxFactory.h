#pragma once

#include "UnrealEd.h"

#include "PyFbxFactory.generated.h"



UCLASS(hidecategories = Object)
class UPyFbxFactory : public UFbxFactory
{
	GENERATED_BODY()

	UPyFbxFactory(const FObjectInitializer& ObjectInitializer);

	virtual bool ConfigureProperties() override;
	virtual void PostInitProperties() override;
	virtual UObject * FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	virtual UObject * FactoryCreateBinary
		(
			UClass * InClass,
			UObject * InParent,
			FName InName,
			EObjectFlags Flags,
			UObject * Context,
			const TCHAR * Type,
			const uint8 *& Buffer,
			const uint8 * BufferEnd,
			FFeedbackContext * Warn,
			bool & bOutOperationCanceled) override;

};

