#include "UnrealEnginePythonPrivatePCH.h"

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
    ]
    

    for enum_def in native_enums_list:
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

    //[[[end]]]
}

ue_PyESlateEnums *py_ue_is_eslate_enums(PyObject *obj)
{
    if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyESlateEnumsType))
        return nullptr;
    return (ue_PyESlateEnums *)obj;

}
