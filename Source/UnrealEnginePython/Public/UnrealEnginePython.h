// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif

#include "ModuleManager.h"

#include "Engine.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/SlateCore/Public/Styling/ISlateStyle.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyle.h"

// We need to make sure reference structs do not mistaken for callable
#define PyCalllable_Check_Extended(value) PyCallable_Check(value) && py_ue_is_uscriptstruct(value) == nullptr

#if ENGINE_MINOR_VERSION >= 18
#define FStringAssetReference FSoftObjectPath
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogPython, Log, All);


class UNREALENGINEPYTHON_API FUnrealEnginePythonModule : public IModuleInterface
{
public:

	bool PythonGILAcquire();
	void PythonGILRelease();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RunString(char *);
	void RunStringSandboxed(char *);
	void RunFile(char *);
	void RunFileSandboxed(char *, void(*callback)(void *arg), void *arg);

	void UESetupPythonInterpreter(bool);

	FString ScriptsPath;
	FString ZipPath;
	FString AdditionalModulesPath;

	bool BrutalFinalize;

	// pep8ize a string using various strategy (currently only autopep8 is supported)
	FString Pep8ize(FString Code);

private:
	void *ue_python_gil;
	// used by console
	void *main_dict;
	void *local_dict;
	void *main_module;

	TSharedPtr<FSlateStyleSet> StyleSet;
};



struct FScopePythonGIL
{
	FScopePythonGIL()
	{
#if defined(UEPY_THREADING)
		UnrealEnginePythonModule = FModuleManager::LoadModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
		safeForRelease = UnrealEnginePythonModule.PythonGILAcquire();
#endif
	}

	~FScopePythonGIL()
	{
#if defined(UEPY_THREADING)
		if (safeForRelease)
		{
			UnrealEnginePythonModule.PythonGILRelease();
		}
#endif
	}

	FUnrealEnginePythonModule UnrealEnginePythonModule;
	bool safeForRelease;
};




