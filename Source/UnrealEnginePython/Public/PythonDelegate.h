#pragma once


#include "PythonDelegate.generated.h"

UCLASS()
class UPythonDelegate : public UObject
{
	GENERATED_BODY()

public:
	~UPythonDelegate();
	virtual void ProcessEvent(UFunction *function, void *Parms) override;
	void SetPyCallable(PyObject *callable);
	void SetSignature(UFunction *original_signature);

	void PyInputHandler();

private:
	UFunction *signature;

	UFUNCTION()
	void PyFakeCallable();

	PyObject *py_callable;

	
};

