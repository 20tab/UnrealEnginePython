#pragma once

#include "UnrealEd.h"

#include "PyFbxFactory.generated.h"



UCLASS(hidecategories=Object)
class UPyFbxFactory : public UFbxFactory
{
	GENERATED_BODY()

	UPyFbxFactory(const FObjectInitializer& ObjectInitializer);

	virtual bool ConfigureProperties() override;

};

