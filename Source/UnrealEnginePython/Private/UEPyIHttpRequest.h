#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"


struct ue_PyIHttpBaseType;

typedef struct {
	ue_PyIHttpBase base;
	/* Type-specific fields go here. */
	TSharedRef<IHttpRequest> http_request;
} ue_PyIHttpRequest;


void ue_python_init_ihttp_request(PyObject *);
