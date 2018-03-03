#include "UnrealEnginePythonPrivatePCH.h"

#include "PythonBlueprintFunctionLibrary.h"

void UPythonBlueprintFunctionLibrary::ExecutePythonScript(FString script) {
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.RunFile(TCHAR_TO_UTF8(*script));
}

void UPythonBlueprintFunctionLibrary::ExecutePythonScriptWithArgs(FString script, FString arg0, FString arg1) {
    FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
    PythonModule.RunFileWithArgs(TCHAR_TO_UTF8(*script), TCHAR_TO_UTF8(*arg0), TCHAR_TO_UTF8(*arg1));
}

void UPythonBlueprintFunctionLibrary::ExecutePythonString(const FString& PythonCmd) {
    FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
    PythonModule.RunString(TCHAR_TO_UTF8(*PythonCmd));
}
