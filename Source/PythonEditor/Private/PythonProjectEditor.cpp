// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonProjectEditor.h"
#include "SPythonEditor.h"
#include "SPythonProjectEditor.h"
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"
#include "PythonProjectEditorToolbar.h"
#include "Editor/Kismet/Public/WorkflowOrientedApp/WorkflowUObjectDocuments.h"
#include "Editor/Kismet/Public/WorkflowOrientedApp/ApplicationMode.h"
#include "PythonProjectItem.h"
#include "PythonEditorStyle.h"
#include "PythonProject.h"
#include "PythonProjectEditorCommands.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/MessageDialog.h"
#define LOCTEXT_NAMESPACE "PythonEditor"

TWeakPtr<FPythonProjectEditor> FPythonProjectEditor::PythonEditor;

const FName PythonEditorAppName = FName(TEXT("PythonEditorApp"));
TSharedPtr<SPythonProjectEditor> MyPythonProjectEditor;

namespace PythonEditorModes
{
	// Mode identifiers
	static const FName StandardMode(TEXT("StandardMode"));
};

namespace PythonEditorTabs
{
	// Tab identifiers
	static const FName ProjectViewID(TEXT("ProjectView"));
	static const FName PythonViewID(TEXT("Document"));
};

struct FPythonTabSummoner : public FDocumentTabFactoryForObjects<UPythonProjectItem>
{
public:
	DECLARE_DELEGATE_RetVal_TwoParams(TSharedRef<SWidget>, FOnCreatePythonEditorWidget, TSharedRef<FTabInfo>, UPythonProjectItem*);

public:
	FPythonTabSummoner(TSharedPtr<class FPythonProjectEditor> InPythonProjectEditorPtr, FOnCreatePythonEditorWidget CreatePythonEditorWidgetCallback)
		: FDocumentTabFactoryForObjects<UPythonProjectItem>(PythonEditorTabs::PythonViewID, InPythonProjectEditorPtr)
		, PythonProjectEditorPtr(InPythonProjectEditorPtr)
		, OnCreatePythonEditorWidget(CreatePythonEditorWidgetCallback)
	{
	}

	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override
	{
		TSharedRef<SPythonEditor> PythonEditor = StaticCastSharedRef<SPythonEditor>(Tab->GetContent());
		//	InPythonProjectEditorPtr.Pin()->OnPythonEditorFocused(PythonEditor);
	}

	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override
	{
		TSharedRef<SPythonEditor> GraphEditor = StaticCastSharedRef<SPythonEditor>(Tab->GetContent());
		//	GraphEditor->NotifyItemChanged();
	}

	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override
	{
		TSharedRef<SPythonEditor> GraphEditor = StaticCastSharedRef<SPythonEditor>(Tab->GetContent());

		//	UPythonProjectItem* Graph = FTabPayload_UObject::CastChecked<UPythonProjectItem>(Payload);
		//	BlueprintEditorPtr.Pin()->GetBlueprintObj()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
	}

protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UPythonProjectItem* DocumentID) const override
	{
		return FText::FromString(DocumentID->Name);
	}

	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UPythonProjectItem* DocumentID) const override
	{
		check(Info.TabInfo.IsValid());
		return OnCreatePythonEditorWidget.Execute(Info.TabInfo.ToSharedRef(), DocumentID);
	}

	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UPythonProjectItem* DocumentID) const override
	{
		return FPythonEditorStyle::Get().GetBrush("ProjectEditor.Icon.File");
	}

	/*	virtual TSharedRef<FGenericTabHistory> CreateTabHistoryNode(TSharedPtr<FTabPayload> Payload) override
		{
			return MakeShareable(new FSourceTabHistory(SharedThis(this), Payload));
		}*/

protected:
	TWeakPtr<class FPythonProjectEditor> PythonProjectEditorPtr;
	FOnCreatePythonEditorWidget OnCreatePythonEditorWidget;
};


struct FProjectViewSummoner : public FWorkflowTabFactory
{
public:
	FProjectViewSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
		: FWorkflowTabFactory(PythonEditorTabs::ProjectViewID, InHostingApp)
	{
		TabLabel = LOCTEXT("ProjectTabLabel", "Project");

		bIsSingleton = true;

		ViewMenuDescription = LOCTEXT("ProjectTabMenu_Description", "Project");
		ViewMenuTooltip = LOCTEXT("ProjectTabMenu_ToolTip", "Shows the project panel");
	}

	~FProjectViewSummoner()
	{
		MyPythonProjectEditor.Reset();
	}

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override
	{
		TSharedPtr<FPythonProjectEditor> PythonEditorPtr = StaticCastSharedPtr<FPythonProjectEditor>(HostingApp.Pin());
		return SAssignNew(MyPythonProjectEditor, SPythonProjectEditor, PythonEditorPtr->GetPythonProjectBeingEdited());
	}
};


class FBasicPythonEditorMode : public FApplicationMode
{
public:
	FBasicPythonEditorMode(TSharedPtr<class FPythonProjectEditor> InPythonEditor, FName InModeName);

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	// End of FApplicationMode interface

protected:
	TWeakPtr<FPythonProjectEditor> MyPythonEditor;
	FWorkflowAllowedTabSet ProjectViewTabFactories;

};

FBasicPythonEditorMode::FBasicPythonEditorMode(TSharedPtr<class FPythonProjectEditor> InPythonEditor, FName InModeName)
	: FApplicationMode(InModeName)
{
	MyPythonEditor = InPythonEditor;

	ProjectViewTabFactories.RegisterFactory(MakeShareable(new FProjectViewSummoner(InPythonEditor)));

	TabLayout = FTabManager::NewLayout("Standalone_PythonEditor_Layout_v1.01")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(InPythonEditor->GetToolbarTabId(), ETabState::OpenedTab)
				)
			->Split
			(
				FTabManager::NewSplitter()
				->SetSizeCoefficient(0.9f)
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2)
					->SetHideTabWell(true)
					->AddTab(PythonEditorTabs::ProjectViewID, ETabState::OpenedTab)
					)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8)
					->SetHideTabWell(false)
					->AddTab(PythonEditorTabs::PythonViewID, ETabState::ClosedTab)

					)
				)
			);

	InPythonEditor->GetToolbarBuilder()->AddEditorToolbar(ToolbarExtender);
}

void FBasicPythonEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FPythonProjectEditor> Editor = MyPythonEditor.Pin();

	Editor->RegisterToolbarTab(InTabManager.ToSharedRef());
	Editor->PushTabFactories(ProjectViewTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

FPythonProjectEditor::FPythonProjectEditor()
{
}

void FPythonProjectEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& NewTabManager)
{
	DocumentManager->SetTabManager(NewTabManager);

	FWorkflowCentricApplication::RegisterTabSpawners(NewTabManager);
}

void FPythonProjectEditor::RegisterToolbarTab(const TSharedRef<class FTabManager>& NewTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(NewTabManager);
}

void FPythonProjectEditor::InitPythonEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UPythonProject* PythonProject)
{
	FAssetEditorManager::Get().CloseOtherEditors(PythonProject, this);
	PythonProjectBeingEdited = PythonProject;

	TSharedPtr<FPythonProjectEditor> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);
	}

	PythonEditor = ThisPtr;

	TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FPythonTabSummoner(ThisPtr, FPythonTabSummoner::FOnCreatePythonEditorWidget::CreateSP(this, &FPythonProjectEditor::CreatePythonEditorWidget)));
	DocumentManager->RegisterDocumentFactory(GraphEditorFactory);

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FPythonProjectEditorToolbar(SharedThis(this)));
	}

	FPythonProjectEditorCommands::Register();

	// Initialize the asset editor and spawn nothing (dummy layout)
	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
	InitAssetEditor(Mode, InitToolkitHost, PythonEditorAppName, DummyLayout, /*bCreateDefaultStandaloneMenu=*/ true, /*bCreateDefaultToolbar=*/ true, PythonProject);

	BindCommands();

	// Create the modes and activate one (which will populate with a real layout)
	AddApplicationMode(
		PythonEditorModes::StandardMode,
		MakeShareable(new FBasicPythonEditorMode(ThisPtr, PythonEditorModes::StandardMode)));
	SetCurrentMode(PythonEditorModes::StandardMode);

	RegenerateMenusAndToolbars();
}

void FPythonProjectEditor::BindCommands()
{
	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().New,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::New_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanNew)
		);
	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().NewDirectory,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::NewDirectory_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanNew)
		);
	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().Delete,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::Delete_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanDelete)
		);
	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().Save,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::Save_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanSave)
		);

	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().SaveAll,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::SaveAll_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanSaveAll)
		);
	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().Execute,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::Execute_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanExecute)
		);
#if PLATFORM_MAC
	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().ExecuteInMainThread,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::ExecuteInMainThread_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanExecute)
		);
#endif

	ToolkitCommands->MapAction(FPythonProjectEditorCommands::Get().PEP8ize,
		FExecuteAction::CreateSP(this, &FPythonProjectEditor::PEP8ize_Internal),
		FCanExecuteAction::CreateSP(this, &FPythonProjectEditor::CanExecute)
	);

}

void FPythonProjectEditor::CloseFileForEditing(UPythonProjectItem* Item)
{
	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(Item);
	DocumentManager->CloseTab(Payload);
}

void FPythonProjectEditor::OpenFileForEditing(UPythonProjectItem* Item)
{
	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(Item);
	DocumentManager->OpenDocument(Payload, FDocumentTracker::OpenNewDocument);
}

FName FPythonProjectEditor::GetToolkitFName() const
{
	return FName("PythonEditor");
}

FText FPythonProjectEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Python Editor");
}

FText FPythonProjectEditor::GetToolkitName() const
{
	return LOCTEXT("PythonAppLabel", "Python Editor");
}

FText FPythonProjectEditor::GetToolkitToolTipText() const
{
	return LOCTEXT("PythonAppLabel", "Python Editor");
}

FString FPythonProjectEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("PythonEditor");
}

FLinearColor FPythonProjectEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

void FPythonProjectEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	UPythonProject* PythonProject = PythonProjectBeingEdited.Get();
	Collector.AddReferencedObject(PythonProject);
}

TSharedRef<SWidget> FPythonProjectEditor::CreatePythonEditorWidget(TSharedRef<FTabInfo> TabInfo, UPythonProjectItem* Item)
{
	return SNew(SPythonEditor, Item);
}

FString FPythonProjectEditor::GetSafeName(bool IsDirectory)
{
	UPythonProject* PythonProject = PythonProjectBeingEdited.Get();

	TArray<UPythonProjectItem*> selItems = MyPythonProjectEditor->GetSelectedItems();
	FString basePath = PythonProject->Path;
	if (selItems.Num() > 0) {
		if (selItems[0]->Type == EPythonProjectItemType::Folder) {
			basePath = selItems[0]->Path;
		}
	}

	int suf = 2;
	FString scriptName;
	while (true)
	{

		FString SufStr = FString::Printf(TEXT("%d"), suf);
		if (!IsDirectory) {
			scriptName = basePath / TEXT("PythonScript") + SufStr + TEXT(".py");
			if (!FPaths::FileExists(scriptName)) {
				return scriptName;
			}
		}
		else {
			scriptName = basePath / TEXT("NewDirectory") + SufStr;
			if (!FPaths::DirectoryExists(scriptName)) {
				return scriptName;
			}
		}

		suf++;
	}
}

void FPythonProjectEditor::New_Internal()
{
	New();
}

bool FPythonProjectEditor::New()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	FString scriptName = GetSafeName(false);
	IFileHandle* fileHandle = PlatformFile.OpenWrite(*scriptName);
	if (fileHandle == NULL) {
		return false;
	}
	fileHandle->~IFileHandle();
	UPythonProject* PythonProject = PythonProjectBeingEdited.Get();
	PythonProject->RescanChildren();
	UPythonProjectItem* item = MyPythonProjectEditor->SelectByPath(scriptName);
	if (item)
		OpenFileForEditing(item);
	return true;
}

void FPythonProjectEditor::NewDirectory_Internal()
{
	NewDirectory();
}

bool FPythonProjectEditor::NewDirectory()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	FString dirName = GetSafeName(true);
	if (!PlatformFile.CreateDirectory(*dirName)) {
		return false;
	}
	UPythonProject* PythonProject = PythonProjectBeingEdited.Get();
	PythonProject->RescanChildren();
	return true;
}

void FPythonProjectEditor::Delete_Internal()
{
	Delete();
}

bool FPythonProjectEditor::Delete()
{
	EAppReturnType::Type ret = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("Are you sure to delete ?")));
	if (EAppReturnType::Yes != ret) {
		return false;
	}
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	TArray<UPythonProjectItem*> selItems = MyPythonProjectEditor->GetSelectedItems();
	for (UPythonProjectItem* item : selItems)
	{
		CloseFileForEditing(item);
		PlatformFile.DeleteFile(*item->Path);
	}
	UPythonProject* PythonProject = PythonProjectBeingEdited.Get();
	PythonProject->RescanChildren();
	MyPythonProjectEditor->RequestTreeRefresh();
	return true;
}
bool FPythonProjectEditor::CanDelete() const
{
	return MyPythonProjectEditor->HasSelectionItem();
}

void FPythonProjectEditor::Save_Internal()
{
	Save();
}

bool FPythonProjectEditor::Save()
{
	if (DocumentManager.IsValid() && DocumentManager->GetActiveTab().IsValid())
	{
		TSharedRef<SPythonEditor> PythonEditorRef = StaticCastSharedRef<SPythonEditor>(DocumentManager->GetActiveTab()->GetContent());
		return PythonEditorRef->Save();
	}

	return false;
}

bool FPythonProjectEditor::CanSave() const
{
	if (DocumentManager.IsValid() && DocumentManager->GetActiveTab().IsValid())
	{
		TSharedRef<SWidget> Content = DocumentManager->GetActiveTab()->GetContent();
		TSharedRef<SPythonEditor> PythonEditorRef = StaticCastSharedRef<SPythonEditor>(Content);
		return PythonEditorRef->CanSave();
	}

	return false;
}

void FPythonProjectEditor::SaveAll_Internal()
{
	SaveAll();
}

bool FPythonProjectEditor::SaveAll()
{
	bool bResult = true;

	if (DocumentManager.IsValid())
	{
		TArray<TSharedPtr<SDockTab>> AllTabs = DocumentManager->GetAllDocumentTabs();
		for (auto& Tab : AllTabs)
		{
			if (Tab.IsValid())
			{
				TSharedRef<SPythonEditor> PythonEditorRef = StaticCastSharedRef<SPythonEditor>(Tab->GetContent());
				if (!PythonEditorRef->Save())
				{
					bResult = false;
				}
			}
		}
	}

	return bResult;
}

void FPythonProjectEditor::Execute_Internal()
{
	Execute();
}

#if PLATFORM_MAC
void FPythonProjectEditor::ExecuteInMainThread_Internal()
{
	ExecuteInMainThread();
}

bool FPythonProjectEditor::ExecuteInMainThread()
{
	if (DocumentManager.IsValid() && DocumentManager->GetActiveTab().IsValid())
	{
		TSharedRef<SPythonEditor> PythonEditorRef = StaticCastSharedRef<SPythonEditor>(DocumentManager->GetActiveTab()->GetContent());
		PythonEditorRef->ExecuteInMainThread();
	}

	return true;
}


#endif

void FPythonProjectEditor::PEP8ize_Internal()
{
	PEP8ize();
}

bool FPythonProjectEditor::Execute()
{
	if (DocumentManager.IsValid() && DocumentManager->GetActiveTab().IsValid())
	{
		TSharedRef<SPythonEditor> PythonEditorRef = StaticCastSharedRef<SPythonEditor>(DocumentManager->GetActiveTab()->GetContent());
		PythonEditorRef->Execute();
	}

	return true;
}

bool FPythonProjectEditor::PEP8ize()
{
	if (DocumentManager.IsValid() && DocumentManager->GetActiveTab().IsValid())
	{
		TSharedRef<SPythonEditor> PythonEditorRef = StaticCastSharedRef<SPythonEditor>(DocumentManager->GetActiveTab()->GetContent());
		PythonEditorRef->PEP8ize();
	}

	return true;
}

bool FPythonProjectEditor::CanSaveAll() const
{
	return true;
}
bool FPythonProjectEditor::CanNew() const
{
	return true;
}
bool FPythonProjectEditor::CanExecute() const
{
	if (DocumentManager.IsValid() && DocumentManager->GetActiveTab().IsValid())
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
