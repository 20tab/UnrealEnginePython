#pragma once



#include "UnrealEnginePython.h"

#include "SlateBasics.h"
#include "SlateExtras.h"


#if WITH_EDITOR
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "UnrealEdMisc.h"
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
#include "UEPySVerticalBox.h"
#include "UEPySViewport.h"


#include "UEPySImage.h"
#include "UEPySDockTab.h"
#include "UEPySTableViewBase.h"
#include "UEPySListView.h"
#include "UEPySPythonListView.h"
#include "UEPySPythonMultiColumnTableRow.h"
#include "UEPySTreeView.h"
#include "UEPySPythonTreeView.h"
#include "UEPySSplitter.h"
#include "UEPySHeaderRow.h"
#include "UEPySCheckBox.h"
#include "UEPySNumericEntryBox.h"
#include "UEPySCanvas.h"
#include "UEPySSlider.h"
#include "UEPySVectorInputBox.h"
#include "UEPySRotatorInputBox.h"
#include "UEPySPythonComboBox.h"
#include "UEPySScrollBox.h"
#include "UEPySColorBlock.h"
#include "UEPySBox.h"
#include "UEPySProgressBar.h"
#include "UEPySSpacer.h"
#include "UEPySPythonWidget.h"
#include "UEPySOverlay.h"

#include "UEPyFTabManager.h"
#include "UEPyFTabSpawnerEntry.h"
#include "UEPyFMenuBuilder.h"
#include "UEPyFToolBarBuilder.h"
#include "UEPyFSlateIcon.h"
#include "UEPyFSlateStyleSet.h"

#include "UEPyFGeometry.h"
#include "UEPyFPaintContext.h"

#include "UEPyFInputEvent.h"
#include "UEPyFPointerEvent.h"
#include "UEPyFKeyEvent.h"
#include "UEPyFCharacterEvent.h"
#include "UEPyFModifierKeysState.h"

#if WITH_EDITOR
#include "UEPySEditorViewport.h"
#include "UEPySLevelViewport.h"
#include "UEPySPythonEditorViewport.h"
#include "UEPySGraphEditor.h"
#include "UEPySPythonShelf.h"
#include "UEPySFilePathPicker.h"
#include "UEPySDropTarget.h"
#include "UEPySAssetDropTarget.h"
#include "UEPySObjectPropertyEntryBox.h"
#include "UEPyIDetailsView.h"
#include "UEPyIStructureDetailsView.h"
#include "UEPySNodePanel.h"
#include "UEPySGraphPanel.h"
#endif

#include "Runtime/Core/Public/Misc/Attribute.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"

#include "PythonDelegate.h"

PyObject *py_unreal_engine_get_editor_window(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_slate_style(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_icon_for_class(PyObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_add_menu_extension(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_menu_bar_extension(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_tool_bar_extension(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_detail_view(PyObject *, PyObject *, PyObject *);
PyObject *py_unreal_engine_create_structure_detail_view(PyObject *, PyObject *, PyObject *);
PyObject *py_unreal_engine_create_property_view(PyObject *, PyObject *, PyObject *);

PyObject *py_unreal_engine_add_asset_view_context_menu_extension(PyObject * self, PyObject *);
#endif

PyObject *py_unreal_engine_invoke_tab(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_swidget_from_wrapper(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_wrapper_from_pyswidget(PyObject *, PyObject *);

PyObject *py_unreal_engine_register_nomad_tab_spawner(PyObject *, PyObject *);
PyObject *py_unreal_engine_unregister_nomad_tab_spawner(PyObject *, PyObject *);

PyObject *py_unreal_engine_open_color_picker(PyObject *, PyObject *, PyObject *);
PyObject *py_unreal_engine_destroy_color_picker(PyObject *, PyObject *);

PyObject *py_unreal_engine_play_sound(PyObject *, PyObject *);

void ue_py_register_swidget(SWidget *, ue_PySWidget *);
void ue_py_unregister_swidget(SWidget *);

void ue_py_setup_swidget(ue_PySWidget *);


PyObject *ue_py_dict_get_item(PyObject *, const char *);

template<typename T> ue_PySWidget *py_ue_new_swidget(TSharedRef<SWidget> s_widget, PyTypeObject *py_type)
{
	ue_PySWidget *ret = (ue_PySWidget *)PyObject_New(T, py_type);

	ue_py_setup_swidget(ret);

	ret->s_widget = s_widget;

	ue_py_register_swidget(&s_widget.Get(), ret);
	return ret;
}

#define ue_py_snew_base(T, field, required, arguments) self->field.s_widget = TSharedRef<T>(MakeTDecl<T>(#T, __FILE__, __LINE__, required) <<= arguments); ue_py_register_swidget((SWidget *)&self->field.s_widget.Get(), (ue_PySWidget *)self)

#define ue_py_snew_simple(T, field) ue_py_snew_base(T, field, RequiredArgs::MakeRequiredArgs(), T::FArguments())

#define ue_py_snew_simple_with_req_args(T, field, ... ) ue_py_snew_base(T, field, RequiredArgs::MakeRequiredArgs(__VA_ARGS__), T::FArguments())

#define ue_py_snew(T, field) ue_py_snew_base(T, field, RequiredArgs::MakeRequiredArgs(), arguments)

#define ue_py_snew_with_args(T, field, args) ue_py_snew_base(T, field, RequiredArgs::MakeRequiredArgs(args), arguments)


ue_PySWidget *ue_py_get_swidget(TSharedRef<SWidget> s_widget);

#define ue_py_slate_base_up(_base, _func, _param, _attribute) \
{\
	PyObject *value = ue_py_dict_get_item(kwargs, _param);\
	if (value) {\
		if (PyCalllable_Check_Extended(value)) {\
			_base handler;\
			ue_PySWidget *py_swidget = (ue_PySWidget *)self;\
			TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewSlateDelegate(py_swidget->s_widget, value);\
			handler.Bind(py_delegate, &FPythonSlateDelegate::_func);\
			arguments._attribute(handler);\
		}

#define ue_py_slate_base_event_up(_base, _func, _param, _attribute) \
{\
	PyObject *value = ue_py_dict_get_item(kwargs, _param);\
	if (value) {\
		if (PyCalllable_Check_Extended(value)) {\
			_base handler;\
			ue_PySWidget *py_swidget = (ue_PySWidget *)self;\
			TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewSlateDelegate(py_swidget->s_widget, value);\
			handler.BindSP(py_delegate, &FPythonSlateDelegate::_func);\
			arguments._attribute(handler);\
		}

#define ue_py_slate_down(_param) else {\
			PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " _param); \
			return -1;\
		}\
	}\
}

#define ue_py_slate_up(_type, _func, _param, _attribute) ue_py_slate_base_up(TAttribute<_type>, _func, _param, _attribute)

#define ue_py_slate_farguments_text(param, attribute) ue_py_slate_up(FText, GetterFText, param, attribute)\
	else if (PyUnicode_Check(value)) {\
		arguments.attribute(FText::FromString(UTF8_TO_TCHAR(PyUnicode_AsUTF8(value))));\
	}\
	ue_py_slate_down(param)


#define ue_py_slate_farguments_string(param, attribute) ue_py_slate_up(FString, GetterFString, param, attribute)\
	else if (PyUnicode_Check(value)) {\
		arguments.attribute(UTF8_TO_TCHAR(PyUnicode_AsUTF8(value)));\
	}\
	ue_py_slate_down(param)



#define ue_py_slate_farguments_float(param, attribute) ue_py_slate_up(float, GetterFloat, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute(PyFloat_AsDouble(py_float)); \
			Py_DECREF(py_float);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_tfloat(param, attribute) ue_py_slate_up(TOptional<float>, GetterTFloat, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute((TOptional<float>)PyFloat_AsDouble(py_float)); \
			Py_DECREF(py_float);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_fvector2d(param, attribute) ue_py_slate_up(FVector2D, GetterFVector2D, param, attribute)\
		else if (PyTuple_Check(value)) {\
			if (PyTuple_Size(value) == 2) {\
				PyObject *py_first = PyTuple_GetItem(value, 0);\
				PyObject *py_second = PyTuple_GetItem(value, 1);\
				if (PyNumber_Check(py_first)) {\
					PyObject *py_x = PyNumber_Float(py_first);\
					PyObject *py_y = PyNumber_Float(py_second);\
					arguments.attribute(FVector2D(PyFloat_AsDouble(py_x), PyFloat_AsDouble(py_y)));\
					Py_DECREF(py_x);\
					Py_DECREF(py_y);\
				}\
			}\
		}\
		ue_py_slate_down(param)





#define ue_py_slate_farguments_int(param, attribute) ue_py_slate_up(int, GetterInt, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((int)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		ue_py_slate_down(param)

#define ue_py_slate_farguments_tint(param, attribute) ue_py_slate_up(TOptional<int32>, GetterIntT<TOptional<int32>>, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((TOptional<int32>)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_enum(param, attribute, _type) ue_py_slate_up(_type, GetterIntT<_type>, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((_type)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		ue_py_slate_down(param)



#define ue_py_slate_farguments_flinear_color(param, attribute) ue_py_slate_up(FLinearColor, GetterFLinearColor, param, attribute)\
		else if (ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(value)) {\
			arguments.attribute(py_color->color); \
		}\
		ue_py_slate_down(param)



#define ue_py_slate_farguments_struct(param, attribute, _type) ue_py_slate_up(_type, GetterStructT<_type>, param, attribute)\
		else if (_type *u_struct = ue_py_check_struct<_type>(value)) {\
			arguments.attribute((_type)*u_struct); \
		}\
		ue_py_slate_down(param)



#define ue_py_slate_farguments_optional_struct(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (_type *u_struct = ue_py_check_struct<_type>(value)) {\
				arguments.attribute((_type)*u_struct); \
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}


#define ue_py_slate_farguments_optional_struct_ptr(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (_type *u_struct = ue_py_check_struct<_type>(value)) {\
				Py_INCREF(value);\
				arguments.attribute((_type *)u_struct);\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}

#define ue_py_slate_farguments_optional_uobject(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (_type *u_object = ue_py_check_type<_type>(value)) {\
				arguments.attribute((_type *)u_object);\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}


#define ue_py_slate_farguments_optional_fvector2d(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (PyTuple_Check(value)) {\
				if (PyTuple_Size(value) == 2) {\
					PyObject *py_first = PyTuple_GetItem(value, 0);\
					PyObject *py_second = PyTuple_GetItem(value, 1);\
					if (PyNumber_Check(py_first)) {\
						PyObject *py_x = PyNumber_Float(py_first);\
						PyObject *py_y = PyNumber_Float(py_second);\
						arguments.attribute(FVector2D(PyFloat_AsDouble(py_x), PyFloat_AsDouble(py_y)));\
						Py_DECREF(py_x);\
						Py_DECREF(py_y);\
					}\
				}\
				else {\
					PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
					return -1;\
				}\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}


#define ue_py_slate_farguments_optional_enum(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (PyNumber_Check(value)) {\
				PyObject *py_int = PyNumber_Long(value);\
				arguments.attribute((_type)PyLong_AsLong(py_int));\
				Py_DECREF(py_int);\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}



#define ue_py_slate_farguments_optional_float(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute(PyFloat_AsDouble(py_float)); \
			Py_DECREF(py_float);\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}

#define ue_py_slate_farguments_optional_foptional_size(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute(FOptionalSize(PyFloat_AsDouble(py_float))); \
			Py_DECREF(py_float);\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}

#define ue_py_slate_farguments_optional_string(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (PyUnicode_Check(value)) {\
		arguments.attribute(UTF8_TO_TCHAR(PyUnicode_AsUTF8(value)));\
	}\
}

#define ue_py_slate_farguments_optional_text(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyUnicode_Check(value)) {\
			arguments.attribute(FText::FromString(UTF8_TO_TCHAR(PyUnicode_AsUTF8(value))));\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}

#define ue_py_slate_farguments_optional_named_slot(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (ue_PySWidget *py_swidget = py_ue_is_swidget(value)) {\
            Py_INCREF(py_swidget);\
            arguments.attribute()\
            [\
                py_swidget->s_widget\
            ];\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}


#define ue_py_slate_farguments_bool(param, attribute) ue_py_slate_up(bool, GetterBool, param, attribute)\
		else if (PyObject_IsTrue(value)) {\
			arguments.attribute(true); \
		}\
		else {\
			arguments.attribute(false); \
		}\
	}\
}


#define ue_py_slate_farguments_optional_bool(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyObject_IsTrue(value)) {\
			arguments.attribute(true); \
		}\
		else {\
			arguments.attribute(false); \
		}\
	}\
}



#define ue_py_slate_farguments_event(param, attribute, type, method) ue_py_slate_base_event_up(type, method, param, attribute)\
		ue_py_slate_down(param)



#define ue_py_slate_setup_farguments(_type) _type::FArguments arguments;\
	ue_py_slate_farguments_bool("is_enabled", IsEnabled);\
	ue_py_slate_farguments_text("tool_tip_text", ToolTipText);\
    ue_py_slate_farguments_fvector2d("render_transform_pivot", RenderTransformPivot)

#define ue_py_slate_farguments_required_slot(param) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
    value = value ? value : PyTuple_GetItem(args, 0);\
	if (ue_PySWidget *py_swidget = value ? py_ue_is_swidget(value) : nullptr) {\
        Py_INCREF(py_swidget);\
        ue_PySWidget *self_py_swidget = py_ue_is_swidget((PyObject*)self);\
        arguments.AttachWidget(py_swidget->s_widget->AsShared());\
	}\
	else {\
		PyErr_SetString(PyExc_TypeError, "unsupported type for required slot " param); \
		return -1;\
	}\
}

#define ue_py_slate_setup_hack_slot_args(_type, _swidget_ref) _type::FSlot &arguments = _swidget_ref->AddSlot();\
    ue_py_slate_farguments_required_slot("widget");

void ue_python_init_slate(PyObject *);

struct FPythonItem
{
	PyObject *py_object = nullptr;

	FPythonItem(PyObject *item)
	{
		py_object = item;
	}
};


class FPythonSlateDelegate : public FPythonSmartDelegate
{

public:
	FReply OnMouseEvent(const FGeometry &geometry, const FPointerEvent &pointer_event);
	FReply OnClicked();

	FReply OnKeyDown(const FGeometry &geometry, const FKeyEvent &key_event);
	void OnTextChanged(const FText &text);
	void OnTextCommitted(const FText &text, ETextCommit::Type commit_type);
    void OnInt32Changed(int32 value);
    void OnInt32Committed(int32 value, ETextCommit::Type commit_type);
	void OnFloatChanged(float value);
	void OnFloatCommitted(float value, ETextCommit::Type commit_type);
    void OnSort(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode);

	void OnLinearColorChanged(FLinearColor color);

	void OnStringChanged(const FString &text);

	TSharedRef<SDockTab> SpawnPythonTab(const FSpawnTabArgs& args);

	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FPythonItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void GetChildren(TSharedPtr<FPythonItem> InItem, TArray<TSharedPtr<FPythonItem>>& OutChildren);

#if WITH_EDITOR
	void OnAssetDoubleClicked(const FAssetData& AssetData);
	TSharedPtr<SWidget> OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets);
	void OnAssetSelected(const FAssetData& AssetData);
	TSharedRef<FExtender> OnExtendContentBrowserMenu(const TArray<FAssetData> &SelectedAssets);
	void MenuPyAssetBuilder(FMenuBuilder &Builder, TArray<FAssetData> SelectedAssets);
	void OnAssetChanged(const FAssetData &AssetData);
	bool OnShouldFilterAsset(const FAssetData& AssetData);
#endif

	void OnWindowClosed(const TSharedRef<SWindow> &Window);

	TSharedPtr<SWidget> OnContextMenuOpening();
	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FPythonItem> py_item);
    TSharedRef<SWidget> OnGetMenuContent();
	void OnSelectionChanged(TSharedPtr<FPythonItem> py_item, ESelectInfo::Type select_type);

	void SimpleExecuteAction();
	void ExecuteAction(PyObject *py_obj);

	FText GetterFText() const;
	FString GetterFString() const;
	float GetterFloat() const;
	TOptional<float> GetterTFloat() const;
	int GetterInt() const;
	bool GetterBool() const;

	FVector2D GetterFVector2D() const;
	FLinearColor GetterFLinearColor() const;
	void CheckBoxChanged(ECheckBoxState state);


	template<typename T> T GetterIntT() const
	{
		FScopePythonGIL gil;

		PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
		if (!ret)
		{
			unreal_engine_py_log_error();
			return (T)0;
		}
		if (!PyNumber_Check(ret))
		{
			PyErr_SetString(PyExc_ValueError, "returned value is not a number");
			Py_DECREF(ret);
			return (T)0;
		}

		PyObject *py_int = PyNumber_Long(ret);
		int n = PyLong_AsLong(py_int);
		Py_DECREF(py_int);
		Py_DECREF(ret);
		return (T)n;
	}

	template<typename T> T GetterStructT() const
	{
		FScopePythonGIL gil;

		PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
		if (!ret)
		{
			unreal_engine_py_log_error();
			return T();
		}

		T *u_struct = ue_py_check_struct<T>(ret);
		if (!u_struct)
		{
			PyErr_SetString(PyExc_ValueError, "returned value is not a UStruct");
			Py_DECREF(ret);
			return T();
		}

		T u_struct_copy = *u_struct;
		Py_DECREF(ret);
		return u_struct_copy;
	}
};


