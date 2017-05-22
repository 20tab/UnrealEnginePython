#pragma once


#include "PythonScript.generated.h"

UCLASS()
class UPythonScript : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Python")
	FString PyScript;

	UPROPERTY(EditAnywhere, meta=(MultiLine), Category = "Python")
	FString PyCode;
};

