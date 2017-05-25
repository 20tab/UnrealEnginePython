#include "UnrealEnginePythonPrivatePCH.h"

#include "PythonScript.h"

void UPythonScript::Run() {
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.RunFile(TCHAR_TO_UTF8(*ScriptPath));
}

void UPythonScript::RunSandboxed() {
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.RunFileSandboxed(TCHAR_TO_UTF8(*ScriptPath));
}