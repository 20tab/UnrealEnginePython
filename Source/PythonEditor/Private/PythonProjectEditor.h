// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editor/Kismet/Public/WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "Editor/Kismet/Public/WorkflowOrientedApp/WorkflowTabManager.h"
#include "Runtime/Launch/Resources/Version.h"

class FPythonProjectEditor : public FWorkflowCentricApplication, public FGCObject
{
public:
	FPythonProjectEditor();

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	// End of IToolkit interface

	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	// End of FAssetEditorToolkit

	// FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FSerializableObject interface

	static TSharedPtr<FPythonProjectEditor> Get()
	{
		return PythonEditor.Pin();
	}

public:
	/** Initialize the Python editor */
	void InitPythonEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UPythonProject* PythonProject);

	/** Try to open a new file for editing */
	void OpenFileForEditing(class UPythonProjectItem* Item);
	void CloseFileForEditing(class UPythonProjectItem* Item);

	/** Get the current project being edited by this Python editor */
	UPythonProject* GetPythonProjectBeingEdited() const { return PythonProjectBeingEdited.Get(); }

	TSharedRef<SWidget> CreatePythonEditorWidget(TSharedRef<FTabInfo> TabInfo, UPythonProjectItem* Item);

	void RegisterToolbarTab(const TSharedRef<class FTabManager>& TabManager);

	/** Access the toolbar builder for this editor */
	TSharedPtr<class FPythonProjectEditorToolbar> GetToolbarBuilder() { return ToolbarBuilder; }
	bool New();

	bool NewDirectory();

	bool Delete();

	bool Save();

	bool SaveAll();

	bool Execute();

#if PLATFORM_MAC
	bool ExecuteInMainThread();
#endif

	bool PEP8ize();

	FString GetSafeName(bool IsDirectory);

private:
	void BindCommands();

	void New_Internal();

	void NewDirectory_Internal();

	void Delete_Internal();

	void Save_Internal();

	void SaveAll_Internal();

	void Execute_Internal();

#if PLATFORM_MAC
	void ExecuteInMainThread_Internal();
#endif

	void PEP8ize_Internal();

	bool CanNew() const;

	bool CanSave() const;

	bool CanSaveAll() const;

	bool CanExecute() const;

	bool CanDelete() const;

protected:
	TSharedPtr<FDocumentTracker> DocumentManager;

	/** The Python project we are currently editing */
	TWeakObjectPtr<UPythonProject> PythonProjectBeingEdited;

	TSharedPtr<class FPythonProjectEditorToolbar> ToolbarBuilder;

	static TWeakPtr<FPythonProjectEditor> PythonEditor;

	virtual bool CanSaveAsset() const override { return false; }
#if ENGINE_MINOR_VERSION > 17
	virtual bool CanFindInContentBrowser() const override { return false; }
#endif
};
