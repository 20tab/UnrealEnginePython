#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR
#include "PyFbxFactory.h"

bool UPyFbxFactory::ConfigureProperties() {
	bDetectImportTypeOnImport = true;
	bShowOption = false;	

	return true;
}

#endif