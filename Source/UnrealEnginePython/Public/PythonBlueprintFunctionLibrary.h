#pragma once


#include "Kismet/BlueprintFunctionLibrary.h"
#include "PythonBlueprintFunctionLibrary.generated.h"


UCLASS()
class UNREALENGINEPYTHON_API UPythonBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Exec, Category = "Python")
	static void ExecutePythonScript(FString script);

    UFUNCTION(BlueprintCallable, Exec, Category = "Python")
    static void ExecutePythonScriptWithArgs(FString script, TArray<FString>& args);

    UFUNCTION(BlueprintCallable, Exec, Category = "Python")
    static void ExecutePythonString(const FString& PythonCmd);
};

