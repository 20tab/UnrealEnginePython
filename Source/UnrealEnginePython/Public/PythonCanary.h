#pragma once

#include "UnrealEnginePython.h"
#include "UObject/Object.h"
#include "PythonCanary.generated.h"

UCLASS(MinimalAPI)
class UPythonCanary : public UObject
{
	GENERATED_BODY()

public:
	UPythonCanary()
	{
		UE_LOG(LogPython, Error, TEXT("Canary Spawned"));
	}

	~UPythonCanary()
	{
		UE_LOG(LogPython, Error, TEXT("Canary IS DEAD !!!"));
		
		NewObject<UPythonCanary>();
	}
};

