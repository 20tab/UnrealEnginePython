#include "UEPyESlateEnums.h"

#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"

#if WITH_EDITOR
#include "ISequencer.h"
#include "IDetailsView.h"
#endif
#include <ObjectMacros.h>
#include "SSplitter.h"
#include "MenuStack.h"
#include "SlateFwd.h"
#include "SDockTab.h"
#include "RHIDefinitions.h"
#include "SceneTypes.h"

static PyGetSetDef ue_PyESlateEnums_getseters[] = {
	{ NULL }  /* Sentinel */
};

static void ue_PyESlateEnums_dealloc(ue_PyESlateEnums *self)
{
#if defined(UEPY_MEMORY_DEBUG)
    UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyESlateEnums %p"), self);
#endif
    Py_DECREF(self->py_dict);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject ue_PyESlateEnumsType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.ESlateEnums", /* tp_name */
	sizeof(ue_PyESlateEnums), /* tp_basicsize */
	0,                         /* tp_itemsize */
    (destructor)ue_PyESlateEnums_dealloc,       /* tp_dealloc */
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
    0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine ESlateEnums",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,
	ue_PyESlateEnums_getseters,
};

static PyObject* ue_PyESlateEnums_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ue_PyESlateEnums *self = (ue_PyESlateEnums *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
#if defined(UEPY_MEMORY_DEBUG)
        UE_LOG(LogPython, Warning, TEXT("Allocating new %s..."), UTF8_TO_TCHAR(self->ob_base.ob_type->tp_name));
#endif
        self->py_dict = PyDict_New();
    }

    return (PyObject *)self;
}

void ue_python_init_eslate_enums(PyObject *ue_module)
{
    ue_PyESlateEnumsType.tp_new = ue_PyESlateEnums_new;

    ue_PyESlateEnumsType.tp_getattro = PyObject_GenericGetAttr;
    ue_PyESlateEnumsType.tp_setattro = PyObject_GenericSetAttr;
    ue_PyESlateEnumsType.tp_dictoffset = offsetof(ue_PyESlateEnums, py_dict);

	if (PyType_Ready(&ue_PyESlateEnumsType) < 0)
		return;

	Py_INCREF(&ue_PyESlateEnumsType);
	PyModule_AddObject(ue_module, "ESlateEnums", (PyObject *)&ue_PyESlateEnumsType);



    PyObject *unreal_engine_dict = PyModule_GetDict(ue_module);

    /*[[[cog
    import collections
    EnumDef = collections.namedtuple('EnumDef', 'name cppNameScope values')
    
    native_enums_list = [
        EnumDef(name='ESizeRule',
                cppNameScope='SSplitter::ESizeRule',
                values=[
                    'SizeToContent',
                    'FractionOfParent',
                ]),

        EnumDef(name='EUserInterfaceActionType', 
                cppNameScope='EUserInterfaceActionType::Type',
                values=[
                    'None',
                    'Button',
                    'ToggleButton',
                    'RadioButton',
                    'Check',
                    'CollapsedButton',
                ]),

        EnumDef(name='ESplitterResizeMode', 
                cppNameScope='ESplitterResizeMode::Type',
                values=[
                    'FixedPosition',
                    'FixedSize',
                    'Fill',
                ]),

        EnumDef(name='ESizingRule', 
                cppNameScope='ESizingRule',
                values=[
                    'FixedSize',
                    'Autosized',
                    'UserSized',
                ]),

        EnumDef(name='ESlideDirection', 
                cppNameScope='FPopupTransitionEffect::ESlideDirection',
                values=[
                    'None',
                    'ComboButton',
                    'TopMenu',
                    'SubMenu',
                    'TypeInPopup',
                    'ContextMenu',
                ]),

        EnumDef(name='ETabRole',
                cppNameScope='ETabRole',
                values=[
                    'MajorTab',
                    'PanelTab',
                    'NomadTab',
                    'DocumentTab',
                    'NumRoles'
                ]),

        EnumDef(name='ELoadConfigPropagationFlags',
                cppNameScope='UE4::ELoadConfigPropagationFlags',
                values=[
                    'LCPF_None',
                    'LCPF_ReadParentSections',
                    'LCPF_PropagateToChildDefaultObjects',
                    'LCPF_PropagateToInstances',
                    'LCPF_ReloadingConfigData',
                    'LCPF_PersistentFlags',
                ]),

        EnumDef(name='ESearchPreference',
                cppNameScope='FTabManager::ESearchPreference::ESearchPreference',
                values=[
                    'PreferLiveTab',
                    'RequireClosedTab',
                ]),

        EnumDef(name='ETabActivationCause',
                cppNameScope='ETabActivationCause',
                values=[
                    'UserClickedOnTab',
                    'SetDirectly',
                ]),

        EnumDef(name='EMaterialQualityLevel',
                cppNameScope='EMaterialQualityLevel',
                values=[
                'High',
                'Medium',
                'Low',
                'Num',
                ]),

        EnumDef(name='ERHIFeatureLevel',
                cppNameScope='ERHIFeatureLevel',
                values=[
                'ES2',
                'ES3_1',
                'SM4',
                'SM5',
                'Num',
                ]),

        EnumDef(name='EShaderPlatform',
                cppNameScope='EShaderPlatform',
                values=[
                'SP_PCD3D_SM5',
                'SP_OPENGL_SM4',
                'SP_PS4',
                'SP_OPENGL_PCES2',
                'SP_XBOXONE_D3D12',
                'SP_PCD3D_SM4',
                'SP_OPENGL_SM5',
                'SP_PCD3D_ES2',
                'SP_OPENGL_ES2_ANDROID',
                'SP_OPENGL_ES2_WEBGL',
                'SP_OPENGL_ES2_IOS',
                'SP_METAL',
                'SP_METAL_MRT',
                'SP_OPENGL_ES31_EXT',
                'SP_PCD3D_ES3_1',
                'SP_OPENGL_PCES3_1',
                'SP_METAL_SM5',
                'SP_VULKAN_PCES3_1',
                'SP_METAL_SM5_NOTESS',
                'SP_VULKAN_SM4',
                'SP_VULKAN_SM5',
                'SP_VULKAN_ES3_1_ANDROID',
                'SP_METAL_MACES3_1',
                'SP_METAL_MACES2',
                'SP_OPENGL_ES3_1_ANDROID',
                'SP_SWITCH',
                'SP_SWITCH_FORWARD',
                'SP_METAL_MRT_MAC',
                'SP_NumPlatforms',
                'SP_NumBits',
                ]),
                
        EnumDef(name='EDataValidationResult',
                cppNameScope='EDataValidationResult',
                values=[
                'Invalid',
                'Valid',
                'NotValidated'
                ]),

        EnumDef(name='EFieldIteratorFlags',
                cppNameScope='EFieldIteratorFlags',
                values=[
                'ExcludeSuper',
                'IncludeSuper',
                'ExcludeDeprecated',
                'IncludeDeprecated',
                'ExcludeInterfaces',
                'IncludeInterfaces',
                ]),
    ]    

    #############################################################################################
    # Editor Only Native Enums                                                                  #
    #############################################################################################

    editor_native_enums_list = [
        EnumDef(name='EEditDefaultsOnlyNodeVisibility', 
                cppNameScope='EEditDefaultsOnlyNodeVisibility',
                values=[
                    'Show',
                    'Hide',
                    'Automatic',
                ]),

        EnumDef(name='EMovieSceneDataChangeType', 
                cppNameScope='EMovieSceneDataChangeType',
                values=[
                    'TrackValueChanged',
                    'TrackValueChangedRefreshImmediately',
                    'MovieSceneStructureItemAdded',
                    'MovieSceneStructureItemRemoved',
                    'MovieSceneStructureItemsChanged',
                    'ActiveMovieSceneChanged',
                    'RefreshAllImmediately',
                    'Unknown',
                ]),

        EnumDef(name='EWidgetMode',
                cppNameScope='FWidget::EWidgetMode',
                values=[
                'WM_None',
                'WM_Translate',
                'WM_TranslateRotateZ',
                'WM_2D',
                'WM_Rotate',
                'WM_Scale',
                'WM_Max',
                ]),

        EnumDef(name='ECoordSystem',
                cppNameScope='ECoordSystem',
                values=[
                'COORD_None',
                'COORD_World',
                'COORD_Local',
                'COORD_Max',
                ]),
    ]

    def output_cpp_enums(in_enum_list):
        for enum_def in in_enum_list:
            enumVar_str = f'native_{enum_def.name}'
            cog.out(f"""
            // Enum Wrapper: {enum_def.name}
            {{
                PyObject* {enumVar_str} = PyDict_GetItemString(unreal_engine_dict, "{enum_def.name}");
                if ({enumVar_str} == nullptr)
                {{
                    {enumVar_str} = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
                    PyDict_SetItemString(unreal_engine_dict, "{enum_def.name}", (PyObject*){enumVar_str});
                }}
            
            """, dedent=True, trimblanklines=True)
        
            for enum_val in enum_def.values:
                enum_val_str = '"' + enum_val + '"'
                cog.outl(f'    PyObject_SetAttrString((PyObject*){enumVar_str}, {enum_val_str:17}, PyLong_FromLong((int){enum_def.cppNameScope}::{enum_val}));');

            cog.outl("}")
            cog.outl("")
        
    output_cpp_enums(native_enums_list)

    cog.outl("#if WITH_EDITOR")
    output_cpp_enums(editor_native_enums_list)
    cog.outl("#endif")

    ]]]*/
    // Enum Wrapper: ESizeRule
    {
        PyObject* native_ESizeRule = PyDict_GetItemString(unreal_engine_dict, "ESizeRule");
        if (native_ESizeRule == nullptr)
        {
            native_ESizeRule = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ESizeRule", (PyObject*)native_ESizeRule);
        }

        PyObject_SetAttrString((PyObject*)native_ESizeRule, "SizeToContent"  , PyLong_FromLong((int)SSplitter::ESizeRule::SizeToContent));
        PyObject_SetAttrString((PyObject*)native_ESizeRule, "FractionOfParent", PyLong_FromLong((int)SSplitter::ESizeRule::FractionOfParent));
    }

    // Enum Wrapper: EUserInterfaceActionType
    {
        PyObject* native_EUserInterfaceActionType = PyDict_GetItemString(unreal_engine_dict, "EUserInterfaceActionType");
        if (native_EUserInterfaceActionType == nullptr)
        {
            native_EUserInterfaceActionType = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EUserInterfaceActionType", (PyObject*)native_EUserInterfaceActionType);
        }

        PyObject_SetAttrString((PyObject*)native_EUserInterfaceActionType, "None"           , PyLong_FromLong((int)EUserInterfaceActionType::Type::None));
        PyObject_SetAttrString((PyObject*)native_EUserInterfaceActionType, "Button"         , PyLong_FromLong((int)EUserInterfaceActionType::Type::Button));
        PyObject_SetAttrString((PyObject*)native_EUserInterfaceActionType, "ToggleButton"   , PyLong_FromLong((int)EUserInterfaceActionType::Type::ToggleButton));
        PyObject_SetAttrString((PyObject*)native_EUserInterfaceActionType, "RadioButton"    , PyLong_FromLong((int)EUserInterfaceActionType::Type::RadioButton));
        PyObject_SetAttrString((PyObject*)native_EUserInterfaceActionType, "Check"          , PyLong_FromLong((int)EUserInterfaceActionType::Type::Check));
        PyObject_SetAttrString((PyObject*)native_EUserInterfaceActionType, "CollapsedButton", PyLong_FromLong((int)EUserInterfaceActionType::Type::CollapsedButton));
    }

    // Enum Wrapper: ESplitterResizeMode
    {
        PyObject* native_ESplitterResizeMode = PyDict_GetItemString(unreal_engine_dict, "ESplitterResizeMode");
        if (native_ESplitterResizeMode == nullptr)
        {
            native_ESplitterResizeMode = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ESplitterResizeMode", (PyObject*)native_ESplitterResizeMode);
        }

        PyObject_SetAttrString((PyObject*)native_ESplitterResizeMode, "FixedPosition"  , PyLong_FromLong((int)ESplitterResizeMode::Type::FixedPosition));
        PyObject_SetAttrString((PyObject*)native_ESplitterResizeMode, "FixedSize"      , PyLong_FromLong((int)ESplitterResizeMode::Type::FixedSize));
        PyObject_SetAttrString((PyObject*)native_ESplitterResizeMode, "Fill"           , PyLong_FromLong((int)ESplitterResizeMode::Type::Fill));
    }

    // Enum Wrapper: ESizingRule
    {
        PyObject* native_ESizingRule = PyDict_GetItemString(unreal_engine_dict, "ESizingRule");
        if (native_ESizingRule == nullptr)
        {
            native_ESizingRule = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ESizingRule", (PyObject*)native_ESizingRule);
        }

        PyObject_SetAttrString((PyObject*)native_ESizingRule, "FixedSize"      , PyLong_FromLong((int)ESizingRule::FixedSize));
        PyObject_SetAttrString((PyObject*)native_ESizingRule, "Autosized"      , PyLong_FromLong((int)ESizingRule::Autosized));
        PyObject_SetAttrString((PyObject*)native_ESizingRule, "UserSized"      , PyLong_FromLong((int)ESizingRule::UserSized));
    }

    // Enum Wrapper: ESlideDirection
    {
        PyObject* native_ESlideDirection = PyDict_GetItemString(unreal_engine_dict, "ESlideDirection");
        if (native_ESlideDirection == nullptr)
        {
            native_ESlideDirection = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ESlideDirection", (PyObject*)native_ESlideDirection);
        }

        PyObject_SetAttrString((PyObject*)native_ESlideDirection, "None"           , PyLong_FromLong((int)FPopupTransitionEffect::ESlideDirection::None));
        PyObject_SetAttrString((PyObject*)native_ESlideDirection, "ComboButton"    , PyLong_FromLong((int)FPopupTransitionEffect::ESlideDirection::ComboButton));
        PyObject_SetAttrString((PyObject*)native_ESlideDirection, "TopMenu"        , PyLong_FromLong((int)FPopupTransitionEffect::ESlideDirection::TopMenu));
        PyObject_SetAttrString((PyObject*)native_ESlideDirection, "SubMenu"        , PyLong_FromLong((int)FPopupTransitionEffect::ESlideDirection::SubMenu));
        PyObject_SetAttrString((PyObject*)native_ESlideDirection, "TypeInPopup"    , PyLong_FromLong((int)FPopupTransitionEffect::ESlideDirection::TypeInPopup));
        PyObject_SetAttrString((PyObject*)native_ESlideDirection, "ContextMenu"    , PyLong_FromLong((int)FPopupTransitionEffect::ESlideDirection::ContextMenu));
    }

    // Enum Wrapper: ETabRole
    {
        PyObject* native_ETabRole = PyDict_GetItemString(unreal_engine_dict, "ETabRole");
        if (native_ETabRole == nullptr)
        {
            native_ETabRole = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ETabRole", (PyObject*)native_ETabRole);
        }

        PyObject_SetAttrString((PyObject*)native_ETabRole, "MajorTab"       , PyLong_FromLong((int)ETabRole::MajorTab));
        PyObject_SetAttrString((PyObject*)native_ETabRole, "PanelTab"       , PyLong_FromLong((int)ETabRole::PanelTab));
        PyObject_SetAttrString((PyObject*)native_ETabRole, "NomadTab"       , PyLong_FromLong((int)ETabRole::NomadTab));
        PyObject_SetAttrString((PyObject*)native_ETabRole, "DocumentTab"    , PyLong_FromLong((int)ETabRole::DocumentTab));
        PyObject_SetAttrString((PyObject*)native_ETabRole, "NumRoles"       , PyLong_FromLong((int)ETabRole::NumRoles));
    }

    // Enum Wrapper: ELoadConfigPropagationFlags
    {
        PyObject* native_ELoadConfigPropagationFlags = PyDict_GetItemString(unreal_engine_dict, "ELoadConfigPropagationFlags");
        if (native_ELoadConfigPropagationFlags == nullptr)
        {
            native_ELoadConfigPropagationFlags = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ELoadConfigPropagationFlags", (PyObject*)native_ELoadConfigPropagationFlags);
        }

        PyObject_SetAttrString((PyObject*)native_ELoadConfigPropagationFlags, "LCPF_None"      , PyLong_FromLong((int)UE4::ELoadConfigPropagationFlags::LCPF_None));
        PyObject_SetAttrString((PyObject*)native_ELoadConfigPropagationFlags, "LCPF_ReadParentSections", PyLong_FromLong((int)UE4::ELoadConfigPropagationFlags::LCPF_ReadParentSections));
        PyObject_SetAttrString((PyObject*)native_ELoadConfigPropagationFlags, "LCPF_PropagateToChildDefaultObjects", PyLong_FromLong((int)UE4::ELoadConfigPropagationFlags::LCPF_PropagateToChildDefaultObjects));
        PyObject_SetAttrString((PyObject*)native_ELoadConfigPropagationFlags, "LCPF_PropagateToInstances", PyLong_FromLong((int)UE4::ELoadConfigPropagationFlags::LCPF_PropagateToInstances));
        PyObject_SetAttrString((PyObject*)native_ELoadConfigPropagationFlags, "LCPF_ReloadingConfigData", PyLong_FromLong((int)UE4::ELoadConfigPropagationFlags::LCPF_ReloadingConfigData));
        PyObject_SetAttrString((PyObject*)native_ELoadConfigPropagationFlags, "LCPF_PersistentFlags", PyLong_FromLong((int)UE4::ELoadConfigPropagationFlags::LCPF_PersistentFlags));
    }

    // Enum Wrapper: ESearchPreference
    {
        PyObject* native_ESearchPreference = PyDict_GetItemString(unreal_engine_dict, "ESearchPreference");
        if (native_ESearchPreference == nullptr)
        {
            native_ESearchPreference = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ESearchPreference", (PyObject*)native_ESearchPreference);
        }

        PyObject_SetAttrString((PyObject*)native_ESearchPreference, "PreferLiveTab"  , PyLong_FromLong((int)FTabManager::ESearchPreference::ESearchPreference::PreferLiveTab));
        PyObject_SetAttrString((PyObject*)native_ESearchPreference, "RequireClosedTab", PyLong_FromLong((int)FTabManager::ESearchPreference::ESearchPreference::RequireClosedTab));
    }

    // Enum Wrapper: ETabActivationCause
    {
        PyObject* native_ETabActivationCause = PyDict_GetItemString(unreal_engine_dict, "ETabActivationCause");
        if (native_ETabActivationCause == nullptr)
        {
            native_ETabActivationCause = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ETabActivationCause", (PyObject*)native_ETabActivationCause);
        }

        PyObject_SetAttrString((PyObject*)native_ETabActivationCause, "UserClickedOnTab", PyLong_FromLong((int)ETabActivationCause::UserClickedOnTab));
        PyObject_SetAttrString((PyObject*)native_ETabActivationCause, "SetDirectly"    , PyLong_FromLong((int)ETabActivationCause::SetDirectly));
    }

    // Enum Wrapper: EMaterialQualityLevel
    {
        PyObject* native_EMaterialQualityLevel = PyDict_GetItemString(unreal_engine_dict, "EMaterialQualityLevel");
        if (native_EMaterialQualityLevel == nullptr)
        {
            native_EMaterialQualityLevel = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EMaterialQualityLevel", (PyObject*)native_EMaterialQualityLevel);
        }

        PyObject_SetAttrString((PyObject*)native_EMaterialQualityLevel, "High"           , PyLong_FromLong((int)EMaterialQualityLevel::High));
        PyObject_SetAttrString((PyObject*)native_EMaterialQualityLevel, "Medium"         , PyLong_FromLong((int)EMaterialQualityLevel::Medium));
        PyObject_SetAttrString((PyObject*)native_EMaterialQualityLevel, "Low"            , PyLong_FromLong((int)EMaterialQualityLevel::Low));
        PyObject_SetAttrString((PyObject*)native_EMaterialQualityLevel, "Num"            , PyLong_FromLong((int)EMaterialQualityLevel::Num));
    }

    // Enum Wrapper: ERHIFeatureLevel
    {
        PyObject* native_ERHIFeatureLevel = PyDict_GetItemString(unreal_engine_dict, "ERHIFeatureLevel");
        if (native_ERHIFeatureLevel == nullptr)
        {
            native_ERHIFeatureLevel = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ERHIFeatureLevel", (PyObject*)native_ERHIFeatureLevel);
        }

        PyObject_SetAttrString((PyObject*)native_ERHIFeatureLevel, "ES2"            , PyLong_FromLong((int)ERHIFeatureLevel::ES2));
        PyObject_SetAttrString((PyObject*)native_ERHIFeatureLevel, "ES3_1"          , PyLong_FromLong((int)ERHIFeatureLevel::ES3_1));
        PyObject_SetAttrString((PyObject*)native_ERHIFeatureLevel, "SM4"            , PyLong_FromLong((int)ERHIFeatureLevel::SM4));
        PyObject_SetAttrString((PyObject*)native_ERHIFeatureLevel, "SM5"            , PyLong_FromLong((int)ERHIFeatureLevel::SM5));
        PyObject_SetAttrString((PyObject*)native_ERHIFeatureLevel, "Num"            , PyLong_FromLong((int)ERHIFeatureLevel::Num));
    }

    // Enum Wrapper: EShaderPlatform
    {
        PyObject* native_EShaderPlatform = PyDict_GetItemString(unreal_engine_dict, "EShaderPlatform");
        if (native_EShaderPlatform == nullptr)
        {
            native_EShaderPlatform = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EShaderPlatform", (PyObject*)native_EShaderPlatform);
        }

        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_PCD3D_SM5"   , PyLong_FromLong((int)EShaderPlatform::SP_PCD3D_SM5));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_SM4"  , PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_SM4));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_PS4"         , PyLong_FromLong((int)EShaderPlatform::SP_PS4));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_PCES2", PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_PCES2));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_XBOXONE_D3D12", PyLong_FromLong((int)EShaderPlatform::SP_XBOXONE_D3D12));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_PCD3D_SM4"   , PyLong_FromLong((int)EShaderPlatform::SP_PCD3D_SM4));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_SM5"  , PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_SM5));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_PCD3D_ES2"   , PyLong_FromLong((int)EShaderPlatform::SP_PCD3D_ES2));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_ES2_ANDROID", PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_ES2_ANDROID));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_ES2_WEBGL", PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_ES2_WEBGL));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_ES2_IOS", PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_ES2_IOS));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_METAL"       , PyLong_FromLong((int)EShaderPlatform::SP_METAL));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_METAL_MRT"   , PyLong_FromLong((int)EShaderPlatform::SP_METAL_MRT));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_ES31_EXT", PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_ES31_EXT));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_PCD3D_ES3_1" , PyLong_FromLong((int)EShaderPlatform::SP_PCD3D_ES3_1));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_PCES3_1", PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_PCES3_1));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_METAL_SM5"   , PyLong_FromLong((int)EShaderPlatform::SP_METAL_SM5));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_VULKAN_PCES3_1", PyLong_FromLong((int)EShaderPlatform::SP_VULKAN_PCES3_1));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_METAL_SM5_NOTESS", PyLong_FromLong((int)EShaderPlatform::SP_METAL_SM5_NOTESS));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_VULKAN_SM4"  , PyLong_FromLong((int)EShaderPlatform::SP_VULKAN_SM4));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_VULKAN_SM5"  , PyLong_FromLong((int)EShaderPlatform::SP_VULKAN_SM5));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_VULKAN_ES3_1_ANDROID", PyLong_FromLong((int)EShaderPlatform::SP_VULKAN_ES3_1_ANDROID));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_METAL_MACES3_1", PyLong_FromLong((int)EShaderPlatform::SP_METAL_MACES3_1));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_METAL_MACES2", PyLong_FromLong((int)EShaderPlatform::SP_METAL_MACES2));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_OPENGL_ES3_1_ANDROID", PyLong_FromLong((int)EShaderPlatform::SP_OPENGL_ES3_1_ANDROID));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_SWITCH"      , PyLong_FromLong((int)EShaderPlatform::SP_SWITCH));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_SWITCH_FORWARD", PyLong_FromLong((int)EShaderPlatform::SP_SWITCH_FORWARD));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_METAL_MRT_MAC", PyLong_FromLong((int)EShaderPlatform::SP_METAL_MRT_MAC));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_NumPlatforms", PyLong_FromLong((int)EShaderPlatform::SP_NumPlatforms));
        PyObject_SetAttrString((PyObject*)native_EShaderPlatform, "SP_NumBits"     , PyLong_FromLong((int)EShaderPlatform::SP_NumBits));
    }

    // Enum Wrapper: EDataValidationResult
    {
        PyObject* native_EDataValidationResult = PyDict_GetItemString(unreal_engine_dict, "EDataValidationResult");
        if (native_EDataValidationResult == nullptr)
        {
            native_EDataValidationResult = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EDataValidationResult", (PyObject*)native_EDataValidationResult);
        }

        PyObject_SetAttrString((PyObject*)native_EDataValidationResult, "Invalid"        , PyLong_FromLong((int)EDataValidationResult::Invalid));
        PyObject_SetAttrString((PyObject*)native_EDataValidationResult, "Valid"          , PyLong_FromLong((int)EDataValidationResult::Valid));
        PyObject_SetAttrString((PyObject*)native_EDataValidationResult, "NotValidated"   , PyLong_FromLong((int)EDataValidationResult::NotValidated));
    }

    // Enum Wrapper: EFieldIteratorFlags
    {
        PyObject* native_EFieldIteratorFlags = PyDict_GetItemString(unreal_engine_dict, "EFieldIteratorFlags");
        if (native_EFieldIteratorFlags == nullptr)
        {
            native_EFieldIteratorFlags = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EFieldIteratorFlags", (PyObject*)native_EFieldIteratorFlags);
        }

        PyObject_SetAttrString((PyObject*)native_EFieldIteratorFlags, "ExcludeSuper"   , PyLong_FromLong((int)EFieldIteratorFlags::ExcludeSuper));
        PyObject_SetAttrString((PyObject*)native_EFieldIteratorFlags, "IncludeSuper"   , PyLong_FromLong((int)EFieldIteratorFlags::IncludeSuper));
        PyObject_SetAttrString((PyObject*)native_EFieldIteratorFlags, "ExcludeDeprecated", PyLong_FromLong((int)EFieldIteratorFlags::ExcludeDeprecated));
        PyObject_SetAttrString((PyObject*)native_EFieldIteratorFlags, "IncludeDeprecated", PyLong_FromLong((int)EFieldIteratorFlags::IncludeDeprecated));
        PyObject_SetAttrString((PyObject*)native_EFieldIteratorFlags, "ExcludeInterfaces", PyLong_FromLong((int)EFieldIteratorFlags::ExcludeInterfaces));
        PyObject_SetAttrString((PyObject*)native_EFieldIteratorFlags, "IncludeInterfaces", PyLong_FromLong((int)EFieldIteratorFlags::IncludeInterfaces));
    }

    #if WITH_EDITOR
    // Enum Wrapper: EEditDefaultsOnlyNodeVisibility
    {
        PyObject* native_EEditDefaultsOnlyNodeVisibility = PyDict_GetItemString(unreal_engine_dict, "EEditDefaultsOnlyNodeVisibility");
        if (native_EEditDefaultsOnlyNodeVisibility == nullptr)
        {
            native_EEditDefaultsOnlyNodeVisibility = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EEditDefaultsOnlyNodeVisibility", (PyObject*)native_EEditDefaultsOnlyNodeVisibility);
        }

        PyObject_SetAttrString((PyObject*)native_EEditDefaultsOnlyNodeVisibility, "Show"           , PyLong_FromLong((int)EEditDefaultsOnlyNodeVisibility::Show));
        PyObject_SetAttrString((PyObject*)native_EEditDefaultsOnlyNodeVisibility, "Hide"           , PyLong_FromLong((int)EEditDefaultsOnlyNodeVisibility::Hide));
        PyObject_SetAttrString((PyObject*)native_EEditDefaultsOnlyNodeVisibility, "Automatic"      , PyLong_FromLong((int)EEditDefaultsOnlyNodeVisibility::Automatic));
    }

    // Enum Wrapper: EMovieSceneDataChangeType
    {
        PyObject* native_EMovieSceneDataChangeType = PyDict_GetItemString(unreal_engine_dict, "EMovieSceneDataChangeType");
        if (native_EMovieSceneDataChangeType == nullptr)
        {
            native_EMovieSceneDataChangeType = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EMovieSceneDataChangeType", (PyObject*)native_EMovieSceneDataChangeType);
        }

        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "TrackValueChanged", PyLong_FromLong((int)EMovieSceneDataChangeType::TrackValueChanged));
        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "TrackValueChangedRefreshImmediately", PyLong_FromLong((int)EMovieSceneDataChangeType::TrackValueChangedRefreshImmediately));
        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "MovieSceneStructureItemAdded", PyLong_FromLong((int)EMovieSceneDataChangeType::MovieSceneStructureItemAdded));
        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "MovieSceneStructureItemRemoved", PyLong_FromLong((int)EMovieSceneDataChangeType::MovieSceneStructureItemRemoved));
        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "MovieSceneStructureItemsChanged", PyLong_FromLong((int)EMovieSceneDataChangeType::MovieSceneStructureItemsChanged));
        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "ActiveMovieSceneChanged", PyLong_FromLong((int)EMovieSceneDataChangeType::ActiveMovieSceneChanged));
        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "RefreshAllImmediately", PyLong_FromLong((int)EMovieSceneDataChangeType::RefreshAllImmediately));
        PyObject_SetAttrString((PyObject*)native_EMovieSceneDataChangeType, "Unknown"        , PyLong_FromLong((int)EMovieSceneDataChangeType::Unknown));
    }

    // Enum Wrapper: EWidgetMode
    {
        PyObject* native_EWidgetMode = PyDict_GetItemString(unreal_engine_dict, "EWidgetMode");
        if (native_EWidgetMode == nullptr)
        {
            native_EWidgetMode = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "EWidgetMode", (PyObject*)native_EWidgetMode);
        }

        PyObject_SetAttrString((PyObject*)native_EWidgetMode, "WM_None"        , PyLong_FromLong((int)FWidget::EWidgetMode::WM_None));
        PyObject_SetAttrString((PyObject*)native_EWidgetMode, "WM_Translate"   , PyLong_FromLong((int)FWidget::EWidgetMode::WM_Translate));
        PyObject_SetAttrString((PyObject*)native_EWidgetMode, "WM_TranslateRotateZ", PyLong_FromLong((int)FWidget::EWidgetMode::WM_TranslateRotateZ));
        PyObject_SetAttrString((PyObject*)native_EWidgetMode, "WM_2D"          , PyLong_FromLong((int)FWidget::EWidgetMode::WM_2D));
        PyObject_SetAttrString((PyObject*)native_EWidgetMode, "WM_Rotate"      , PyLong_FromLong((int)FWidget::EWidgetMode::WM_Rotate));
        PyObject_SetAttrString((PyObject*)native_EWidgetMode, "WM_Scale"       , PyLong_FromLong((int)FWidget::EWidgetMode::WM_Scale));
        PyObject_SetAttrString((PyObject*)native_EWidgetMode, "WM_Max"         , PyLong_FromLong((int)FWidget::EWidgetMode::WM_Max));
    }

    // Enum Wrapper: ECoordSystem
    {
        PyObject* native_ECoordSystem = PyDict_GetItemString(unreal_engine_dict, "ECoordSystem");
        if (native_ECoordSystem == nullptr)
        {
            native_ECoordSystem = ue_PyESlateEnums_new(&ue_PyESlateEnumsType, nullptr, nullptr);
            PyDict_SetItemString(unreal_engine_dict, "ECoordSystem", (PyObject*)native_ECoordSystem);
        }

        PyObject_SetAttrString((PyObject*)native_ECoordSystem, "COORD_None"     , PyLong_FromLong((int)ECoordSystem::COORD_None));
        PyObject_SetAttrString((PyObject*)native_ECoordSystem, "COORD_World"    , PyLong_FromLong((int)ECoordSystem::COORD_World));
        PyObject_SetAttrString((PyObject*)native_ECoordSystem, "COORD_Local"    , PyLong_FromLong((int)ECoordSystem::COORD_Local));
        PyObject_SetAttrString((PyObject*)native_ECoordSystem, "COORD_Max"      , PyLong_FromLong((int)ECoordSystem::COORD_Max));
    }

    #endif
    //[[[end]]]
}

ue_PyESlateEnums *py_ue_is_eslate_enums(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyESlateEnumsType))
		return nullptr;
	return (ue_PyESlateEnums *)obj;

}
