#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"

struct ue_PyIHttpBaseType;

typedef struct {
	ue_PyIHttpBase base;
	/* Type-specific fields go here. */
	FHttpResponsePtr http_response;
} ue_PyIHttpResponse;


void ue_python_init_ihttp_response(PyObject *);
PyObject *py_ue_new_ihttp_response(FHttpResponsePtr);