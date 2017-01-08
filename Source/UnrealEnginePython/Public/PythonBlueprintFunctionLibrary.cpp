#include "UnrealEnginePythonPrivatePCH.h"

#include "PythonBlueprintFunctionLibrary.h"

void UPythonBlueprintFunctionLibrary::ExecutePythonScript(FString script) {
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.RunFile(TCHAR_TO_UTF8(*script));
}
