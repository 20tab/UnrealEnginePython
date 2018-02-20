#include "PythonConsolePrivatePCH.h"

#include "PythonScriptFactory.h"
#include "PythonScript.h"

UPythonScriptFactory::UPythonScriptFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

    // @third party code - BEGIN Bebylon - #ThirdParty-Python: WITH_KNL_PYEXT - Workaround for our deployment process
#if !WITH_KNL_PYEXT
	Formats.Add(FString("py;Python Script"));
#endif
    // @third party code - END Bebylon

	bCreateNew = false;
	bEditAfterNew = true;

	bEditorImport = true;

	SupportedClass = UPythonScript::StaticClass();
}

UObject* UPythonScriptFactory::FactoryCreateFile(UClass * Class, UObject *InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext *Warn, bool& bOutOperationCanceled) {
	UPythonScript *NewAsset = NewObject<UPythonScript>(InParent, Class, InName, Flags);

	NewAsset->ScriptPath = Filename;

	bOutOperationCanceled = false;
	return NewAsset;
}