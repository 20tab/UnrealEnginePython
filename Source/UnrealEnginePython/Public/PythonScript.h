#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PythonScript.generated.h"

UCLASS(MinimalAPI)
class UPythonScript : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Python")
	FString ScriptPath;

	UPROPERTY(EditAnywhere, Category = "Python")
	FString FunctionToCall;

	UPROPERTY(EditAnywhere, Category = "Python")
	TArray<FString> FunctionArgs;

	UFUNCTION()
	void Run();

	UFUNCTION()
	void RunSandboxed();

	void CallSpecificFunctionWithArgs();
};

