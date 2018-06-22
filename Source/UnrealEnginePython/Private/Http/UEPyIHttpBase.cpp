
#include "UEPyIHttpBase.h"


static PyObject *py_ue_ihttp_base_get_content_length(ue_PyIHttpBase *self, PyObject * args)
{
	return PyLong_FromLong((int)self->http_base->GetContentLength());
}

static PyObject *py_ue_ihttp_base_get_url(ue_PyIHttpBase *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_base->GetURL()));
}

static PyObject *py_ue_ihttp_base_get_content_type(ue_PyIHttpBase *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_base->GetContentType()));
}

static PyObject *py_ue_ihttp_base_get_content(ue_PyIHttpBase *self, PyObject * args)
{
	TArray<uint8> data = self->http_base->GetContent();
	return PyBytes_FromStringAndSize((char *)data.GetData(), data.Num());
}

static PyObject *py_ue_ihttp_base_get_header(ue_PyIHttpBase *self, PyObject * args)
{

	char *key;
	if (!PyArg_ParseTuple(args, "s:get_header", &key))
	{
		return NULL;
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_base->GetHeader(UTF8_TO_TCHAR(key))));
}

static PyObject *py_ue_ihttp_base_get_url_parameter(ue_PyIHttpBase *self, PyObject * args)
{

	char *key;
	if (!PyArg_ParseTuple(args, "s:get_url_parameter", &key))
	{
		return NULL;
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_base->GetURLParameter(UTF8_TO_TCHAR(key))));
}

static PyObject *py_ue_ihttp_base_get_all_headers(ue_PyIHttpBase *self, PyObject * args)
{
	TArray<FString> headers = self->http_base->GetAllHeaders();
	PyObject *py_headers = PyList_New(0);
	for (FString item : headers)
	{
		PyObject *py_header = PyUnicode_FromString(TCHAR_TO_UTF8(*item));
		PyList_Append(py_headers, py_header);
		Py_DECREF(py_header);
	}
	return py_headers;
}

static PyMethodDef ue_PyIHttpBase_methods[] = {
	{ "get_content", (PyCFunction)py_ue_ihttp_base_get_content, METH_VARARGS, "" },
	{ "get_all_headers", (PyCFunction)py_ue_ihttp_base_get_all_headers, METH_VARARGS, "" },
	{ "get_content_length", (PyCFunction)py_ue_ihttp_base_get_content_length, METH_VARARGS, "" },
	{ "get_content_type", (PyCFunction)py_ue_ihttp_base_get_content_type, METH_VARARGS, "" },
	{ "get_header", (PyCFunction)py_ue_ihttp_base_get_header, METH_VARARGS, "" },
	{ "get_url", (PyCFunction)py_ue_ihttp_base_get_url, METH_VARARGS, "" },
	{ "get_url_parameter", (PyCFunction)py_ue_ihttp_base_get_url_parameter, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyIHttpBase_str(ue_PyIHttpBase *self)
{
	return PyUnicode_FromFormat("<unreal_engine.IHttpBase '%p'>",
		self->http_base);
}

PyTypeObject ue_PyIHttpBaseType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IHttpBase", /* tp_name */
	sizeof(ue_PyIHttpBase), /* tp_basicsize */
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
	(reprfunc)ue_PyIHttpBase_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine HttpBase Interface",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIHttpBase_methods,             /* tp_methods */
	0,
	0,
};


void ue_python_init_ihttp_base(PyObject *ue_module)
{
	ue_PyIHttpBaseType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyIHttpBaseType) < 0)
		return;

	Py_INCREF(&ue_PyIHttpBaseType);
	PyModule_AddObject(ue_module, "IHttpBase", (PyObject *)&ue_PyIHttpBaseType);
}
