#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"
#include "Editor/SkeletalMeshEditor/Public/ISkeletalMeshEditor.h"
#include "Editor/StaticMeshEditor/Public/StaticMeshEditorModule.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include <map>
#include <list>

#define PY_SLATE_BOX	0x01
#define PY_SLATE_BORDER	0x02
#define PY_SLATE_WINDOW	0x04
#define PY_SLATE_TAB	0x08
#define PY_SLATE_BUTTON 0x0F


struct FPythonSlateCommand {
	TSharedPtr<class FUICommandInfo> PythonSlateAction;
	FString command_name;
	PyObject *py_callable;
	FString menu_bar;
	int content_browser = 0;
	void Builder(FMenuBuilder& Builder);
	void Callback();
	TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets, TSharedPtr<class FUICommandList> CommandList);
	void GenerateMenu(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets, PyObject *menu_definition);
	void GenerateMenuSection(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets, PyObject *menu_definition);
};


struct FPythonSlateMenuBar {
	FString name;
	std::list<FPythonSlateCommand *> *commands;
	void Builder(FMenuBarBuilder& MenuBarBuilder);
	void Filler(FMenuBuilder& Builder);
};

struct FPythonSlateToolBar {
	enum Editors { SkeletalMeshEditor, StaticMeshEditor, AnimationEditor, BlueprintEditor, MaterialEditor, LevelEditor};
	TSharedPtr<class FUICommandInfo> PythonSlateAction;
	int module;
	FString name;
	FString tooltip;
	PyObject *py_callable;
	FName image_brush;
	std::list<FPythonSlateCommand *> *commands;
	TSharedRef<FExtender> GetSkeletalMeshEditorToolbarExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor);
	void SkeletalMeshEditorToolbarBuilder(FToolBarBuilder& ToolbarBuilder, USkeletalMesh* InSkeletalMesh);

	//TSharedRef<FExtender> GetStaticMeshEditorToolbarExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<IStaticMeshEditor> InStaticMeshEditor);
	void FPythonSlateToolBar::StaticMeshEditorToolbarBuilder(FToolBarBuilder& ToolbarBuilder);
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

PyObject *py_unreal_engine_add_menu_extension(PyObject *, PyObject *, PyObject *);

PyObject *py_unreal_engine_add_toolbar_extension(PyObject *, PyObject *);

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	TSharedRef<SWidget> *s_widget_ref;
	int s_widget_type;
} ue_PySWidget;

void ue_python_init_swidget(PyObject *);

struct SPythonWidgetSpawner {
	TSharedRef<SDockTab> SpawnTabNomad(const FSpawnTabArgs& SpawnTabArgs);
};

PyObject *py_unreal_engine_add_nomad_tab(PyObject *, PyObject *);
PyObject *py_unreal_engine_slate_text_block(PyObject *, PyObject *);
PyObject *py_unreal_engine_slate_box(PyObject *, PyObject *);
PyObject *py_unreal_engine_slate_window(PyObject *, PyObject *);
PyObject *py_unreal_engine_slate_button(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_editor_window(PyObject *, PyObject *);
#endif
