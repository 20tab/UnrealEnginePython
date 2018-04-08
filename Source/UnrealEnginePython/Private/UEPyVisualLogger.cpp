#include "UEPyVisualLogger.h"

#include "Runtime/Engine/Public/VisualLogger/VisualLogger.h"

PyObject *py_ue_vlog(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *category;
	char *text;
	int verbosity = ELogVerbosity::Log;

	if (!PyArg_ParseTuple(args, "ss|i:vlog", &category, &text, &verbosity))
		return nullptr;
#if ENABLE_VISUAL_LOG
	if (FVisualLogger::IsRecording())
	{
		FVisualLogger::CategorizedLogf(self->ue_object,
			FLogCategoryBase(UTF8_TO_TCHAR(category), (ELogVerbosity::Type)verbosity, (ELogVerbosity::Type)verbosity),
			(ELogVerbosity::Type)verbosity, TEXT("%s"), UTF8_TO_TCHAR(text));
	}
#endif
	Py_RETURN_NONE;
}

PyObject *py_ue_vlog_cylinder(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	char *category;
	PyObject *py_start;
	PyObject *py_end;
	float radius;

	PyObject *py_color = nullptr;
	char *text = (char*)"";
	int verbosity = ELogVerbosity::Log;

	if (!PyArg_ParseTuple(args, "sOOf|Osi:vlog_cylinder", &category, &py_start, &py_end, &radius, &py_color, &text, &verbosity))
		return nullptr;

	ue_PyFVector *py_vec_start = py_ue_is_fvector(py_start);
	if (!py_vec_start)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");

	ue_PyFVector *py_vec_end = py_ue_is_fvector(py_end);
	if (!py_vec_end)
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");

	FColor color = FColor::White;

	if (py_color)
	{
		ue_PyFColor *py_fcolor = py_ue_is_fcolor(py_color);
		if (!py_fcolor)
			return PyErr_Format(PyExc_Exception, "argument is not a FColor");
		color = py_fcolor->color;
	}

#if ENABLE_VISUAL_LOG
	if (FVisualLogger::IsRecording())
	{
		FVisualLogger::GeometryShapeLogf(self->ue_object,
			FLogCategoryBase(UTF8_TO_TCHAR(category), (ELogVerbosity::Type)verbosity, (ELogVerbosity::Type)verbosity),
			(ELogVerbosity::Type)verbosity,
			py_vec_start->vec, py_vec_end->vec, radius, color,
			TEXT("%s"), UTF8_TO_TCHAR(text));
	}
#endif
	Py_RETURN_NONE;
}


