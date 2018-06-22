#pragma once

#include "UEPyModule.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"

#include "UEPyIHttpBase.h"

extern PyTypeObject ue_PyIHttpBaseType;

typedef struct
{
	ue_PyIHttpBase base;
	/* Type-specific fields go here. */
	IHttpResponse *http_response;
} ue_PyIHttpResponse;


void ue_python_init_ihttp_response(PyObject *);
PyObject *py_ue_new_ihttp_response(IHttpResponse *);