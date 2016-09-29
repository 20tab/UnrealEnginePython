#pragma once

#include "UnrealEnginePython.h"


#if WITH_EDITOR
#include "SlateBasics.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"
#include <list>


struct FPythonSlateCommand {
	TSharedPtr<class FUICommandInfo> PythonSlateAction;
	FString command_name;
	PyObject *py_callable;
	void Builder(FMenuBuilder& Builder);
	void Callback();
};

class FPythonSlateCommands : public TCommands<FPythonSlateCommands>
{
public:

	FPythonSlateCommands()
		: TCommands<FPythonSlateCommands>(TEXT("UnrealEnginePython"), NSLOCTEXT("Contexts" ,"UnrealEnginePython", "UnrealEnginePython"), NAME_None, "EditorStyle")
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	static void ApplyPythonExtenders();
};

PyObject *py_unreal_engine_add_menu_extension(PyObject *, PyObject *);
#endif