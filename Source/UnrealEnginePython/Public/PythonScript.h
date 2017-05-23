#pragma once


#include "PythonScript.generated.h"

UCLASS()
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

	//void Run();
	//void RunSandboxed();

};

