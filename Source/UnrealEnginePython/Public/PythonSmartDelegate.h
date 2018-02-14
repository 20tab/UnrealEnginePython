#pragma once

#include "UnrealEnginePythonPrivatePCH.h"

class FPythonSmartDelegate : public TSharedFromThis<FPythonSmartDelegate>
{

public:
	FPythonSmartDelegate();
	~FPythonSmartDelegate();

	void SetPyCallable(PyObject *callable);



	bool Tick(float DeltaTime);
	void Void();

#if WITH_EDITOR
	void PyFOnAssetPostImport(UFactory *factory, UObject *u_object);
#endif

protected:

	PyObject *py_callable;

};

