#pragma once

#include "UnrealEnginePython.h"

#include "UEPyModule.generated.h"



typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	UObject *ue_object;
} ue_PyUObject;

UCLASS()
class UPyObject : public UObject
{
	GENERATED_BODY()
public:
	ue_PyUObject *py_object;
};

void unreal_engine_py_log_error();
ue_PyUObject *ue_get_python_wrapper(UObject *);
UWorld *ue_get_uworld(ue_PyUObject *);
AActor *ue_get_actor(ue_PyUObject *);
PyObject *ue_py_convert_property(UProperty *, uint8 *);
bool ue_py_convert_pyobject(PyObject *, UProperty *, uint8 *);
ue_PyUObject *ue_is_pyuobject(PyObject *);