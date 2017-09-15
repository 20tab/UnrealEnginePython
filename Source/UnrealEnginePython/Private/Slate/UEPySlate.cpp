
#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorToolkit.h"
#include "Editor/Persona/Public/PersonaModule.h"
#if ENGINE_MINOR_VERSION > 13
#include "Editor/AnimationEditor/Public/IAnimationEditorModule.h"
#endif
#include "Editor/StaticMeshEditor/Public/StaticMeshEditorModule.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/ISinglePropertyView.h"
#include "Editor/PropertyEditor/Public/IDetailsView.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#endif

#include "Runtime/Slate/Public/Framework/Commands/UICommandList.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"
#include "Runtime/Slate/Public/Framework/Docking/TabManager.h"
#include "Runtime/Slate/Public/Widgets/Views/STableRow.h"

#include "Runtime/AppFramework/Public/Widgets/Colors/SColorPicker.h"


#include "UEPySlate.h"

FReply UPythonSlateDelegate::OnMouseEvent(const FGeometry &geometry, const FPointerEvent &pointer_event) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"OO", py_ue_new_fgeometry(geometry), py_ue_new_fpointer_event(pointer_event));
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

FReply UPythonSlateDelegate::OnKeyDown(const FGeometry &geometry, const FKeyEvent &key_event) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"OO", py_ue_new_fgeometry(geometry), py_ue_new_fkey_event(key_event));
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

void UPythonSlateDelegate::OnTextChanged(const FText& text) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"s", TCHAR_TO_UTF8(*text.ToString()));
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonSlateDelegate::OnStringChanged(const FString& text) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"s", TCHAR_TO_UTF8(*text));
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonSlateDelegate::OnTextCommitted(const FText& text, ETextCommit::Type commit_type) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"si", TCHAR_TO_UTF8(*text.ToString()), (int)commit_type);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonSlateDelegate::OnFloatChanged(float value) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"f", value);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonSlateDelegate::OnLinearColorChanged(FLinearColor color) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_flinearcolor(color));
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonSlateDelegate::OnFloatCommitted(float value, ETextCommit::Type commit_type) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"fi", value, (int)commit_type);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonSlateDelegate::CheckBoxChanged(ECheckBoxState state) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"i", (int)state);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

#if WITH_EDITOR
void UPythonSlateDelegate::OnAssetDoubleClicked(const FAssetData& AssetData) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_fassetdata(AssetData));
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void UPythonSlateDelegate::OnAssetSelected(const FAssetData& AssetData) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_fassetdata(AssetData));
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void UPythonSlateDelegate::OnAssetChanged(const FAssetData& AssetData) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_fassetdata(AssetData));
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

TSharedPtr<SWidget> UPythonSlateDelegate::OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets) {
	FScopePythonGIL gil;

	PyObject *py_list = PyList_New(0);
	for (FAssetData asset : SelectedAssets) {
		PyList_Append(py_list, py_ue_new_fassetdata(asset));
	}

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_list);
	Py_DECREF(py_list);
	if (!ret) {
		unreal_engine_py_log_error();
		return nullptr;
	}

	ue_PySWidget *s_widget = py_ue_is_swidget(ret);
	if (!s_widget) {
		Py_DECREF(ret);
		UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
		return nullptr;
	}
	TSharedPtr<SWidget> value = s_widget->s_widget;
	Py_DECREF(ret);
	return value;
}

void UPythonSlateDelegate::MenuPyAssetBuilder(FMenuBuilder &Builder, TArray<FAssetData> SelectedAssets) {
	FScopePythonGIL gil;

	PyObject *py_list = PyList_New(0);
	for (FAssetData asset : SelectedAssets) {
		PyList_Append(py_list, py_ue_new_fassetdata(asset));
	}

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"OO", py_ue_new_fmenu_builder(&Builder), py_list);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

TSharedRef<FExtender> UPythonSlateDelegate::OnExtendContentBrowserMenu(const TArray<FAssetData>& SelectedAssets) {
	TSharedRef<FExtender> Extender(new FExtender());

	Extender->AddMenuExtension((char *)"GetAssetActions", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateUObject(this, &UPythonSlateDelegate::MenuPyAssetBuilder, SelectedAssets));

	return Extender;
}

#endif

TSharedRef<SWidget> UPythonSlateDelegate::OnGenerateWidget(TSharedPtr<FPythonItem> py_item) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_item.Get()->py_object);
	if (!ret) {
		unreal_engine_py_log_error();
		return SNullWidget::NullWidget;
	}

	ue_PySWidget *s_widget = py_ue_is_swidget(ret);
	if (!s_widget) {
		Py_DECREF(ret);
		UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
		return SNullWidget::NullWidget;
	}
	TSharedRef<SWidget> value = s_widget->s_widget;
	Py_DECREF(ret);
	return value;
}

void UPythonSlateDelegate::OnSelectionChanged(TSharedPtr<FPythonItem> py_item, ESelectInfo::Type select_type) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"Oi", py_item.Get()->py_object, (int)select_type);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

TSharedPtr<SWidget> UPythonSlateDelegate::OnContextMenuOpening() {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return nullptr;
	}

	ue_PySWidget *s_widget = py_ue_is_swidget(ret);
	if (!s_widget) {
		Py_DECREF(ret);
		UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
		return nullptr;
	}
	TSharedPtr<SWidget> value = s_widget->s_widget;
	Py_DECREF(ret);
	return value;
}

void UPythonSlateDelegate::SimpleExecuteAction() {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void UPythonSlateDelegate::ExecuteAction(PyObject *py_obj) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_obj);
	if (!ret) {
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

FText UPythonSlateDelegate::GetterFText() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return FText();
	}
	PyObject *str = PyObject_Str(ret);
	if (!str) {
		unreal_engine_py_log_error();
		Py_DECREF(ret);
		return FText();
	}

	FText text = FText::FromString(PyUnicode_AsUTF8(str));
	Py_DECREF(str);
	Py_DECREF(ret);
	return text;
}

FString UPythonSlateDelegate::GetterFString() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return FString();
	}
	PyObject *str = PyObject_Str(ret);
	if (!str) {
		unreal_engine_py_log_error();
		Py_DECREF(ret);
		return FString();
	}

	FString fstr = UTF8_TO_TCHAR(PyUnicode_AsUTF8(str));
	Py_DECREF(str);
	Py_DECREF(ret);
	return fstr;
}

float UPythonSlateDelegate::GetterFloat() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return 0;
	}
	if (!PyNumber_Check(ret)) {
		PyErr_SetString(PyExc_ValueError, "returned value is not a number");
		Py_DECREF(ret);
		return 0;
	}

	PyObject *py_float = PyNumber_Float(ret);
	float n = PyFloat_AsDouble(py_float);
	Py_DECREF(py_float);
	Py_DECREF(ret);
	return n;
}

TOptional<float> UPythonSlateDelegate::GetterTFloat() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return 0;
	}
	if (!PyNumber_Check(ret)) {
		PyErr_SetString(PyExc_ValueError, "returned value is not a number");
		Py_DECREF(ret);
		return 0;
	}

	PyObject *py_float = PyNumber_Float(ret);
	float n = PyFloat_AsDouble(py_float);
	Py_DECREF(py_float);
	Py_DECREF(ret);
	return n;
}

int UPythonSlateDelegate::GetterInt() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return 0;
	}
	if (!PyNumber_Check(ret)) {
		PyErr_SetString(PyExc_ValueError, "returned value is not a number");
		Py_DECREF(ret);
		return 0;
	}

	PyObject *py_int = PyNumber_Long(ret);
	int n = PyLong_AsLong(py_int);
	Py_DECREF(py_int);
	Py_DECREF(ret);
	return n;
}

bool UPythonSlateDelegate::GetterBool() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return 0;
	}
	if (PyObject_IsTrue(ret)) {
		Py_DECREF(ret);
		return true;
	}

	Py_DECREF(ret);
	return false;
}

FVector2D UPythonSlateDelegate::GetterFVector2D() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return FVector2D();
	}

	if (!PyTuple_Check(ret)) {
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "returned value is not a tuple");
		return FVector2D();
	}

	if (PyTuple_Size(ret) != 2) {
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "returned value is not a 2 items tuple");
		return FVector2D();
	}

	PyObject *first_item = PyTuple_GetItem(ret, 0);
	if (!PyNumber_Check(first_item)) {
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "tuple does not contain numbers");
		return FVector2D();
	}

	PyObject *py_x = PyNumber_Float(first_item);
	float x = (float)PyFloat_AsDouble(py_x);

	Py_DECREF(py_x);

	PyObject *py_y = PyNumber_Float(PyTuple_GetItem(ret, 1));
	float y = (float)PyFloat_AsDouble(py_y);

	Py_DECREF(py_y);

	Py_DECREF(ret);
	return FVector2D(x, y);
}

FLinearColor UPythonSlateDelegate::GetterFLinearColor() const {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return FLinearColor();
	}

	ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(ret);

	if (!py_color) {
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "returned value is not a FLinearColor");
		return FLinearColor();
	}

	FLinearColor color = py_color->color;

	Py_DECREF(ret);
	return color;
}

TSharedRef<SDockTab> UPythonSlateDelegate::SpawnPythonTab(const FSpawnTabArgs &args) {
	TSharedRef<SDockTab> dock_tab = SNew(SDockTab).TabRole(ETabRole::NomadTab);
	PyObject *py_dock = (PyObject *)ue_py_get_swidget(dock_tab);
	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_dock);
	if (!ret) {
		unreal_engine_py_log_error();
	}
	else {
		Py_DECREF(ret);
	}
	return dock_tab;
}

TSharedRef<ITableRow> UPythonSlateDelegate::GenerateRow(TSharedPtr<FPythonItem> InItem, const TSharedRef<STableViewBase>& OwnerTable) {
	PyObject *ret = PyObject_CallFunction(py_callable, (char*)"O", InItem.Get()->py_object);
	if (!ret) {
		unreal_engine_py_log_error();
		return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable);
	}
	ue_PySWidget *s_widget = py_ue_is_swidget(ret);
	if (!s_widget) {
		UE_LOG(LogPython, Error, TEXT("python callable did not return a SWidget"));
		return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable);
	}

	return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable).Content()[s_widget->s_widget];
}

void UPythonSlateDelegate::GetChildren(TSharedPtr<FPythonItem> InItem, TArray<TSharedPtr<FPythonItem>>& OutChildren) {
	PyObject *ret = PyObject_CallFunction(py_callable, (char*)"O", InItem.Get()->py_object);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	PyObject *py_iterable = PyObject_GetIter(ret);
	if (!py_iterable || !PyIter_Check(py_iterable)) {
		UE_LOG(LogPython, Error, TEXT("returned value is not iterable"));
		Py_XDECREF(py_iterable);
		Py_DECREF(ret);
		return;
	}

	while (PyObject *item = PyIter_Next(py_iterable)) {
		Py_INCREF(item);
		OutChildren.Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}
	Py_DECREF(py_iterable);
	Py_DECREF(ret);
}

static std::map<SWidget *, ue_PySWidget *> *py_slate_mapping;

ue_PySWidget *ue_py_get_swidget(TSharedRef<SWidget> s_widget) {
	ue_PySWidget *ret = nullptr;
	auto it = py_slate_mapping->find(&s_widget.Get());
	// not found, it means it is an SWidget not generated from python
	if (it == py_slate_mapping->end()) {
		if (s_widget->GetType().Compare(FName("SWindow")) == 0) {
			return py_ue_new_swidget<ue_PySWindow>(s_widget, &ue_PySWindowType);
		}
		if (s_widget->GetType().Compare(FName("SDockTab")) == 0) {
			return py_ue_new_swidget<ue_PySDockTab>(s_widget, &ue_PySDockTabType);
		}
		else {
			return py_ue_new_swidget<ue_PySWidget>(s_widget, &ue_PySWidgetType);
		}
	}
	else {
		ret = it->second;
	}
	Py_INCREF(ret);
	return ret;
}

void ue_py_setup_swidget(ue_PySWidget *self) {
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Allocating new %s..."), UTF8_TO_TCHAR(self->ob_base.ob_type->tp_name));
#endif
	self->py_dict = PyDict_New();
	new(&self->s_widget) TSharedRef<SWidget>(SNullWidget::NullWidget);
	new(&self->delegates) TArray<UPythonSlateDelegate *>();
	new(&self->py_swidget_slots) TArray<ue_PySWidget *>();
	new(&self->py_refs) TArray<PyObject *>();
	self->py_swidget_content = nullptr;
}

void ue_py_register_swidget(SWidget *s_widget, ue_PySWidget *py_s_widget) {
	(*py_slate_mapping)[s_widget] = py_s_widget;
}

void ue_py_unregister_swidget(SWidget *s_widget) {
	(*py_slate_mapping).erase(s_widget);
}

void ue_python_init_slate(PyObject *module) {

	py_slate_mapping = new std::map<SWidget *, ue_PySWidget *>();

	ue_python_init_swidget(module);
	ue_python_init_scompound_widget(module);
	ue_python_init_swindow(module);
	ue_python_init_sborder(module);
	ue_python_init_sbutton(module);
	ue_python_init_sleaf_widget(module);
	ue_python_init_stext_block(module);
	ue_python_init_seditable_text_box(module);
	ue_python_init_smulti_line_editable_text(module);
	ue_python_init_spanel(module);
	ue_python_init_sgrid_panel(module);
	ue_python_init_sbox_panel(module);
	ue_python_init_shorizontal_box(module);
	ue_python_init_svertical_box(module);
	ue_python_init_sviewport(module);
	ue_python_init_simage(module);
	ue_python_init_sdock_tab(module);
	ue_python_init_stable_view_base(module);
	ue_python_init_slist_view(module);
	ue_python_init_spython_list_view(module);
	ue_python_init_stree_view(module);
	ue_python_init_spython_tree_view(module);
	ue_python_init_ssplitter(module);
	ue_python_init_sheader_row(module);
	ue_python_init_scheck_box(module);
	ue_python_init_snumeric_entry_box(module);
	ue_python_init_scanvas(module);
	ue_python_init_sslider(module);
	ue_python_init_svector_input_box(module);
	ue_python_init_srotator_input_box(module);
	ue_python_init_spython_combo_box(module);
	ue_python_init_sscroll_box(module);
	ue_python_init_scolor_block(module);
	ue_python_init_sbox(module);
	ue_python_init_sprogress_bar(module);
	ue_python_init_sspacer(module);
	ue_python_init_spython_widget(module);



#if WITH_EDITOR
	ue_python_init_seditor_viewport(module);
	ue_python_init_slevel_viewport(module);
	ue_python_init_spython_editor_viewport(module);
	ue_python_init_sgraph_editor(module);
#if ENGINE_MINOR_VERSION > 14
	ue_python_init_spython_shelf(module);
#if ENGINE_MINOR_VERSION > 13
	ue_python_init_sfile_path_picker(module);
#endif
#endif
	ue_python_init_sdrop_target(module);
	ue_python_init_sasset_drop_target(module);
	ue_python_init_sobject_property_entry_box(module);
#endif

	ue_python_init_ftab_manager(module);
	ue_python_init_ftab_spawner_entry(module);
	ue_python_init_fmenu_builder(module);
	ue_python_init_fslate_style_set(module);
	ue_python_init_ftool_bar_builder(module);
	ue_python_init_fslate_icon(module);

	ue_python_init_fgeometry(module);
	ue_python_init_fpaint_context(module);

	ue_python_init_finput_event(module);
	ue_python_init_fpointer_event(module);
	ue_python_init_fkey_event(module);
}

PyObject *ue_py_dict_get_item(PyObject *dict, const char *key) {
	if (dict == nullptr)
		return nullptr;
	return PyDict_GetItemString(dict, key);
}

PyObject *py_unreal_engine_get_editor_window(PyObject *self, PyObject *args) {

	if (!FGlobalTabmanager::Get()->GetRootWindow().IsValid()) {
		return PyErr_Format(PyExc_Exception, "no RootWindow found");
	}

	return (PyObject *)ue_py_get_swidget(FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
}

// slate commands tool class
class FPythonSlateCommands : public TCommands<FPythonSlateCommands>
{
public:

	FPythonSlateCommands()
		: TCommands<FPythonSlateCommands>(TEXT("UnrealEnginePython"), NSLOCTEXT("Contexts", "UnrealEnginePython", "UnrealEnginePython"), NAME_None, "EditorStyle")
	{
	}

	void Setup(char *command_name, PyObject *py_object) {
		py_callable = py_object;
		Py_INCREF(py_callable);

		name = FString(command_name);
	}

	// TCommands<> interface
	virtual void RegisterCommands() override {
		commands = MakeShareable(new FUICommandList);

		UI_COMMAND_Function(this, command, nullptr, *name, *name, TCHAR_TO_UTF8(*name), *name, *name, EUserInterfaceActionType::Button, FInputGesture());
		commands->MapAction(command, FExecuteAction::CreateRaw(this, &FPythonSlateCommands::Callback), FCanExecuteAction());
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

	void MenuBuilder(FMenuBuilder &Builder) {
		Builder.AddMenuEntry(command);
	}

	void MenuPyBuilder(FMenuBuilder &Builder) {
		PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_fmenu_builder(&Builder));
		if (!ret) {
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	void MenuBarBuilder(FMenuBarBuilder &Builder) {
		FText label = FText::FromString(name);
		Builder.AddPullDownMenu(label, label, FNewMenuDelegate::CreateRaw(this, &FPythonSlateCommands::MenuPyBuilder));
	}

	void ToolBarBuilder(FToolBarBuilder &Builder) {
		PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_ue_new_ftool_bar_builder(&Builder));
		if (!ret) {
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	TSharedPtr<FUICommandList> GetCommands() {
		return commands;
	}
private:
	TSharedPtr<FUICommandList> commands;
	TSharedPtr<FUICommandInfo> command;
	PyObject *py_callable;

	FString name;
};

#if WITH_EDITOR

PyObject *py_unreal_engine_create_detail_view(PyObject *self, PyObject * args, PyObject *kwargs) {

	PyObject *py_object;
	PyObject *py_allow_search = nullptr;

	char *kwlist[] = {
		(char *)"uobject",
		(char *)"allow_search",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:create_detail_view", kwlist,
		&py_object, &py_allow_search)) {
		return nullptr;
	}

	UObject *u_object = ue_py_check_type<UObject>(py_object);
	if (!u_object) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs view_args;
	view_args.bAllowSearch = (py_allow_search && PyObject_IsTrue(py_allow_search));
	view_args.NameAreaSettings = FDetailsViewArgs::ENameAreaSettings::HideNameArea;

	TSharedPtr<IDetailsView> view = PropertyEditorModule.CreateDetailView(view_args);
	view->SetObject(u_object);

	return (PyObject *)py_ue_new_swidget<ue_PySWidget>(view->AsShared(), &ue_PySWidgetType);
}

PyObject *py_unreal_engine_create_property_view(PyObject *self, PyObject * args, PyObject *kwargs) {

	PyObject *py_object;
	char *name;
	char *name_override = nullptr;

	char *kwlist[] = {
		(char*)"uobject",
		(char *)"name",
		(char *)"name_override",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Os|s:create_property_view", kwlist,
		&py_object, &name, &name_override)) {
		return nullptr;
	}

	UObject *u_object = ue_py_check_type<UObject>(py_object);
	if (!u_object) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FSinglePropertyParams params;
	if (name_override)
		params.NameOverride = FText::FromString(UTF8_TO_TCHAR(name_override));

	auto view_widget = PropertyEditorModule.CreateSingleProperty(u_object, FName(name), params);

	if (!view_widget.IsValid())
		return PyErr_Format(PyExc_Exception, "unable to create SingleProperty widget");

	return (PyObject *)py_ue_new_swidget<ue_PySWidget>(view_widget->AsShared(), &ue_PySWidgetType);
}


PyObject *py_unreal_engine_add_menu_extension(PyObject * self, PyObject * args) {

	char *command_name;
	PyObject *py_callable;

	char *hook = (char *)"WindowLayout";
	char *module = (char*)"LevelEditor";

	if (!PyArg_ParseTuple(args, "sO|ss:add_menu_extension", &command_name, &py_callable, &hook, &module)) {
		return NULL;
	}

	if (!FModuleManager::Get().ModuleExists(UTF8_TO_TCHAR(module)))
		return PyErr_Format(PyExc_Exception, "module %s does not exist", module);

	// unfortunately we need to manually check for module names :(
	IHasMenuExtensibility *menu_extension_interface = nullptr;

	if (!strcmp(module, (char *)"LevelEditor")) {
		FLevelEditorModule &Module = FModuleManager::LoadModuleChecked<FLevelEditorModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}
	else if (!strcmp(module, (char *)"Persona")) {
		FPersonaModule &Module = FModuleManager::LoadModuleChecked<FPersonaModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}
#if ENGINE_MINOR_VERSION > 13
	else if (!strcmp(module, (char *)"AnimationEditor")) {
		IAnimationEditorModule &Module = FModuleManager::LoadModuleChecked<IAnimationEditorModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}
#endif
	else if (!strcmp(module, (char *)"StaticMeshEditor")) {
		IStaticMeshEditorModule &Module = FModuleManager::LoadModuleChecked<IStaticMeshEditorModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}

	if (!menu_extension_interface)
		return PyErr_Format(PyExc_Exception, "module %s is not supported", module);

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	TSharedRef<FPythonSlateCommands> *commands = new TSharedRef<FPythonSlateCommands>(new FPythonSlateCommands());

	commands->Get().Setup(command_name, py_callable);

	commands->Get().RegisterCommands();

	TSharedRef<FExtender> extender = MakeShareable(new FExtender());
	extender->AddMenuExtension(hook, EExtensionHook::After, commands->Get().GetCommands(), FMenuExtensionDelegate::CreateRaw(&commands->Get(), &FPythonSlateCommands::MenuBuilder));

	menu_extension_interface->GetMenuExtensibilityManager()->AddExtender(extender);

	Py_RETURN_NONE;
}


PyObject *py_unreal_engine_add_menu_bar_extension(PyObject * self, PyObject * args) {

	char *command_name;
	PyObject *py_callable;

	char *hook = (char *)"Help";

	if (!PyArg_ParseTuple(args, "sO|s:add_menu_bar_extension", &command_name, &py_callable, &hook)) {
		return NULL;
	}

	FLevelEditorModule &ExtensibleModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	TSharedRef<FPythonSlateCommands> *commands = new TSharedRef<FPythonSlateCommands>(new FPythonSlateCommands());

	commands->Get().Setup(command_name, py_callable);

	commands->Get().RegisterCommands();

	TSharedRef<FExtender> extender = MakeShareable(new FExtender());
	extender->AddMenuBarExtension(hook, EExtensionHook::After, commands->Get().GetCommands(), FMenuBarExtensionDelegate::CreateRaw(&commands->Get(), &FPythonSlateCommands::MenuBarBuilder));

	ExtensibleModule.GetMenuExtensibilityManager()->AddExtender(extender);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_add_tool_bar_extension(PyObject * self, PyObject * args) {

	char *command_name;
	PyObject *py_callable;

	char *hook = (char *)"Settings";

	if (!PyArg_ParseTuple(args, "sO|s:add_tool_bar_extension", &command_name, &py_callable, &hook)) {
		return NULL;
	}

	FLevelEditorModule &ExtensibleModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	TSharedRef<FPythonSlateCommands> *commands = new TSharedRef<FPythonSlateCommands>(new FPythonSlateCommands());

	commands->Get().Setup(command_name, py_callable);

	commands->Get().RegisterCommands();

	TSharedRef<FExtender> extender = MakeShareable(new FExtender());
	extender->AddToolBarExtension(hook, EExtensionHook::After, commands->Get().GetCommands(), FToolBarExtensionDelegate::CreateRaw(&commands->Get(), &FPythonSlateCommands::ToolBarBuilder));

	ExtensibleModule.GetToolBarExtensibilityManager()->AddExtender(extender);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_add_asset_view_context_menu_extension(PyObject * self, PyObject * args) {

	PyObject *py_callable;

	if (!PyArg_ParseTuple(args, "O:add_asset_view_context_menu_extension", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	FContentBrowserModule &ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FContentBrowserMenuExtender_SelectedAssets> &Extenders = ContentBrowser.GetAllAssetViewContextMenuExtenders();

	FContentBrowserMenuExtender_SelectedAssets handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnExtendContentBrowserMenu);

	Extenders.Add(handler);

	Py_RETURN_NONE;
}
#endif

PyObject *py_unreal_engine_register_nomad_tab_spawner(PyObject * self, PyObject * args) {

	char *name;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "sO:register_nomad_tab_spawner", &name, &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	FOnSpawnTab spawn_tab;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	spawn_tab.BindUObject(py_delegate, &UPythonSlateDelegate::SpawnPythonTab);

	FTabSpawnerEntry *spawner_entry = &FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UTF8_TO_TCHAR(name), spawn_tab)
		// TODO: more generic way to set the group
#if WITH_EDITOR
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
#endif
		;

	PyObject *ret = py_ue_new_ftab_spawner_entry(spawner_entry);
	Py_INCREF(ret);
	return ret;
}

PyObject *py_unreal_engine_unregister_nomad_tab_spawner(PyObject * self, PyObject * args) {

	char *name;
	if (!PyArg_ParseTuple(args, "s:unregister_nomad_tab_spawner", &name)) {
		return NULL;
	}

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UTF8_TO_TCHAR(name));

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_invoke_tab(PyObject * self, PyObject * args) {

	char *name;
	if (!PyArg_ParseTuple(args, "s:invoke_tab", &name)) {
		return NULL;
	}

	FGlobalTabmanager::Get()->InvokeTab(FTabId(FName(UTF8_TO_TCHAR(name))));

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *py_unreal_engine_open_color_picker(PyObject *self, PyObject *args, PyObject *kwargs) {

	PyObject *py_callable_on_color_committed = nullptr;

	char *kwlist[] = {
		(char *)"on_color_committed",
		nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O:open_color_picker", kwlist,
		&py_callable_on_color_committed)) {
		return nullptr;
	}

	if (!PyCallable_Check(py_callable_on_color_committed)) {
		return PyErr_Format(PyExc_Exception, "on_color_committed must be a callable");
	}

	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable_on_color_committed);
	// TODO: this memory should be freed...
	py_delegate->AddToRoot();

	FColorPickerArgs color_args;
	color_args.OnColorCommitted.BindUObject(py_delegate, &UPythonSlateDelegate::OnLinearColorChanged);

	if (OpenColorPicker(color_args)) {
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject *py_unreal_engine_destroy_color_picker(PyObject *self, PyObject * args) {
	DestroyColorPicker();
	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_play_sound(PyObject *self, PyObject * args) {
	PyObject *py_sound;
	int user_index;
	if (!PyArg_ParseTuple(args, "O|i:play_sound", &py_sound, &user_index)) {
		return nullptr;
	}

	FSlateSound *sound = ue_py_check_struct<FSlateSound>(py_sound);
	if (!sound) {
		USoundBase *u_sound = ue_py_check_type<USoundBase>(py_sound);
		if (u_sound) {
			FSlateSound slate_sound = FSlateSound();
			slate_sound.SetResourceObject(u_sound);
			sound = &slate_sound;
		}
	}

	if (!sound) {
		return PyErr_Format(PyExc_Exception, "argument is not a FSlateColor or a USoundBase");
	}

	FSlateApplication::Get().PlaySound(*sound, user_index);
	Py_RETURN_NONE;
}
