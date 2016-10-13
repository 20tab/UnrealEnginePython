#pragma once

#include "UnrealEnginePython.h"
#include "PythonDelegate.h"
#include <map>
#include <list>

//#include "UEPyModule.generated.h"

typedef struct {
	PyObject_HEAD
		/* Type-specific fields go here. */
		UObject *ue_object;
		// reference to proxy class (can be null)
		PyObject *py_proxy;
		// list of exposed delegates
		std::list<UPythonDelegate*> *python_delegates_gc;
		// the __dict__
		PyObject *py_dict;
} ue_PyUObject;


void unreal_engine_py_log_error();
ue_PyUObject *ue_get_python_wrapper(UObject *);
UWorld *ue_get_uworld(ue_PyUObject *);
AActor *ue_get_actor(ue_PyUObject *);
PyObject *ue_py_convert_property(UProperty *, uint8 *);
bool ue_py_convert_pyobject(PyObject *, UProperty *, uint8 *);
ue_PyUObject *ue_is_pyuobject(PyObject *);

void ue_autobind_events_for_pyclass(ue_PyUObject *, PyObject *);
PyObject *ue_bind_pyevent(ue_PyUObject *, FString, PyObject *, bool);

PyObject *py_ue_ufunction_call(UFunction *, UObject *, PyObject *, int, PyObject *);

void ue_pydelegates_cleanup(ue_PyUObject *);

UClass *unreal_engine_new_uclass(char *, UClass *);
UFunction *unreal_engine_add_function(UClass *, char *, PyObject *, uint32);
