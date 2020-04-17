// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorToolkit.h"
#include "LevelEditor.h"
#include "PythonEditorStyle.h"
#include "PythonProjectEditor.h"
#include "PythonProject.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

static const FName PythonEditorTabName( TEXT( "PythonEditor" ) );

#define LOCTEXT_NAMESPACE "PythonEditor"

class FPythonEditor : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		FPythonEditorStyle::Initialize();

		struct Local
		{
			static TSharedRef<SDockTab> SpawnPythonEditorTab(const FSpawnTabArgs& TabArgs)
			{
				TSharedRef<FPythonProjectEditor> NewPythonProjectEditor(new FPythonProjectEditor());
				NewPythonProjectEditor->InitPythonEditor(EToolkitMode::Standalone, TSharedPtr<class IToolkitHost>(), GetMutableDefault<UPythonProject>());

				return FGlobalTabmanager::Get()->GetMajorTabForTabManager(NewPythonProjectEditor->GetTabManager().ToSharedRef()).ToSharedRef();
			}

			static void OpenPythonEditor()
			{
				SpawnPythonEditorTab(FSpawnTabArgs(TSharedPtr<SWindow>(), FTabId()));	
			}

			static void ExtendMenu(class FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.AddMenuEntry
				(
					LOCTEXT( "PythonEditorTabTitle", "Python Editor" ),
					LOCTEXT( "PythonEditorTooltipText", "Open the Python Editor tab." ),
					FSlateIcon(FPythonEditorStyle::Get().GetStyleSetName(), "PythonEditor.TabIcon"),
					FUIAction
					(
						FExecuteAction::CreateStatic(&Local::OpenPythonEditor)
					)
				);
			}
		};

		Extender = MakeShareable(new FExtender());
	
		// Add Python editor extension to main menu
		Extender->AddMenuExtension(
			"WindowLayout",
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateStatic( &Local::ExtendMenu ) );

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender( Extender );

		// Register a tab spawner so that our tab can be automatically restored from layout files
		FGlobalTabmanager::Get()->RegisterTabSpawner( PythonEditorTabName, FOnSpawnTab::CreateStatic( &Local::SpawnPythonEditorTab ) )
				.SetDisplayName( LOCTEXT( "PythonEditorTabTitle", "Python Editor" ) )
				.SetTooltipText( LOCTEXT( "PythonEditorTooltipText", "Open the Python Editor tab." ) )
				.SetIcon(FSlateIcon(FPythonEditorStyle::Get().GetStyleSetName(), "PythonEditor.TabIcon"));
	}

	virtual void ShutdownModule() override
	{
		// Unregister the tab spawner
		FGlobalTabmanager::Get()->UnregisterTabSpawner( PythonEditorTabName );

		if(FModuleManager::Get().IsModuleLoaded("LevelEditor"))
		{
			FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
			LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender( Extender );		
		}

		FPythonEditorStyle::Shutdown();
	}

private:
	TSharedPtr<FExtender> Extender; 
};

IMPLEMENT_MODULE( FPythonEditor, PythonEditor )

#undef LOCTEXT_NAMESPACE