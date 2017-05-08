
#include "UnrealEnginePythonPrivatePCH.h"


/*
static PyObject *py_ue_ihttp_request_set_verb(ue_PyIHttpRequest *self, PyObject * args) {

	char *verb;
	if (!PyArg_ParseTuple(args, "s:set_verb", &verb)) {
		return NULL;
	}

	self->http_request->SetVerb(UTF8_TO_TCHAR(verb));

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ihttp_request_set_url(ue_PyIHttpRequest *self, PyObject * args) {

	char *url;
	if (!PyArg_ParseTuple(args, "s:set_url", &url)) {
		return NULL;
	}

	self->http_request->SetURL(UTF8_TO_TCHAR(url));

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ihttp_request_set_header(ue_PyIHttpRequest *self, PyObject * args) {

	char *key;
	char *value;
	if (!PyArg_ParseTuple(args, "ss:set_header", &key, &value)) {
		return NULL;
	}

	self->http_request->SetHeader(UTF8_TO_TCHAR(key), UTF8_TO_TCHAR(value));

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ihttp_request_append_to_header(ue_PyIHttpRequest *self, PyObject * args) {

	char *key;
	char *value;
	if (!PyArg_ParseTuple(args, "ss:append_to_header", &key, &value)) {
		return NULL;
	}

	self->http_request->AppendToHeader(UTF8_TO_TCHAR(key), UTF8_TO_TCHAR(value));

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ihttp_request_set_content(ue_PyIHttpRequest *self, PyObject * args) {

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:set_content", &py_obj)) {
		return NULL;
	}

	if (PyUnicode_Check(py_obj)) {
		self->http_request->SetContentAsString(UTF8_TO_TCHAR(PyUnicode_AsUTF8(py_obj)));
	}
	else if (PyBytes_Check(py_obj)) {
		char *buf = nullptr;
		Py_ssize_t len = 0;
		PyBytes_AsStringAndSize(py_obj, &buf, &len);
		TArray<uint8> data;
		data.Append((uint8 *)buf, len);
		self->http_request->SetContent(data);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ihttp_request_tick(ue_PyIHttpRequest *self, PyObject * args) {

	float delta_seconds;
	if (!PyArg_ParseTuple(args, "f:tick", &delta_seconds)) {
		return NULL;
	}

	self->http_request->Tick(delta_seconds);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ihttp_request_process_request(ue_PyIHttpRequest *self, PyObject * args) {
	self->http_request->ProcessRequest();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_ihttp_request_cancel_request(ue_PyIHttpRequest *self, PyObject * args) {
	self->http_request->CancelRequest();

	Py_INCREF(Py_None);
	return Py_None;
}*/

static PyObject *py_ue_ihttp_base_get_content_length(ue_PyIHttpBase *self, PyObject * args) {
	return PyLong_FromLong((int)self->http_base->GetContentLength());
}



static PyMethodDef ue_PyIHttpBase_methods[] = {
	//{ "get_content", (PyCFunction)py_ue_ihttp_base_get_content, METH_VARARGS, "" },
	//{ "get_all_headers", (PyCFunction)py_ue_ihttp_base_get_all_headers, METH_VARARGS, "" },
	{ "get_content_length", (PyCFunction)py_ue_ihttp_base_get_content_length, METH_VARARGS, "" },
	//{ "get_content_type", (PyCFunction)py_ue_ihttp_base_get_content_type, METH_VARARGS, "" },
	//{ "get_header", (PyCFunction)py_ue_ihttp_base_get_header, METH_VARARGS, "" },
	//{ "get_url", (PyCFunction)py_ue_ihttp_base_get_url, METH_VARARGS, "" },
	//{ "get_url_parameter", (PyCFunction)py_ue_ihttp_base_get_url_parameter, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyIHttpBase_str(ue_PyIHttpBase *self)
{
	return PyUnicode_FromFormat("<unreal_engine.IHttpBase {'url': '%s'}>",
		PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_base->GetURL())));
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


void ue_python_init_ihttp_base(PyObject *ue_module) {
	ue_PyIHttpBaseType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyIHttpBaseType) < 0)
		return;

	Py_INCREF(&ue_PyIHttpBaseType);
	PyModule_AddObject(ue_module, "IHttpBase", (PyObject *)&ue_PyIHttpBaseType);
}