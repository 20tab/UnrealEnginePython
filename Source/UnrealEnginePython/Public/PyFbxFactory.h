#pragma once

#if WITH_EDITOR

#include "UnrealEd.h"

#include "PyFbxFactory.generated.h"



UCLASS(hidecategories=Object)
class UPyFbxFactory : public UFbxFactory
{
	GENERATED_BODY()

	virtual bool ConfigureProperties() override;

};

#endif