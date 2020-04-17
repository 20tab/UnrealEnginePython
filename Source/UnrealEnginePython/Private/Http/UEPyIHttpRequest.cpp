
#include "UEPyIHttpRequest.h"

#include "UEPyIHttpResponse.h"

#include "Runtime/Online/HTTP/Public/HttpManager.h"

static PyObject *py_ue_ihttp_request_set_verb(ue_PyIHttpRequest *self, PyObject * args)
{

	char *verb;
	if (!PyArg_ParseTuple(args, "s:set_verb", &verb))
	{
		return NULL;
	}

	self->http_request->SetVerb(UTF8_TO_TCHAR(verb));

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_set_url(ue_PyIHttpRequest *self, PyObject * args)
{

	char *url;
	if (!PyArg_ParseTuple(args, "s:set_url", &url))
	{
		return NULL;
	}

	self->http_request->SetURL(UTF8_TO_TCHAR(url));

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_set_header(ue_PyIHttpRequest *self, PyObject * args)
{

	char *key;
	char *value;
	if (!PyArg_ParseTuple(args, "ss:set_header", &key, &value))
	{
		return NULL;
	}

	self->http_request->SetHeader(UTF8_TO_TCHAR(key), UTF8_TO_TCHAR(value));

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_append_to_header(ue_PyIHttpRequest *self, PyObject * args)
{

	char *key;
	char *value;
	if (!PyArg_ParseTuple(args, "ss:append_to_header", &key, &value))
	{
		return NULL;
	}

	self->http_request->AppendToHeader(UTF8_TO_TCHAR(key), UTF8_TO_TCHAR(value));

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_set_content(ue_PyIHttpRequest *self, PyObject * args)
{

	PyObject *py_obj;
	if (!PyArg_ParseTuple(args, "O:set_content", &py_obj))
	{
		return NULL;
	}

	if (PyUnicodeOrString_Check(py_obj))
	{
		self->http_request->SetContentAsString(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(py_obj)));
	}
	else if (PyBytes_Check(py_obj))
	{
		char *buf = nullptr;
		Py_ssize_t len = 0;
		PyBytes_AsStringAndSize(py_obj, &buf, &len);
		TArray<uint8> data;
		data.Append((uint8 *)buf, len);
		self->http_request->SetContent(data);
	}

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_tick(ue_PyIHttpRequest *self, PyObject * args)
{

	float delta_seconds;
	if (!PyArg_ParseTuple(args, "f:tick", &delta_seconds))
	{
		return NULL;
	}

	FHttpModule::Get().GetHttpManager().Tick(delta_seconds);

	self->http_request->Tick(delta_seconds);

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_process_request(ue_PyIHttpRequest *self, PyObject * args)
{
	self->http_request->ProcessRequest();

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_cancel_request(ue_PyIHttpRequest *self, PyObject * args)
{
	self->http_request->CancelRequest();

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_get_status(ue_PyIHttpRequest *self, PyObject * args)
{
	return PyLong_FromLong((int)self->http_request->GetStatus());
}

static PyObject *py_ue_ihttp_request_get_verb(ue_PyIHttpRequest *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_request->GetVerb()));
}

static PyObject *py_ue_ihttp_request_get_elapsed_time(ue_PyIHttpRequest *self, PyObject * args)
{
	return PyFloat_FromDouble(self->http_request->GetElapsedTime());
}

static PyObject *py_ue_ihttp_request_get_response(ue_PyIHttpRequest *self, PyObject * args)
{
	FHttpResponsePtr response = self->http_request->GetResponse();
	if (!response.IsValid())
	{
		return PyErr_Format(PyExc_Exception, "unable to retrieve IHttpResponse");
	}
	return py_ue_new_ihttp_response(response.Get());
}

void FPythonSmartHttpDelegate::OnRequestComplete(FHttpRequestPtr request, FHttpResponsePtr response, bool successful)
{
	FScopePythonGIL gil;

	if (!request.IsValid() || !response.IsValid())
	{
		UE_LOG(LogPython, Error, TEXT("Unable to retrieve HTTP infos"));
		return;
	}

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"ONO", py_http_request, py_ue_new_ihttp_response(response.Get()), successful ? Py_True : Py_False);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSmartHttpDelegate::OnRequestProgress(FHttpRequestPtr request, int32 sent, int32 received)
{
	FScopePythonGIL gil;

	if (!request.IsValid())
	{
		UE_LOG(LogPython, Error, TEXT("Unable to retrieve HTTP infos"));
		return;
	}

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"Oii", py_http_request, sent, received);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

static PyObject *py_ue_ihttp_request_bind_on_process_request_complete(ue_PyIHttpRequest *self, PyObject * args)
{

	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_process_request_complete", &py_callable))
	{
		return nullptr;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a callable");
	}

	TSharedRef<FPythonSmartHttpDelegate> py_delegate = MakeShareable(new FPythonSmartHttpDelegate);
	py_delegate->SetPyCallable(py_callable);
	// this trick avoids generating a new python object
	py_delegate->SetPyHttpRequest(self);
	self->http_request->OnProcessRequestComplete().BindSP(py_delegate, &FPythonSmartHttpDelegate::OnRequestComplete);

	self->on_process_request_complete = py_delegate;

	Py_RETURN_NONE;
}

static PyObject *py_ue_ihttp_request_bind_on_request_progress(ue_PyIHttpRequest *self, PyObject * args)
{

	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_request_progress", &py_callable))
	{
		return nullptr;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a callable");
	}

	TSharedRef<FPythonSmartHttpDelegate> py_delegate = MakeShareable(new FPythonSmartHttpDelegate);
	py_delegate->SetPyCallable(py_callable);
	// this trick avoids generating a new python object
	py_delegate->SetPyHttpRequest(self);
	self->http_request->OnRequestProgress().BindSP(py_delegate, &FPythonSmartHttpDelegate::OnRequestProgress);

	Py_RETURN_NONE;
}

static PyMethodDef ue_PyIHttpRequest_methods[] = {
	{ "bind_on_process_request_complete", (PyCFunction)py_ue_ihttp_request_bind_on_process_request_complete, METH_VARARGS, "" },
	{ "bind_on_request_progress", (PyCFunction)py_ue_ihttp_request_bind_on_request_progress, METH_VARARGS, "" },
	{ "append_to_header", (PyCFunction)py_ue_ihttp_request_append_to_header, METH_VARARGS, "" },
	{ "cancel_request", (PyCFunction)py_ue_ihttp_request_cancel_request, METH_VARARGS, "" },
	{ "get_elapsed_time", (PyCFunction)py_ue_ihttp_request_get_elapsed_time, METH_VARARGS, "" },
	{ "get_response", (PyCFunction)py_ue_ihttp_request_get_response, METH_VARARGS, "" },
	{ "get_status", (PyCFunction)py_ue_ihttp_request_get_status, METH_VARARGS, "" },
	{ "get_verb", (PyCFunction)py_ue_ihttp_request_get_verb, METH_VARARGS, "" },
	{ "process_request", (PyCFunction)py_ue_ihttp_request_process_request, METH_VARARGS, "" },
	{ "set_content", (PyCFunction)py_ue_ihttp_request_set_content, METH_VARARGS, "" },
	{ "set_header", (PyCFunction)py_ue_ihttp_request_set_header, METH_VARARGS, "" },
	{ "set_url", (PyCFunction)py_ue_ihttp_request_set_url, METH_VARARGS, "" },
	{ "set_verb", (PyCFunction)py_ue_ihttp_request_set_verb, METH_VARARGS, "" },
	{ "tick", (PyCFunction)py_ue_ihttp_request_tick, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyIHttpRequest_str(ue_PyIHttpRequest *self)
{
	return PyUnicode_FromFormat("<unreal_engine.IHttpRequest '%p'>",
		&self->http_request.Get());
}

static void ue_PyIHttpRequest_dealloc(ue_PyIHttpRequest *self)
{
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyIHttpRequest %p mapped to IHttpRequest %p"), self, &self->http_request.Get());
#endif
	self->on_process_request_complete = nullptr;
	self->on_request_progress = nullptr;
	Py_DECREF(self->py_dict);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject ue_PyIHttpRequestType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IHttpRequest", /* tp_name */
	sizeof(ue_PyIHttpRequest), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_PyIHttpRequest_dealloc,       /* tp_dealloc */
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

static int ue_py_ihttp_request_init(ue_PyIHttpRequest *self, PyObject *args, PyObject *kwargs)
{
	char *verb = nullptr;
	char* url = nullptr;
	if (!PyArg_ParseTuple(args, "|ss:__init__", &verb, &url))
	{
		return -1;
	}
	new(&self->http_request) TSharedRef<IHttpRequest>(FHttpModule::Get().CreateRequest());
	new(&self->on_process_request_complete) TSharedPtr<FPythonSmartHttpDelegate>(nullptr);
	new(&self->on_request_progress) TSharedPtr<FPythonSmartHttpDelegate>(nullptr);
	self->py_dict = PyDict_New();
	if (verb)
	{
		self->http_request->SetVerb(UTF8_TO_TCHAR(verb));
	}

	if (url)
	{
		self->http_request->SetURL(UTF8_TO_TCHAR(url));
	}

	self->base.http_base = &self->http_request.Get();
	return 0;
}

void ue_python_init_ihttp_request(PyObject *ue_module)
{
	ue_PyIHttpRequestType.tp_new = PyType_GenericNew;

	ue_PyIHttpRequestType.tp_init = (initproc)ue_py_ihttp_request_init;

	ue_PyIHttpRequestType.tp_base = &ue_PyIHttpBaseType;

	ue_PyIHttpRequestType.tp_getattro = PyObject_GenericGetAttr;
	ue_PyIHttpRequestType.tp_setattro = PyObject_GenericSetAttr;
	ue_PyIHttpRequestType.tp_dictoffset = offsetof(ue_PyIHttpRequest, py_dict);

	if (PyType_Ready(&ue_PyIHttpRequestType) < 0)
		return;

	Py_INCREF(&ue_PyIHttpRequestType);
	PyModule_AddObject(ue_module, "IHttpRequest", (PyObject *)&ue_PyIHttpRequestType);
}
