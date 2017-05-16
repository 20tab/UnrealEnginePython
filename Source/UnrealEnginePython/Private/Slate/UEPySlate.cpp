#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "LevelEditor.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandList.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"

#include "UEPySlate.h"

FReply UPythonSlateDelegate::OnMouseEvent(const FGeometry &geometry, const FPointerEvent &pointer_event) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"OO", py_ue_new_uscriptstruct(FGeometry::StaticStruct(), (uint8 *)&geometry), py_ue_new_uscriptstruct(FPointerEvent::StaticStruct(), (uint8 *)&pointer_event));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}

	if (ret == Py_False) {
		Py_DECREF(ret);
		return FReply::Unhandled();
	}
	Py_DECREF(ret);
	return FReply::Handled();
}

FReply UPythonSlateDelegate::OnClicked() {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}

	if (ret == Py_False) {
		Py_DECREF(ret);
		return FReply::Unhandled();
	}
	Py_DECREF(ret);
	return FReply::Handled();
}

static std::map<SWidget *, ue_PySWidget *> *py_slate_mapping;

ue_PySWidget *ue_py_get_swidget(TSharedPtr<SWidget> s_widget) {
	ue_PySWidget *ret = nullptr;
	auto it = py_slate_mapping->find(s_widget.Get());
	// not found, it means it is an SWidget not generated from python
	if (it == py_slate_mapping->end()) {
		if (s_widget->GetType() == FName("SWindow")) {
			ret = py_ue_new_swidget<ue_PySWindow>(s_widget.Get(), &ue_PySWindowType);
		}
		else {
			ret = py_ue_new_swidget<ue_PySWidget>(s_widget.Get(), &ue_PySWidgetType);
		}
	}
	else {
		ret = it->second;
	}
	Py_INCREF(ret);
	return ret;
}

void ue_py_register_swidget(SWidget *s_widget, ue_PySWidget *py_s_widget) {
	(*py_slate_mapping)[s_widget] = py_s_widget;
}

void ue_python_init_slate(PyObject *module) {

	py_slate_mapping = new std::map<SWidget *, ue_PySWidget *>();

	ue_python_init_swidget(module);
	ue_python_init_scompound_widget(module);
	ue_python_init_swindow(module);
	ue_python_init_sborder(module);
	ue_python_init_sbutton(module);
	ue_python_init_sbutton(module);
	ue_python_init_sleaf_widget(module);
	ue_python_init_stext_block(module);
	ue_python_init_seditable_text_box(module);
	ue_python_init_smulti_line_editable_text(module);
	ue_python_init_spanel(module);
	ue_python_init_sgrid_panel(module);
	ue_python_init_sbox_panel(module);
	ue_python_init_shorizontal_box(module);
	ue_python_init_sviewport(module);
	ue_python_init_seditor_viewport(module);
	ue_python_init_spython_editor_viewport(module);
	ue_python_init_simage(module);
}

PyObject *py_unreal_engine_get_editor_window(PyObject *self, PyObject *args) {

	if (!FGlobalTabmanager::Get()->GetRootWindow().IsValid()) {
		return PyErr_Format(PyExc_Exception, "no RootWindow found");
	}

	return (PyObject *)ue_py_get_swidget(FGlobalTabmanager::Get()->GetRootWindow());
}

class FPythonSlateCommands : public TCommands<FPythonSlateCommands>
{
public:

	FPythonSlateCommands()
		: TCommands<FPythonSlateCommands>(TEXT("UnrealEnginePython"), NSLOCTEXT("Contexts", "UnrealEnginePython", "UnrealEnginePython"), NAME_None, "EditorStyle")
	{
		UE_LOG(LogPython, Warning, TEXT("BULDING COMMAND"));
		
	}

	void Setup(char *command_name, PyObject *py_object) {
		py_callable = py_object;
		Py_INCREF(py_callable);

		name = FString(command_name);
	}

	// TCommands<> interface
	virtual void RegisterCommands() override {
		UE_LOG(LogPython, Warning, TEXT("REGISTERING COMMAND"));
		commands = MakeShareable(new FUICommandList);

		extender = MakeShareable(new FExtender());

		UI_COMMAND_Function(this, command, nullptr, *name, *name, TCHAR_TO_UTF8(*name), *name, *name, EUserInterfaceActionType::Button, FInputGesture());
		commands->MapAction(command, FExecuteAction::CreateRaw(this, &FPythonSlateCommands::Callback), FCanExecuteAction());

		extender->AddMenuExtension("WindowLayout", EExtensionHook::After, commands, FMenuExtensionDelegate::CreateRaw(this, &FPythonSlateCommands::Builder));

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(extender);
	}

	void Callback() {
		FScopePythonGIL gil;
		PyObject *ret = PyObject_CallObject(py_callable, nullptr);
		if (!ret) {
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	void Builder(FMenuBuilder &Builder) {
		UE_LOG(LogPython, Warning, TEXT("BULDING MENU %s"), *command->GetCommandName().ToString());
		Builder.AddMenuEntry(command);
	}

	TSharedPtr<FExtender> GetExtender() {
		return extender;
	}

	TSharedPtr<FUICommandList> GetCommands() {
		return commands;
	}
private:
	TSharedPtr<FUICommandList> commands;
	TSharedPtr<FUICommandInfo> command;
	TSharedPtr<FExtender> extender;
	PyObject *py_callable;

	FString name;
};

PyObject *py_unreal_engine_add_menu_extension(PyObject * self, PyObject * args) {

	char *command_name;
	PyObject *py_callable;
	int interface_type = EUserInterfaceActionType::Button;

	char *menu_bar = nullptr;

	if (!PyArg_ParseTuple(args, "sO|s:add_menu_extension", &command_name, &py_callable, &menu_bar)) {
		return NULL;
	}

	UE_LOG(LogPython, Warning, TEXT("STARTIIIING !!!"));

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	

	TSharedRef<FPythonSlateCommands> commands = MakeShareable(new FPythonSlateCommands());

	commands->Setup(command_name, py_callable);

	commands->RegisterCommands();

	UE_LOG(LogPython, Warning, TEXT("EXTENSION ADDED"));

	Py_INCREF(Py_None);
	return Py_None;
}

#endif