
#include "PyFbxFactory.h"
#include "FbxMeshUtils.h"
#include "Runtime/Launch/Resources/Version.h"

UPyFbxFactory::UPyFbxFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// disable automatic detection of the factory
	ImportPriority = -1;

}

bool UPyFbxFactory::ConfigureProperties() {
	bDetectImportTypeOnImport = false;
	bShowOption = false;

	return true;
}

void UPyFbxFactory::PostInitProperties() {

	Super::PostInitProperties();
	ImportUI->MeshTypeToImport = FBXIT_MAX;
}

UObject * UPyFbxFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
#if ENGINE_MINOR_VERSION >= 20
	if (ImportUI->MeshTypeToImport == FBXIT_MAX)
	{
		if (!DetectImportType(UFactory::CurrentFilename))
		{
			return nullptr;
		}
	}
	FbxMeshUtils::SetImportOption(ImportUI);
	// ensure auto-detect is skipped
	bDetectImportTypeOnImport = false;
#endif
	return Super::FactoryCreateFile(InClass, InParent, InName, Flags, Filename, Parms, Warn, bOutOperationCanceled);
}

UObject *UPyFbxFactory::FactoryCreateBinary
(
	UClass * InClass,
	UObject * InParent,
	FName InName,
	EObjectFlags Flags,
	UObject * Context,
	const TCHAR * Type,
	const uint8 *& Buffer,
	const uint8 * BufferEnd,
	FFeedbackContext * Warn,
	bool & bOutOperationCanceled) {

	if (ImportUI->MeshTypeToImport == FBXIT_MAX) {
		if (!DetectImportType(UFactory::CurrentFilename)) {
			return nullptr;
		}
	}

	FbxMeshUtils::SetImportOption(ImportUI);

	// ensure auto-detect is skipped
	bDetectImportTypeOnImport = false;

	return Super::FactoryCreateBinary(InClass, InParent, InName, Flags, Context, Type, Buffer, BufferEnd, Warn, bOutOperationCanceled);
}
