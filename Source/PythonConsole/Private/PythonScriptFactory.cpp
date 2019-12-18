
#include "PythonScriptFactory.h"
#include "PythonScript.h"

UPythonScriptFactory::UPythonScriptFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	Formats.Add(FString("py;Python Script"));

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