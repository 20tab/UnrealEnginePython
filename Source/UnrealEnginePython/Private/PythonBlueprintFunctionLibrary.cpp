#include "UnrealEnginePythonPrivatePCH.h"

#include "PythonBlueprintFunctionLibrary.h"

void UPythonBlueprintFunctionLibrary::ExecutePythonScript(FString script) {
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.RunFile(TCHAR_TO_UTF8(*script));
}

void UPythonBlueprintFunctionLibrary::ExecutePythonScriptWithArgs(FString script, TArray<FString>& args) {
    FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
    PythonModule.RunFileWithArgs(TCHAR_TO_UTF8(*script), args);
}

void UPythonBlueprintFunctionLibrary::ExecutePythonString(const FString& PythonCmd) {
    FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
    PythonModule.RunString(TCHAR_TO_UTF8(*PythonCmd));
}
