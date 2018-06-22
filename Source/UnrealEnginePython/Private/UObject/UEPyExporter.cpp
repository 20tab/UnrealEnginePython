#include "UEPyExporter.h"


#include "Exporters/Exporter.h"


PyObject *py_ue_export_to_file(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_object;
	char *filename;

	if (!PyArg_ParseTuple(args, "Os:export_to_file", &py_object, &filename))
	{
		return nullptr;
	}

	UExporter *Exporter = ue_py_check_type<UExporter>(self);
	if (!Exporter)
	{
		return PyErr_Format(PyExc_Exception, "uobject is not a UExporter");
	}

	UObject *Object = ue_py_check_type<UObject>(py_object);
	if (!Object)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	if (UExporter::ExportToFile(Object, Exporter, UTF8_TO_TCHAR(filename), false, false, false) > 0)
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}


