
#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "UEPySlate.h"
#include "LevelEditor.h"

#include "Editor/Persona/Public/IPersonaToolkit.h"
#include "Editor/SkeletalMeshEditor/Public/ISkeletalMeshEditorModule.h"

// ugly global map to hold the list/mappings of new commands
// we cannot rely on FPythonSlateCommands as it is initialiazed too late for us
std::map<FString, std::list<FPythonSlateCommand *> *> *python_commands;
std::list<FPythonSlateToolBar *> *python_toolbar_commands;

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

void FPythonSlateCommand::GenerateMenuSection(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets, PyObject *menu_definition) {
	if (PyDict_Contains(menu_definition, PyUnicode_FromString((char *)"menus"))) {
		PyObject *menus = PyDict_GetItemString(menu_definition, "menus");
		for (int i = 0; i < PyList_Size(menus); i++) {
			PyObject *menu = PyList_GetItem(menus, i);
			PyObject *menu_name = PyDict_GetItemString(menu, (char *)"name");
			PyObject *menu_tooltip = PyDict_GetItemString(menu, (char *)"tooltip");
			PyObject *menu_icon = PyDict_GetItemString(menu, (char *)"icon");
			FSlateIcon slate_icon;
			if (menu_icon)
				slate_icon = FSlateIcon("UnrealEnginePython", PyUnicode_AsUTF8(menu_icon));
			else
				slate_icon = FSlateIcon();

			MenuBuilder.AddSubMenu(
				FText::FromString(PyUnicode_AsUTF8(menu_name)),
				FText::FromString(PyUnicode_AsUTF8(menu_tooltip)),
				FNewMenuDelegate::CreateRaw(this, &FPythonSlateCommand::GenerateMenu, SelectedAssets, menu),
				false,
				slate_icon
			);
		}
	}

	if (PyDict_Contains(menu_definition, PyUnicode_FromString((char *)"actions"))) {
		PyObject *actions = PyDict_GetItemString(menu_definition, (char *)"actions");
		for (int i = 0; i < PyList_Size(actions); i++) {
			PyObject *action = PyList_GetItem(actions, i);
			PyObject *name = PyDict_GetItemString(action, (char *)"name");
			PyObject *tooltip = PyDict_GetItemString(action, (char *)"tooltip");
			PyObject *callback = PyDict_GetItemString(action, (char *)"callback");
			PyObject *menu_icon = PyDict_GetItemString(action, (char *)"icon");
			FSlateIcon slate_icon;
			if (menu_icon)
				slate_icon = FSlateIcon("UnrealEnginePython", PyUnicode_AsUTF8(menu_icon));
			else
				slate_icon = FSlateIcon();

			//add a menu entry that will invoke that uiaction
			MenuBuilder.AddMenuEntry(
				FText::FromString(PyUnicode_AsUTF8(name)),
				FText::FromString(PyUnicode_AsUTF8(tooltip)),
				slate_icon,
				FUIAction(FExecuteAction::CreateLambda([callback, SelectedAssets]()
				{
					FScopePythonGIL gil;
					PyObject *selected_assets = PyList_New(SelectedAssets.Num());
					for (int x = 0; x < SelectedAssets.Num(); x++)
						PyList_SetItem(selected_assets, x, py_ue_new_fassetdata(const_cast<FAssetData *>(&SelectedAssets[x])));

					if (!selected_assets) {
						PyErr_Format(PyExc_Exception, "uobject is in invalid state");
						return;
					}
					Py_INCREF(selected_assets);
					PyObject *ret = PyObject_CallObject(callback, Py_BuildValue("(O)", selected_assets));
					Py_DECREF(selected_assets);
					if (!ret) {
						unreal_engine_py_log_error();
						return;
					}
					Py_DECREF(ret);
				})),
				NAME_None,
			EUserInterfaceActionType::Button);
		}
    }
}

void FPythonSlateCommand::GenerateMenu(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets, PyObject *menu_definition) {
	FScopePythonGIL gil;
	if (PyDict_Contains(menu_definition, PyUnicode_FromString((char *)"section"))) {
		PyObject *section = PyDict_GetItemString(menu_definition, (char *)"section");
		PyObject *name = PyDict_GetItemString(section, (char *)"name");
		PyObject *caption = PyDict_GetItemString(section, (char *)"caption");
		MenuBuilder.BeginSection(PyUnicode_AsUTF8(name), FText::FromString(PyUnicode_AsUTF8(caption)));
		{
			GenerateMenuSection(MenuBuilder, SelectedAssets, menu_definition);
		}
		MenuBuilder.EndSection();
	}
	else
		GenerateMenuSection(MenuBuilder, SelectedAssets, menu_definition);
}

TSharedRef<FExtender> FPythonSlateCommand::OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets, TSharedPtr<class FUICommandList> CommandList) {
	TSharedRef<FExtender> Extender(new FExtender());
	PyObject *py_objs = PyList_New(SelectedAssets.Num());
	for (int i = 0; i < SelectedAssets.Num(); i++)
		PyList_SetItem(py_objs, i, (PyObject *)py_ue_new_fassetdata(const_cast<FAssetData *>(&SelectedAssets[i])));
	
	FScopePythonGIL gil;
	PyObject *ret = PyObject_CallObject(py_callable, Py_BuildValue("(O)", py_objs));
	if (ret) {
		if (PyDict_Check(ret)) {
			FName hook = FName(PyUnicode_AsUTF8(PyDict_GetItemString(ret, (char *)"extension_hook")));
			Extender->AddMenuExtension(
				hook,
				static_cast<EExtensionHook::Position>(PyLong_AsLong(PyDict_GetItemString(ret, (char *)"position"))),
				CommandList,
				FMenuExtensionDelegate::CreateRaw(this, &FPythonSlateCommand::GenerateMenu, SelectedAssets, ret)
			);
		}
		else {
			Py_DECREF(ret);
		}
	}
	else {
		unreal_engine_py_log_error();
	}
		
	
	return Extender;

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

TSharedRef<FExtender> FPythonSlateToolBar::GetSkeletalMeshEditorToolbarExtender(const TSharedRef<FUICommandList> CommandList, TSharedRef<ISkeletalMeshEditor> InSkeletalMeshEditor)
{
	TSharedRef<FExtender> Extender = MakeShareable(new FExtender);

	//MeshComponent* MeshComponent = Cast<UMeshComponent>(InSkeletalMeshEditor->GetPersonaToolkit()->GetPreviewMeshComponent());
	USkeletalMesh *SkeletalMesh = InSkeletalMeshEditor->GetPersonaToolkit()->GetPreviewMesh();

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		CommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FPythonSlateToolBar::SkeletalMeshEditorToolbarBuilder, SkeletalMesh)
	);

	return Extender;
}

void FPythonSlateToolBar::SkeletalMeshEditorToolbarBuilder(FToolBarBuilder& ParentToolbarBuilder, USkeletalMesh* InSkeletalMesh)
{
	FSlateIcon slate_icon;
	if (image_brush.IsValid())
		slate_icon = FSlateIcon("UnrealEnginePython", image_brush);
	else
		slate_icon = FSlateIcon();
	
	//TAttribute<FSlateIcon>::Create(&FSourceControlStatus::GetSourceControlIcon)
	ParentToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateLambda([this, InSkeletalMesh]()
		{
			FScopePythonGIL gil;
			
			ue_PyUObject *mesh = ue_get_python_wrapper((UObject *)InSkeletalMesh);
			if (!mesh) {
				PyErr_Format(PyExc_Exception, "uobject is in invalid state");
				return;
			}
			Py_INCREF(mesh);
			PyObject *ret = PyObject_CallObject(py_callable, Py_BuildValue("(O)", mesh));
			Py_DECREF(mesh);
			if (!ret) {
				unreal_engine_py_log_error();
				return;
			}
			Py_DECREF(ret);
		})),
		NAME_None,
		FText::FromString(name),
		FText::FromString(tooltip),
		slate_icon
		);
}

void FPythonSlateToolBar::StaticMeshEditorToolbarBuilder(FToolBarBuilder& ParentToolbarBuilder)
{
	FSlateIcon slate_icon;
	if (image_brush.IsValid())
		slate_icon = FSlateIcon("UnrealEnginePython", image_brush);
	else
		slate_icon = FSlateIcon();

	//TAttribute<FSlateIcon>::Create(&FSourceControlStatus::GetSourceControlIcon)
	ParentToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateLambda([this]()
		{
			FScopePythonGIL gil;

			/*ue_PyUObject *mesh = ue_get_python_wrapper((UObject *)InStaticMesh);
			if (!mesh) {
				PyErr_Format(PyExc_Exception, "uobject is in invalid state");
				return;
			}
			Py_INCREF(mesh);*/
			//PyObject *ret = PyObject_CallObject(py_callable, Py_BuildValue("(O)", mesh));
			//Py_DECREF(mesh);
			/*if (!ret) {
				unreal_engine_py_log_error();
				return;
			}
			Py_DECREF(ret);*/
		})),
		NAME_None,
		FText::FromString(name),
		FText::FromString(tooltip),
		slate_icon
		);
}

void FPythonSlateCommands::ApplyPythonExtenders() {

	if (python_commands) {

		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		TSharedPtr<class FUICommandList> PythonSlateCommands = MakeShareable(new FUICommandList);
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		TArray<FContentBrowserMenuExtender_SelectedAssets>& ContentBrowserExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();

		for (auto cmd_map : *python_commands) {
			if (cmd_map.first.Compare(FString("")) == 0) {
				for (FPythonSlateCommand *cmd : *cmd_map.second) {
					PythonSlateCommands->MapAction(
						cmd->PythonSlateAction,
						FExecuteAction::CreateRaw(cmd, &FPythonSlateCommand::Callback),
						FCanExecuteAction());
					if (cmd->content_browser) {
						FContentBrowserMenuExtender_SelectedAssets ContentBrowserExtender = FContentBrowserMenuExtender_SelectedAssets::CreateRaw(cmd, &FPythonSlateCommand::OnExtendContentBrowserAssetSelectionMenu, PythonSlateCommands);
						ContentBrowserExtenders.Add(ContentBrowserExtender);
					}
					else {
						TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
						MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PythonSlateCommands,
							FMenuExtensionDelegate::CreateRaw(cmd, &FPythonSlateCommand::Builder));
						LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
					}
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

	if (python_toolbar_commands) {
		for (FPythonSlateToolBar *cmd : *python_toolbar_commands) {
			TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender());

			if (cmd->module == FPythonSlateToolBar::Editors::SkeletalMeshEditor) {
				ISkeletalMeshEditorModule& SkeletalMeshEditorModule = FModuleManager::Get().LoadModuleChecked<ISkeletalMeshEditorModule>("SkeletalMeshEditor");
				auto& ToolbarExtenders = SkeletalMeshEditorModule.GetAllSkeletalMeshEditorToolbarExtenders();
				ToolbarExtenders.Add(ISkeletalMeshEditorModule::FSkeletalMeshEditorToolbarExtender::CreateRaw(cmd, &FPythonSlateToolBar::GetSkeletalMeshEditorToolbarExtender));
			}
			if (cmd->module == FPythonSlateToolBar::Editors::StaticMeshEditor) {
				IStaticMeshEditorModule& StaticMeshEditorModule = FModuleManager::LoadModuleChecked<IStaticMeshEditorModule>("StaticMeshEditor");
				TSharedRef<FExtender> Extender = MakeShareable(new FExtender);
				Extender->AddToolBarExtension(
					"Asset",
					EExtensionHook::After,
					NULL,
					FToolBarExtensionDelegate::CreateRaw(cmd, &FPythonSlateToolBar::StaticMeshEditorToolbarBuilder)
				);
				StaticMeshEditorModule.GetToolBarExtensibilityManager()->AddExtender(Extender);
			}
		}
	}
}

PyObject *py_unreal_engine_add_toolbar_extension(PyObject * self, PyObject * args) {
	
	int module;
	char *name;
	char *tooltip;
	PyObject *py_obj;
	char *image_brush = nullptr;
	if (!PyArg_ParseTuple(args, "issO|s:add_toolbar_extension", &module, &name, &tooltip, &py_obj, &image_brush)) {
		return NULL;
	}

	if (!PyCallable_Check(py_obj)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	Py_INCREF(py_obj);

	FPythonSlateToolBar *cmd = new FPythonSlateToolBar;

	cmd->PythonSlateAction = nullptr;
	cmd->name = FString(UTF8_TO_TCHAR(name));
	cmd->tooltip = FString(UTF8_TO_TCHAR(tooltip));
	cmd->py_callable = py_obj;
	cmd->module = module;
	if (image_brush)
		cmd->image_brush = FName(UTF8_TO_TCHAR(image_brush));

	if (!python_toolbar_commands)
		python_toolbar_commands = new std::list<FPythonSlateToolBar *>;

	python_toolbar_commands->push_back(cmd);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_add_menu_extension(PyObject * self, PyObject * args, PyObject * kwargs) {

	char *name;
	char *menu_bar = (char*)"";
	PyObject *py_obj;
	PyObject *content_browser = nullptr;

	//if (!PyArg_ParseTuple(args, "sO|sO:add_menu_extension", &name, &py_obj, &menu_bar)) {
	//	return NULL;
	//}

	static char *kw_names[] = { (char *)"name", (char *)"callback", (char *)"menu_bar", (char *)"content_browser", NULL };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sO|sO:add_menu_extension", kw_names, &name, &py_obj, &menu_bar, &content_browser)) {
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
	if (content_browser)
		cmd->content_browser = PyObject_IsTrue(content_browser);

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