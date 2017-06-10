#pragma once


#include "Kismet/BlueprintFunctionLibrary.h"
#include "PythonBlueprintFunctionLibrary.generated.h"


UCLASS()
class UNREALENGINEPYTHON_API UPythonBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Python")
	static void ExecutePythonScript(FString script);
};

