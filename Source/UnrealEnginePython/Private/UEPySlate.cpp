
#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "UEPySlate.h"
#include "LevelEditor.h"

// ugly global map to hold the list/mappings of new commands
// we cannot rely on FPythonSlateCommands as it is initialiazed too late for us
std::map<FString, std::list<FPythonSlateCommand *> *> *python_commands;

void FPythonSlateCommands::RegisterCommands()
{
	if (!python_commands)
		return;

	for (auto cmd_map : *python_commands) {
		for (FPythonSlateCommand *cmd : *cmd_map.second) {
			if (!cmd->py_callable)
				continue;
			UI_COMMAND_Function(this,
				cmd->PythonSlateAction,
				nullptr,
				*cmd->command_name,
				*cmd->command_name,
				TCHAR_TO_UTF8(*cmd->command_name),
				*cmd->command_name,
				*cmd->command_name,
				EUserInterfaceActionType::Button, FInputGesture());

			UE_LOG(LogPython, Log, TEXT("Registered python extender command \"%s\""), *cmd->command_name);
		}
	}
}

void FPythonSlateCommand::Builder(FMenuBuilder& Builder) {
	Builder.AddMenuEntry(PythonSlateAction);
}


void FPythonSlateCommand::Callback() {
	FScopePythonGIL gil;
	PyObject *ret = PyObject_CallObject(py_callable, NULL);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateMenuBar::Filler(FMenuBuilder& Builder) {
	for (FPythonSlateCommand *cmd : *commands) {
		Builder.AddMenuEntry(cmd->PythonSlateAction);
	}
}

void FPythonSlateMenuBar::Builder(FMenuBarBuilder& MenuBarBuilder) {
	MenuBarBuilder.AddPullDownMenu(FText::FromString(name),
		FText::FromString(name),
		FNewMenuDelegate::CreateRaw(this, &FPythonSlateMenuBar::Filler),
		FName(*name)
		);
}

void FPythonSlateCommands::ApplyPythonExtenders() {

	if (!python_commands)
		return;

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<class FUICommandList> PythonSlateCommands = MakeShareable(new FUICommandList);

	for (auto cmd_map : *python_commands) {
		if (cmd_map.first.Compare(FString("")) == 0) {
			for (FPythonSlateCommand *cmd : *cmd_map.second) {
				

				PythonSlateCommands->MapAction(
					cmd->PythonSlateAction,
					FExecuteAction::CreateRaw(cmd, &FPythonSlateCommand::Callback),
					FCanExecuteAction());

				TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
				MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PythonSlateCommands,
					FMenuExtensionDelegate::CreateRaw(cmd, &FPythonSlateCommand::Builder));
				LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
			}
		}
		// create a menubar
		else {
			FPythonSlateMenuBar *menu_bar = new FPythonSlateMenuBar;
			menu_bar->name = cmd_map.first;
			menu_bar->commands = cmd_map.second;

			for (FPythonSlateCommand *cmd : *cmd_map.second) {
				PythonSlateCommands->MapAction(
					cmd->PythonSlateAction,
					FExecuteAction::CreateRaw(cmd, &FPythonSlateCommand::Callback),
					FCanExecuteAction());
			}
			
			TSharedPtr<FExtender> MenuBarExtender = MakeShareable(new FExtender());
			MenuBarExtender->AddMenuBarExtension("Help", EExtensionHook::After, PythonSlateCommands,
				FMenuBarExtensionDelegate::CreateRaw(menu_bar, &FPythonSlateMenuBar::Builder));
			LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuBarExtender);
		}
	}
}

PyObject *py_unreal_engine_add_menu_extension(PyObject * self, PyObject * args) {

	char *name;
	char *menu_bar = (char*)"";
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "sO|s:add_menu_extension", &name, &py_obj, &menu_bar)) {
		return NULL;
	}

	if (!PyCallable_Check(py_obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	Py_INCREF(py_obj);

	FPythonSlateCommand *cmd = new FPythonSlateCommand;

	cmd->PythonSlateAction = nullptr;
	cmd->command_name = FString(UTF8_TO_TCHAR(name));
	cmd->py_callable = py_obj;
	cmd->menu_bar = FString(UTF8_TO_TCHAR(menu_bar));

	if (!python_commands)
		python_commands = new std::map<FString, std::list<FPythonSlateCommand *> *>;
	// add new bar item if it does not exist
	if (python_commands->find(cmd->menu_bar) == python_commands->end()) {
		auto *commands_list = new std::list<FPythonSlateCommand *>;
		python_commands->insert({ cmd->menu_bar, commands_list });
	}
	python_commands->at(cmd->menu_bar)->push_back(cmd);

	Py_INCREF(Py_None);
	return Py_None;
}

#endif