#pragma once


#include "PythonDelegate.generated.h"

UCLASS()
class UPythonDelegate : public UObject
{
	GENERATED_BODY()

public:
	UPythonDelegate();
	~UPythonDelegate();
	virtual void ProcessEvent(UFunction *function, void *Parms) override;
	void SetPyCallable(PyObject *callable);
	void SetSignature(UFunction *original_signature);

	void PyInputHandler();
	void PyInputAxisHandler(float value);

	bool Tick(float DeltaTime);

#if WITH_EDITOR
	void PyFOnAssetPostImport(UFactory *factory, UObject *u_object);
#endif

protected:
	UFunction *signature;
	bool signature_set;

	UFUNCTION()
	void PyFakeCallable();

	PyObject *py_callable;

	
};

