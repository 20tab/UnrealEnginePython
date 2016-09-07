#include "PythonConsolePrivatePCH.h"

#include "PyFbxFactory.h"

UPyFbxFactory::UPyFbxFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// disable automatic detection of the factory
	ImportPriority = -1;
}

bool UPyFbxFactory::ConfigureProperties() {
	bDetectImportTypeOnImport = true;
	bShowOption = false;	

	return true;
}
