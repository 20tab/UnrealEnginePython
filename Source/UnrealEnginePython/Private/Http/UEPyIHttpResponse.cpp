
#include "UEPyIHttpResponse.h"



static PyObject *py_ue_ihttp_response_get_response_code(ue_PyIHttpResponse *self, PyObject * args)
{
	return PyLong_FromLong(self->http_response->GetResponseCode());
}

static PyObject *py_ue_ihttp_response_get_content_as_string(ue_PyIHttpResponse *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->http_response->GetContentAsString()));
}

static PyMethodDef ue_PyIHttpResponse_methods[] = {
		{ "get_response_code", (PyCFunction)py_ue_ihttp_response_get_response_code, METH_VARARGS, "" },
		{ "get_content_as_string", (PyCFunction)py_ue_ihttp_response_get_content_as_string, METH_VARARGS, "" },
		{ NULL }  /* Sentinel */
};


static PyObject *ue_PyIHttpResponse_str(ue_PyIHttpResponse *self)
{
	return PyUnicode_FromFormat("<unreal_engine.IHttpResponse '%p'>",
		self->http_response);
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

void ue_python_init_ihttp_response(PyObject *ue_module)
{
	ue_PyIHttpResponseType.tp_new = PyType_GenericNew;

	ue_PyIHttpResponseType.tp_base = &ue_PyIHttpBaseType;

	if (PyType_Ready(&ue_PyIHttpResponseType) < 0)
		return;

	Py_INCREF(&ue_PyIHttpResponseType);
	PyModule_AddObject(ue_module, "IHttpResponse", (PyObject *)&ue_PyIHttpResponseType);
}

PyObject *py_ue_new_ihttp_response(IHttpResponse *response)
{
	ue_PyIHttpResponse *ret = (ue_PyIHttpResponse *)PyObject_New(ue_PyIHttpResponse, &ue_PyIHttpResponseType);
	ret->http_response = response;
	ret->base.http_base = response;
	return (PyObject *)ret;
}