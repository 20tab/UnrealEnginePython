#pragma once


#include "PythonScript.generated.h"

UCLASS()
class UPythonScript : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Python")
	FString ScriptPath;
};

