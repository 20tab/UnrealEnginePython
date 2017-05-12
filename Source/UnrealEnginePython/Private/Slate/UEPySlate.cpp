
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

static PyObject *ue_PySWidget_str(ue_PySWidget *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SWidget '%s'>",
		TCHAR_TO_UTF8(*self->s_widget_ref->Get().GetTypeAsString()));
}

static PyObject *py_slate_set_content(ue_PySWidget *self, PyObject * args) {
	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:set_content", &py_obj)) {
		return NULL;
	}

	ue_PySWidget *s_widget = (ue_PySWidget *)py_obj;

	if (self->s_widget_type & PY_SLATE_BORDER) {
		SBorder *s_border = (SBorder *)&self->s_widget_ref->Get();
		s_border->SetContent(*s_widget->s_widget_ref);
	}
	else if (self->s_widget_type & PY_SLATE_BOX) {
		SBox *s_box = (SBox *)&self->s_widget_ref->Get();
		s_box->SetContent(*s_widget->s_widget_ref);
	}
	else if (self->s_widget_type & PY_SLATE_WINDOW) {
		SWindow *s_win = (SWindow *)&self->s_widget_ref->Get();
		s_win->SetContent(*s_widget->s_widget_ref);
	}
	else {
		return PyErr_Format(PyExc_Exception, "slate object does not expose the SetContent() method");
	}

	Py_INCREF(self);
	return (PyObject*)self;
}

static PyObject *py_slate_get_window_handle(ue_PySWidget *self, PyObject * args) {
	
	if (self->s_widget_type & PY_SLATE_WINDOW) {
		SWindow *s_window = (SWindow *)&self->s_widget_ref->Get();
		return PyLong_FromLongLong((long long)s_window->GetNativeWindow()->GetOSWindowHandle());
	}

	if (self->s_widget_type & PY_SLATE_TAB) {
		SDockTab *s_tab = (SDockTab *)&self->s_widget_ref->Get();
		if (s_tab->GetParentWindow().IsValid()) {
			return PyLong_FromLongLong((long long)s_tab->GetParentWindow()->GetNativeWindow()->GetOSWindowHandle());
		}
		return PyErr_Format(PyExc_Exception, "slate tab has no parent window");
	}
	
	return PyErr_Format(PyExc_Exception, "slate object does not expose the GetNativeWindow() method");

	Py_INCREF(self);
	return (PyObject*)self;
}

static PyMethodDef ue_PySWidget_methods[] = {
	{ "set_content", (PyCFunction)py_slate_set_content, METH_VARARGS, "" },
	{ "get_window_handle", (PyCFunction)py_slate_get_window_handle, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PySWidgetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SWidget", /* tp_name */
	sizeof(ue_PySWidget), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	(reprfunc)ue_PySWidget_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine SWidget",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySWidget_methods,             /* tp_methods */
};

void ue_python_init_swidget(PyObject *ue_module) {
	ue_PySWidgetType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PySWidgetType) < 0)
		return;

	Py_INCREF(&ue_PySWidgetType);
	PyModule_AddObject(ue_module, "SWidget", (PyObject *)&ue_PySWidgetType);
}

TSharedRef<SDockTab> SPythonWidgetSpawner::SpawnTabNomad(const FSpawnTabArgs& SpawnTabArgs) {
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);
}

PyObject *py_unreal_engine_add_nomad_tab(PyObject *self, PyObject *args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:add_nomad_tab", &name)) {
		return NULL;
	}

	SPythonWidgetSpawner *spawner = new SPythonWidgetSpawner;

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(name, FOnSpawnTab::CreateRaw(spawner, &SPythonWidgetSpawner::SpawnTabNomad))
		.SetDisplayName(FText::FromString(UTF8_TO_TCHAR(name)))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	ue_PySWidget *ue_py_s_widget = (ue_PySWidget *)PyObject_New(ue_PySWidget, &ue_PySWidgetType);
	ue_py_s_widget->s_widget_ref = new TSharedRef<SWidget>(&FGlobalTabmanager::Get()->InvokeTab(FName(name)).Get());
	ue_py_s_widget->s_widget_type = PY_SLATE_TAB|PY_SLATE_BORDER;

	Py_INCREF(ue_py_s_widget);
	return (PyObject *)ue_py_s_widget;
}

PyObject *py_unreal_engine_slate_text_block(PyObject *self, PyObject *args) {
	char *text;
	if (!PyArg_ParseTuple(args, "s:slate_text_block", &text)) {
		return NULL;
	}

	ue_PySWidget *ue_py_s_widget = (ue_PySWidget *)PyObject_New(ue_PySWidget, &ue_PySWidgetType);
	ue_py_s_widget->s_widget_ref = new TSharedRef<SWidget>(SNew(STextBlock).Text(FText::FromString(UTF8_TO_TCHAR(text))));

	Py_INCREF(ue_py_s_widget);
	return (PyObject*)ue_py_s_widget;
}

PyObject *py_unreal_engine_slate_box(PyObject *self, PyObject *args) {
	
	ue_PySWidget *ue_py_s_widget = (ue_PySWidget *)PyObject_New(ue_PySWidget, &ue_PySWidgetType);
	ue_py_s_widget->s_widget_ref = new TSharedRef<SWidget>(SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center));
	ue_py_s_widget->s_widget_type = PY_SLATE_BOX;

	Py_INCREF(ue_py_s_widget);
	return (PyObject*)ue_py_s_widget;
}

PyObject *py_unreal_engine_get_editor_window(PyObject *self, PyObject *args) {

	if (!FGlobalTabmanager::Get()->GetRootWindow().IsValid()) {
		return PyErr_Format(PyExc_Exception, "no RootWindow found");
	}

	ue_PySWidget *ue_py_s_widget = (ue_PySWidget *)PyObject_New(ue_PySWidget, &ue_PySWidgetType);
	ue_py_s_widget->s_widget_ref = new TSharedRef<SWidget>(FGlobalTabmanager::Get()->GetRootWindow().Get());
	ue_py_s_widget->s_widget_type = PY_SLATE_WINDOW;

	Py_INCREF(ue_py_s_widget);
	return (PyObject*)ue_py_s_widget;
}

PyObject *py_unreal_engine_slate_window(PyObject *self, PyObject *args) {
	char *text;
	int width;
	int height;
	if (!PyArg_ParseTuple(args, "sii:slate_window", &text, &width, &height)) {
		return NULL;
	}

	ue_PySWidget *ue_py_s_widget = (ue_PySWidget *)PyObject_New(ue_PySWidget, &ue_PySWidgetType);
	ue_py_s_widget->s_widget_ref = new TSharedRef<SWidget>(SNew(SWindow)
		.Title(FText::FromString(UTF8_TO_TCHAR(text)))
		.ClientSize(FVector2D(width, height))
		);
	ue_py_s_widget->s_widget_type = PY_SLATE_WINDOW;
	SWindow *s_win = (SWindow *)&ue_py_s_widget->s_widget_ref->Get();
	TSharedRef<SWindow> *s_window_ref = new TSharedRef<SWindow>(s_win);
	FSlateApplication::Get().AddWindow(*s_window_ref, true);

	Py_INCREF(ue_py_s_widget);
	return (PyObject*)ue_py_s_widget;
}

PyObject *py_unreal_engine_slate_button(PyObject *self, PyObject *args) {
	char *text;
	if (!PyArg_ParseTuple(args, "s:slate_button", &text)) {
		return NULL;
	}

	ue_PySWidget *ue_py_s_widget = (ue_PySWidget *)PyObject_New(ue_PySWidget, &ue_PySWidgetType);
	ue_py_s_widget->s_widget_ref = new TSharedRef<SWidget>(SNew(SButton).Text(FText::FromString(UTF8_TO_TCHAR(text))));

	Py_INCREF(ue_py_s_widget);
	return (PyObject*)ue_py_s_widget;
}
#endif