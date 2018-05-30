#pragma once

#include "UEPyModule.h"

#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpBase.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	IHttpBase *http_base;
} ue_PyIHttpBase;


void ue_python_init_ihttp_base(PyObject *);
