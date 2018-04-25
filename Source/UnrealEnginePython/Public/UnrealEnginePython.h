// Copyright 20Tab S.r.l.

#pragma once

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif


//#define UEPY_MEMORY_DEBUG	1
//#define UEPY_THREADING 1

#if defined(UNREAL_ENGINE_PYTHON_ON_MAC)
#include <Headers/Python.h>
#include <Headers/structmember.h>
#elif defined(UNREAL_ENGINE_PYTHON_ON_LINUX)
#include <Python.h>
#include <structmember.h>
#else
#include <include/pyconfig.h>
#ifndef SIZEOF_PID_T
#define SIZEOF_PID_T 4
#endif
#include <include/Python.h>
#include <include/structmember.h>
#endif


#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "Styling/SlateStyle.h"
#include "UObject/ScriptMacros.h"
#include "Runtime/Launch/Resources/Version.h"

// We need to make sure reference structs do not mistaken for callable
#define PyCalllable_Check_Extended(value) PyCallable_Check(value) && py_ue_is_uscriptstruct(value) == nullptr

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		UObject *ue_object;
	// reference to proxy class (can be null)
	PyObject *py_proxy;
	// the __dict__
	PyObject *py_dict;
	// if true RemoveFromRoot will be called at object destruction time
	int auto_rooted;
} ue_PyUObject;

#if ENGINE_MINOR_VERSION >= 18
#define FStringAssetReference FSoftObjectPath
#endif

#define ue_py_check(py_u) if (!FUnrealEnginePythonHouseKeeper::Get()->IsValidPyUObject(py_u))\
	return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state")

#define ue_py_check_int(py_u) if (!FUnrealEnginePythonHouseKeeper::Get()->IsValidPyUObject(py_u))\
	{\
		PyErr_SetString(PyExc_Exception, "PyUObject is in invalid state");\
		return -1;\
	}

#if PY_MAJOR_VERSION < 3
char *PyUnicode_AsUTF8(PyObject *py_str);
int PyGILState_Check();
#endif
bool PyUnicodeOrString_Check(PyObject *py_obj);

#define Py_RETURN_UOBJECT(py_uobj) ue_PyUObject *ret = ue_get_python_uobject_inc(py_uobj);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");\
	return (PyObject *)ret;

#define Py_RETURN_UOBJECT_NOINC(py_uobj) ue_PyUObject *ret = ue_get_python_uobject(py_uobj);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");\
	return (PyObject *)ret;

#if ENGINE_MINOR_VERSION < 16
template<class CPPSTRUCT>
struct TStructOpsTypeTraitsBase2 : TStructOpsTypeTraitsBase
{

};
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogPython, Log, All);


class UNREALENGINEPYTHON_API FUnrealEnginePythonModule : public IModuleInterface
{
public:

	bool PythonGILAcquire();
	void PythonGILRelease();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RunString(char *);
	void RunStringSandboxed(char *);
	void RunFile(char *);
	void RunFileSandboxed(char *, void(*callback)(void *arg), void *arg);

	void UESetupPythonInterpreter(bool);

	FString ScriptsPath;
	FString ZipPath;
	FString AdditionalModulesPath;

	bool BrutalFinalize;

	// pep8ize a string using various strategy (currently only autopep8 is supported)
	FString Pep8ize(FString Code);

private:
	void *ue_python_gil;
	// used by console
	void *main_dict;
	void *local_dict;
	void *main_module;

	TSharedPtr<FSlateStyleSet> StyleSet;
};



struct FScopePythonGIL
{
	FScopePythonGIL()
	{
#if defined(UEPY_THREADING)
		UnrealEnginePythonModule = FModuleManager::LoadModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
		safeForRelease = UnrealEnginePythonModule.PythonGILAcquire();
#endif
	}

	~FScopePythonGIL()
	{
#if defined(UEPY_THREADING)
		if (safeForRelease)
		{
			UnrealEnginePythonModule.PythonGILRelease();
		}
#endif
	}

	FUnrealEnginePythonModule UnrealEnginePythonModule;
	bool safeForRelease;
};




