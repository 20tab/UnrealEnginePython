#include "UEPyUStructsImporter.h"

static PyObject *ue_PyUStructsImporter_getattro(ue_PyUStructsImporter *self, PyObject *attr_name)
{
	PyObject *py_attr = PyObject_GenericGetAttr((PyObject *)self, attr_name);
	if (!py_attr)
	{
		if (PyUnicodeOrString_Check(attr_name))
		{
			const char *attr = UEPyUnicode_AsUTF8(attr_name);
			if (attr[0] != '_')
			{
				UScriptStruct *u_struct = FindObject<UScriptStruct>(ANY_PACKAGE, UTF8_TO_TCHAR(attr));
				if (u_struct)
				{
					// swallow old exception
					PyErr_Clear();
					Py_RETURN_UOBJECT(u_struct);
				}
			}
		}
	}
	return py_attr;
}

static PyTypeObject ue_PyUStructsImporterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UStructsImporter", /* tp_name */
	sizeof(ue_PyUStructsImporter), /* tp_basicsize */
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
	(getattrofunc)ue_PyUStructsImporter_getattro,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine UStructs Importer",           /* tp_doc */
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

void ue_python_init_ustructsimporter(PyObject *ue_module)
{
	ue_PyUStructsImporterType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyUStructsImporterType) < 0)
		return;

	Py_INCREF(&ue_PyUStructsImporterType);
	PyModule_AddObject(ue_module, "UStructsImporter", (PyObject *)&ue_PyUStructsImporterType);
}

PyObject *py_ue_new_ustructsimporter()
{
	ue_PyUStructsImporter *ret = (ue_PyUStructsImporter *)PyObject_New(ue_PyUStructsImporter, &ue_PyUStructsImporterType);
	return (PyObject *)ret;
}
