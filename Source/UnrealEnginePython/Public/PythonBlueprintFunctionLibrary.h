#pragma once


#include "PythonBlueprintFunctionLibrary.generated.h"



UCLASS()
class UPythonBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Python")
	static void ExecutePythonScript(FString script);
};

