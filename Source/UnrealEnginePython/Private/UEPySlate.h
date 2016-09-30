#pragma once

#include "UnrealEnginePython.h"


#if WITH_EDITOR
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"
#include <map>
#include <list>


struct FPythonSlateCommand {
	TSharedPtr<class FUICommandInfo> PythonSlateAction;
	FString command_name;
	PyObject *py_callable;
	FString menu_bar;
	void Builder(FMenuBuilder& Builder);
	void Callback();
};


struct FPythonSlateMenuBar {
	FString name;
	std::list<FPythonSlateCommand *> *commands;
	void Builder(FMenuBarBuilder& MenuBarBuilder);
	void Filler(FMenuBuilder& Builder);
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



typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	SWidget *s_widget;
} ue_PySWidget;

void ue_python_init_swidget(PyObject *);

struct SPythonWidgetSpawner {
	TSharedRef<SDockTab> SpawnTabNomad(const FSpawnTabArgs& SpawnTabArgs);
};

PyObject *py_unreal_engine_add_nomad_tab(PyObject *, PyObject *);
#endif
