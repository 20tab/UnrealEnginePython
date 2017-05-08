#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"

struct ue_PyIHttpBaseType;

typedef struct {
	ue_PyIHttpBase base;
	/* Type-specific fields go here. */
	TSharedRef<IHttpResponse> http_response;
} ue_PyIHttpResponse;


void ue_python_init_ihttp_response(PyObject *);
