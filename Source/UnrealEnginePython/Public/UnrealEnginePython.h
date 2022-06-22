// Copyright 20Tab S.r.l.

#pragma once

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif


//#define UEPY_MEMORY_DEBUG	1

//#define EXTRA_LOGGING 1

#ifdef EXTRA_LOGGING
#define EXTRA_UE_LOG(log_python, log_level, fmt, args...) \
        UE_LOG(log_python, log_level, fmt, ##args)
#define EXTRA_DEBUG_CODE 1
#else
#define EXTRA_UE_LOG(...)
#endif


#include "CoreMinimal.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyle.h"
#include "UObject/ScriptMacros.h"
#include "Runtime/Launch/Resources/Version.h"

#if PLATFORM_MAC
#include <Python.h>
#include <structmember.h>
#elif PLATFORM_LINUX
#include <Python.h>
#include <structmember.h>
#elif PLATFORM_ANDROID || PLATFORM_IOS
#include <Python.h>
#include <structmember.h>
#elif PLATFORM_WINDOWS
#include <Python3/Win64/include/pyconfig.h>
#ifndef SIZEOF_PID_T
#define SIZEOF_PID_T 4
#endif
#include <Python3/Win64/include/Python.h>
#include <Python3/Win64/include/structmember.h>
#endif

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
	// if owned the life of the UObject is related to the life of PyObject
	int owned;
} ue_PyUObject;

UNREALENGINEPYTHON_API void ue_py_register_magic_module(char *name, PyObject *(*)());
UNREALENGINEPYTHON_API PyObject *ue_py_register_module(const char *);

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 18)
#define FStringAssetReference FSoftObjectPath
#endif

#define ue_py_check(py_u) if (!FUnrealEnginePythonHouseKeeper::Get()->IsValidPyUObject(py_u))\
	return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state")

#define ue_py_check_int(py_u) if (!FUnrealEnginePythonHouseKeeper::Get()->IsValidPyUObject(py_u))\
	{\
		PyErr_SetString(PyExc_Exception, "PyUObject is in invalid state");\
		return -1;\
	}


const char *UEPyUnicode_AsUTF8(PyObject *py_str);

#if PY_MAJOR_VERSION < 3
int PyGILState_Check();
#endif
bool PyUnicodeOrString_Check(PyObject *py_obj);

UNREALENGINEPYTHON_API void unreal_engine_py_log_error();
UNREALENGINEPYTHON_API ue_PyUObject *ue_get_python_uobject(UObject *);
UNREALENGINEPYTHON_API ue_PyUObject *ue_get_python_uobject_inc(UObject *);

#define Py_RETURN_UOBJECT(py_uobj) ue_PyUObject *ret = ue_get_python_uobject_inc(py_uobj);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");\
	return (PyObject *)ret;

#define Py_RETURN_UOBJECT_NOINC(py_uobj) ue_PyUObject *ret = ue_get_python_uobject(py_uobj);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");\
	return (PyObject *)ret;

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)

typedef struct
{
	PyObject_HEAD
	/* Type-specific fields go here. */
	FProperty *ue_fproperty;
	// the __dict__
	PyObject *py_dict;
	//// if true RemoveFromRoot will be called at object destruction time
	//int auto_rooted;
	//// if owned the life of the UObject is related to the life of PyObject
	//int owned;
} ue_PyFProperty;

UNREALENGINEPYTHON_API ue_PyFProperty *ue_get_python_fproperty(FProperty *);
UNREALENGINEPYTHON_API ue_PyFProperty *ue_get_python_fproperty_inc(FProperty *);

#define Py_RETURN_FPROPERTY(py_fprop) ue_PyFProperty *ret = ue_get_python_fproperty_inc(py_fprop);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "fproperty is invalid");\
	return (PyObject *)ret;

#define Py_RETURN_FPROPERTY_NOINC(py_fprop) ue_PyFProperty *ret = ue_get_python_fproperty(py_fprop);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "fproperty is invalid");\
	return (PyObject *)ret;

// for the moment it seems I need both an FProperty wrap and an FFieldClass wrap
// not sure about this yet
// for uobjects both property and class are the same type ie uobjects which is not true for
// FProperties but havent figured the ramifications of this yet

typedef struct
{
	PyObject_HEAD
	/* Type-specific fields go here. */
	FFieldClass *ue_ffieldclass;
	// the __dict__
	PyObject *py_dict;
	//// if true RemoveFromRoot will be called at object destruction time
	//int auto_rooted;
	//// if owned the life of the UObject is related to the life of PyObject
	//int owned;
} ue_PyFFieldClass;

UNREALENGINEPYTHON_API ue_PyFFieldClass *ue_get_python_ffieldclass(FFieldClass *);
UNREALENGINEPYTHON_API ue_PyFFieldClass *ue_get_python_ffieldclass_inc(FFieldClass *);

#define Py_RETURN_FFIELDCLASS(py_ffieldclass) ue_PyFFieldClass *ret = ue_get_python_ffieldclass_inc(py_ffieldclass);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "ffieldclass is invalid");\
	return (PyObject *)ret;

#define Py_RETURN_FFIELDCLASS_NOINC(py_ffieldclass) ue_PyFFieldClass *ret = ue_get_python_ffieldclass(py_ffieldclass);\
	if (!ret)\
		return PyErr_Format(PyExc_Exception, "ffieldclass is invalid");\
	return (PyObject *)ret;

#endif

#if !(ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 16))
template<class CPPSTRUCT>
struct TStructOpsTypeTraitsBase2 : TStructOpsTypeTraitsBase
{

};
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogPython, Log, All);


class UNREALENGINEPYTHON_API FUnrealEnginePythonModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RunString(char *);
	void RunFile(char *);

#if PLATFORM_MAC
	void RunStringInMainThread(char *);
	void RunFileInMainThread(char *);
#endif

	void UESetupPythonInterpreter(bool);

	TArray<FString> ScriptsPaths;
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

	PyGILState_STATE state;

	FScopePythonGIL()
	{
		state = PyGILState_Ensure();
	}

	~FScopePythonGIL()
	{
		PyGILState_Release(state);
	}
};




