#include "UnrealEnginePythonPrivatePCH.h"

#include "PythonScriptFactory.h"

UPythonScriptFactory::UPythonScriptFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UPythonScript::StaticClass();
}

UObject* UPythonScriptFactory::FactoryCreateNew(UClass * Class, UObject *InParent, FName Name, EObjectFlags Flags, UObject *Context, FFeedbackContext *Warn) {
	UPythonScript *NewAsset = NewObject<UPythonScript>(InParent, Class, Name, Flags);

	return NewAsset;
}
