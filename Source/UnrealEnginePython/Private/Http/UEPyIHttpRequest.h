#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"


extern PyTypeObject ue_PyIHttpBaseType;

typedef struct {
	ue_PyIHttpBase base;
	/* Type-specific fields go here. */
	TSharedRef<IHttpRequest> http_request;
	PyObject *py_dict;
} ue_PyIHttpRequest;


void ue_python_init_ihttp_request(PyObject *);

class FPythonSmartHttpDelegate : public FPythonSmartDelegate
{

public:
	void OnRequestComplete(FHttpRequestPtr request, FHttpResponsePtr response, bool successful);
	void OnRequestProgress(FHttpRequestPtr request, int32 sent, int32 received);

	void SetPyHttpRequest(ue_PyIHttpRequest *request) {
		py_http_request = request;
		Py_INCREF(py_http_request);
	}

	~FPythonSmartHttpDelegate() {
		Py_XDECREF(py_http_request);
	}
protected:
	ue_PyIHttpRequest *py_http_request;
};
