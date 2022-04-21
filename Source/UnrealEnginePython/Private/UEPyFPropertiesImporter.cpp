#include "UEPyFPropertiesImporter.h"


#if ENGINE_MINOR_VERSION >= 25


static PyObject *ue_PyFPropertiesImporter_getattro(ue_PyFPropertiesImporter *self, PyObject *attr_name)
{
	EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer called"));
	PyObject *py_attr = PyObject_GenericGetAttr((PyObject *)self, attr_name);
	if (!py_attr)
	{
		EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer attr OK"));
		if (PyUnicodeOrString_Check(attr_name))
		{
			const char *attr = UEPyUnicode_AsUTF8(attr_name);
			EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer attr is string %s"), UTF8_TO_TCHAR(attr));
			if (attr[0] != '_')
			{
				// dont know what to do here 
				// no - as far as I can see we cannot search for property classes anymore
				// we have to explicitly add them
				// - so do we need this - just import the explicit property wrap
				// actually this would have to be the property class wrap

                                EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer field classes %d"), FFieldClass::GetAllFieldClasses().Num());
                                //for (const FFieldClass* FieldClass : FFieldClass::GetAllFieldClasses())
                                //{
                                //        EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer field classes %s"), *FieldClass->GetName());
                                //}
                                EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer field classes map %d"), FFieldClass::GetNameToFieldClassMap().Num());

				// so apparently there is a map of all field classes and this is how we can look it up
				FName FieldTypeStr = FName(UTF8_TO_TCHAR(attr));
				EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer looking for %s"), *FieldTypeStr.ToString());
				FFieldClass** f_class_ptr = FFieldClass::GetNameToFieldClassMap().Find(FieldTypeStr);
				if (f_class_ptr)
				{
					EXTRA_UE_LOG(LogPython, Warning, TEXT("magic properties importer found class"));
					// swallow old exception
					PyErr_Clear();
					// if we create python wrapper each time then think we dont want the extra inc ref
					// - PyObject_New has already incremented new python objects ref count
					// actually this may be OK - we only see this for import statements
					// which in general are only done once - so this wrapper will last for lifetime
					// of python script - except if we have multiple scripts we will get a different wrapper
					// in each script
					Py_RETURN_FFIELDCLASS_NOINC(*f_class_ptr);
				}

				//UClass *u_class = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(attr));
				//if (u_class)
				//{
				//	// swallow old exception
				//	PyErr_Clear();
				//	Py_RETURN_UOBJECT(u_class);
				//}

			}
		}
	}
	return py_attr;
}

static PyTypeObject ue_PyFPropertiesImporterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FPropertiesImporter", /* tp_name */
	sizeof(ue_PyFPropertiesImporter), /* tp_basicsize */
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
	0,                         /* tp_str */
	(getattrofunc)ue_PyFPropertiesImporter_getattro,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FProperties Importer",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,             /* tp_methods */
	0,
	0,
};

void ue_python_init_fpropertiesimporter(PyObject *ue_module)
{
	ue_PyFPropertiesImporterType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFPropertiesImporterType) < 0)
		return;

	Py_INCREF(&ue_PyFPropertiesImporterType);
	PyModule_AddObject(ue_module, "PropertiesImporter", (PyObject *)&ue_PyFPropertiesImporterType);
}

PyObject *py_ue_new_fpropertiesimporter()
{
	ue_PyFPropertiesImporter *ret = (ue_PyFPropertiesImporter *)PyObject_New(ue_PyFPropertiesImporter, &ue_PyFPropertiesImporterType);
	return (PyObject *)ret;
}


#endif
