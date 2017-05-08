
#include "UnrealEnginePythonPrivatePCH.h"

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
}

static PyObject *py_ue_ihttp_request_get_status(ue_PyIHttpRequest *self, PyObject * args) {
	return PyLong_FromLong((int)self->http_request->GetStatus());
}

static PyObject *py_ue_ihttp_request_get_verb(ue_PyIHttpRequest *self, PyObject * args) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_request->GetVerb()));
}

static PyObject *py_ue_ihttp_request_get_elapsed_time(ue_PyIHttpRequest *self, PyObject * args) {
	return PyFloat_FromDouble(self->http_request->GetElapsedTime());
}

static PyObject *py_ue_ihttp_request_get_response(ue_PyIHttpRequest *self, PyObject * args) {
	FHttpResponsePtr response = self->http_request->GetResponse();
	if (!response.IsValid()) {
		return PyErr_Format(PyExc_Exception, "unable to retrieve IHttpResponse");
	}
	return py_ue_new_ihttp_response(response);
}

static PyMethodDef ue_PyIHttpRequest_methods[] = {
	{ "append_to_header", (PyCFunction)py_ue_ihttp_request_append_to_header, METH_VARARGS, "" },
	{ "cancel_request", (PyCFunction)py_ue_ihttp_request_cancel_request, METH_VARARGS, "" },
	{ "get_elapsed_time", (PyCFunction)py_ue_ihttp_request_get_elapsed_time, METH_VARARGS, "" },
	{ "get_response", (PyCFunction)py_ue_ihttp_request_get_response, METH_VARARGS, "" },
	{ "get_status", (PyCFunction)py_ue_ihttp_request_get_status, METH_VARARGS, "" },
	{ "get_verb", (PyCFunction)py_ue_ihttp_request_get_verb, METH_VARARGS, "" },
	{ "process_request", (PyCFunction)py_ue_ihttp_request_process_request, METH_VARARGS, "" },
	{ "set_content", (PyCFunction)py_ue_ihttp_request_set_content, METH_VARARGS, "" },
	{ "set_header", (PyCFunction)py_ue_ihttp_request_set_header, METH_VARARGS, "" },
	{ "set_content", (PyCFunction)py_ue_ihttp_request_set_content, METH_VARARGS, "" },
	{ "set_url", (PyCFunction)py_ue_ihttp_request_set_url, METH_VARARGS, "" },
	{ "set_verb", (PyCFunction)py_ue_ihttp_request_set_verb, METH_VARARGS, "" },
	{ "tick", (PyCFunction)py_ue_ihttp_request_tick, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyIHttpRequest_str(ue_PyIHttpRequest *self)
{
	char *s = (char*)"";
	FString url = self->http_request->GetURL();
	if (!url.IsEmpty()) {
		s = TCHAR_TO_UTF8(*url);
	}
	return PyUnicode_FromFormat("<unreal_engine.IHttpRequest {'url': '%s'}>",
		PyUnicode_FromString(s));
}

static PyTypeObject ue_PyIHttpRequestType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IHttpRequest", /* tp_name */
	sizeof(ue_PyIHttpRequest), /* tp_basicsize */
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
	(reprfunc)ue_PyIHttpRequest_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine HttpRequest Interface",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIHttpRequest_methods,             /* tp_methods */
	0,
	0,
};

static int ue_py_ihttp_request_init(ue_PyIHttpRequest *self, PyObject *args, PyObject *kwargs) {
	char *verb = nullptr;
	char* url = nullptr;
	if (!PyArg_ParseTuple(args, "|ss:__init__", &verb, &url)) {
		return -1;
	}

	self->http_request = FHttpModule::Get().CreateRequest();
	if (verb) {
		self->http_request->SetVerb(UTF8_TO_TCHAR(verb));
	}

	if (url) {
		self->http_request->SetURL(UTF8_TO_TCHAR(url));
	}

	self->base.http_base = self->http_request;
	return 0;
}

void ue_python_init_ihttp_request(PyObject *ue_module) {
	ue_PyIHttpRequestType.tp_new = PyType_GenericNew;

	ue_PyIHttpRequestType.tp_init = (initproc)ue_py_ihttp_request_init;

	ue_PyIHttpRequestType.tp_base = &ue_PyIHttpBaseType;

	if (PyType_Ready(&ue_PyIHttpRequestType) < 0)
		return;

	Py_INCREF(&ue_PyIHttpRequestType);
	PyModule_AddObject(ue_module, "IHttpRequest", (PyObject *)&ue_PyIHttpRequestType);
}