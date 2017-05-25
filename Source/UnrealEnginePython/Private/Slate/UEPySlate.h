#pragma once

#include "UnrealEnginePython.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#if WITH_EDITOR
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#endif

#include <map>


#include "UEPySWidget.h"
#include "UEPySCompoundWidget.h"
#include "UEPySWindow.h"
#include "UEPySBorder.h"
#include "UEPySButton.h"
#include "UEPySLeafWidget.h"
#include "UEPySTextBlock.h"
#include "UEPySEditableTextBox.h"
#include "UEPySMultiLineEditableText.h"
#include "UEPySPanel.h"
#include "UEPySGridPanel.h"
#include "UEPySBoxPanel.h"
#include "UEPySHorizontalBox.h"
#include "UEPySViewport.h"

#include "UEPySImage.h"
#include "UEPySDockTab.h"
#include "UEPySTableViewBase.h"
#include "UEPySListView.h"
#include "UEPySPythonListView.h"
#include "UEPySSplitter.h"
#include "UEPySHeaderRow.h"



#include "UEPyFTabSpawnerEntry.h"
#include "UEPyFMenuBuilder.h"

#if WITH_EDITOR
#include "UEPySEditorViewport.h"
#include "UEPySPythonEditorViewport.h"
#include "UEPySGraphEditor.h"
#include "UEPySPythonShelf.h"
#endif

#include "UEPySlate.generated.h"


PyObject *py_unreal_engine_get_editor_window(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_menu_extension(PyObject *, PyObject *);
PyObject *py_unreal_engine_register_nomad_tab_spawner(PyObject *, PyObject *);
PyObject *py_unreal_engine_unregister_nomad_tab_spawner(PyObject *, PyObject *);

void ue_py_register_swidget(SWidget *, ue_PySWidget *);

template<typename T> TSharedRef<T> ue_py_init_swidget(ue_PySWidget *py_swidget) {
	TSharedRef<T> new_swidget = TSharedRef<T>(SNew(T));
	ue_py_register_swidget(&new_swidget.Get(), py_swidget);
	return new_swidget;
}

template<typename T> ue_PySWidget *py_ue_new_swidget(SWidget *s_widget, PyTypeObject *py_type) {
	ue_PySWidget *ret = (ue_PySWidget *)PyObject_New(T, py_type);

	ret->s_widget = s_widget;

	ue_py_register_swidget(s_widget, ret);
	return ret;
}

#define ue_py_snew(T, field)  self->field.s_widget_owned = ue_py_init_swidget<T>((ue_PySWidget *)self); self->field.s_widget = &self->field.s_widget_owned.Get()

ue_PySWidget *ue_py_get_swidget(TSharedPtr<SWidget> s_widget);



void ue_python_init_slate(PyObject *);

UCLASS()
class UPythonSlateDelegate : public UPythonDelegate
{
	GENERATED_BODY()

public:
	FReply OnMouseEvent(const FGeometry &geometry, const FPointerEvent &pointer_event);
	FReply OnClicked();

	TSharedRef<SDockTab> SpawnPythonTab(const FSpawnTabArgs& args);

	TSharedRef<ITableRow> GenerateWidgetForList(TSharedPtr<PyObject> InItem, const TSharedRef<STableViewBase>& OwnerTable);

#if WITH_EDITOR
	void OnAssetDoubleClicked(const FAssetData& AssetData);
	TSharedPtr<SWidget> OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets);
#endif

	void SimpleExecuteAction();
	void ExecuteAction(PyObject *py_obj);
};


