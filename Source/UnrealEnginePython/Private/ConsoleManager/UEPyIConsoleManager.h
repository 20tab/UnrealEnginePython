#pragma once

#include "UEPyModule.h"

#include "Runtime/Core/Public/HAL/IConsoleManager.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyIConsoleManager;

class FPythonSmartConsoleDelegate;
struct FPythonSmartConsoleDelegatePair
{
	IConsoleObject *Key;
	TSharedRef<FPythonSmartConsoleDelegate> Value;

	FPythonSmartConsoleDelegatePair(IConsoleObject *InKey, TSharedRef<FPythonSmartConsoleDelegate> InValue) : Key(InKey), Value(InValue)
	{

	}
};

class FPythonSmartConsoleDelegate : public FPythonSmartDelegate
{

public:
	void OnConsoleCommand(const TArray < FString > &InArgs);

	static void RegisterPyDelegate(IConsoleObject *InKey, TSharedRef<FPythonSmartConsoleDelegate> InValue)
	{
		FPythonSmartConsoleDelegatePair Pair(InKey, InValue);
		PyDelegatesMapping.Add(Pair);
	}

	static void UnregisterPyDelegate(IConsoleObject *Key)
	{
		int32 Index = -1;
		for (int32 i = 0; i < PyDelegatesMapping.Num(); i++)
		{
			if (PyDelegatesMapping[i].Key == Key)
			{
				Index = i;
				break;
			}
		}

		if (Index >= 0)
		{
			PyDelegatesMapping.RemoveAt(Index);
		}
	}

private:
	static TArray<FPythonSmartConsoleDelegatePair> PyDelegatesMapping;
};

void ue_python_init_iconsole_manager(PyObject *);
