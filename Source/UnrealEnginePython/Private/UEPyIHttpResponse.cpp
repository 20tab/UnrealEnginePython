
#include "UnrealEnginePythonPrivatePCH.h"


/*
static PyObject *py_ue_ihttp_request_get_response(ue_PyIHttpRequest *self, PyObject * args) {
	FHttpResponsePtr response = self->http_request->GetResponse();
	if (!response.IsValid()) {
		return PyErr_Format(PyExc_Exception, "unable to retrieve Http Response");
	}
	return PyFloat_FromDouble(self->http_request->GetElapsedTime());
}
*/

static PyMethodDef ue_PyIHttpResponse_methods[] = {
/*	{ "append_to_header", (PyCFunction)py_ue_ihttp_request_append_to_header, METH_VARARGS, "" },
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
	{ "tick", (PyCFunction)py_ue_ihttp_request_tick, METH_VARARGS, "" },*/
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyIHttpResponse_str(ue_PyIHttpResponse *self)
{
	return PyUnicode_FromFormat("<unreal_engine.IHttpResponse {'url': '%s'}>",
		PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_response->GetURL())));
}

static PyTypeObject ue_PyIHttpResponseType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IHttpResponse", /* tp_name */
	sizeof(ue_PyIHttpResponse), /* tp_basicsize */
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
	(reprfunc)ue_PyIHttpResponse_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine HttpResponse Interface",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIHttpResponse_methods,             /* tp_methods */
	0,
	0,
};

void ue_python_init_ihttp_response(PyObject *ue_module) {
	ue_PyIHttpResponseType.tp_new = PyType_GenericNew;

	ue_PyIHttpResponseType.tp_base = &ue_PyIHttpBaseType;

	if (PyType_Ready(&ue_PyIHttpResponseType) < 0)
		return;

	Py_INCREF(&ue_PyIHttpResponseType);
	PyModule_AddObject(ue_module, "IHttpResponse", (PyObject *)&ue_PyIHttpResponseType);
}