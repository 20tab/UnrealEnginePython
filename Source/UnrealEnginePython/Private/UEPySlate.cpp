
#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "UEPySlate.h"
#include "LevelEditor.h"

// ugly global list to hold the list of new commands
// we cannot rely on FPythonSlateCommands as it is initialiazed too late for us
std::list<FPythonSlateCommand *> *python_commands;

void FPythonSlateCommands::RegisterCommands()
{
	for (FPythonSlateCommand *cmd : *python_commands) {

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

void FPythonSlateCommands::ApplyPythonExtenders() {

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	for (FPythonSlateCommand *cmd : *python_commands) {
		TSharedPtr<class FUICommandList> PythonSlateCommands = MakeShareable(new FUICommandList);

		PythonSlateCommands->MapAction(
			cmd->PythonSlateAction,
			//FExecuteAction::CreateRaw(&FPythonSlateCommands::Get(), &::FPythonSlateCommands::PythonButtonClicked),
			FExecuteAction::CreateRaw(cmd, &FPythonSlateCommand::Callback),
			FCanExecuteAction());

		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PythonSlateCommands,
			FMenuExtensionDelegate::CreateRaw(cmd, &FPythonSlateCommand::Builder));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
}

PyObject *py_unreal_engine_add_menu_extension(PyObject * self, PyObject * args) {

	char *name;
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "sO:add_menu_extension", &name, &py_obj)) {
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

	if (!python_commands)
		python_commands = new std::list<FPythonSlateCommand *>;

	python_commands->push_back(cmd);

	Py_INCREF(Py_None);
	return Py_None;
}

#endif