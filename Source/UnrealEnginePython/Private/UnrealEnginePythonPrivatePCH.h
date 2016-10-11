// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

//#define UEPY_MEMORY_DEBUG	1
//#define UEPY_THREADING 1

#include "UnrealEnginePython.h"

// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.

#include "Engine.h"
#if WITH_EDITOR
#include "Editor.h"
#endif


#if UNREAL_ENGINE_PYTHON_ON_MAC == 3
#include <python3.5m/Python.h>
#include <python3.5m/structmember.h>
#elif UNREAL_ENGINE_PYTHON_ON_MAC == 2
#include <python2.7/Python.h>
#include <python2.7/structmember.h>
#elif UNREAL_ENGINE_PYTHON_ON_LINUX
#include <Python.h>
#include <structmember.h>
#else
#include <include/Python.h>
#include <include/structmember.h>
#endif

#include "UEPyModule.h"
#include "UEPyFVector.h"
#include "UEPyFHitResult.h"
#include "UEPyFRotator.h"
#include "UEPyFTransform.h"
#include "UEPyFColor.h"
#include "UEPyFLinearColor.h"

#include "UEPyCallable.h"
#include "UEPyUClassesImporter.h"
#include "UEPyEnumsImporter.h"

#if WITH_EDITOR
#include "UEPyFARFilter.h"
#endif

#define ue_py_check(py_u) if (!py_u->ue_object || !py_u->ue_object->IsValidLowLevel() || py_u->ue_object->IsPendingKillOrUnreachable())\
							return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state")

#if PY_MAJOR_VERSION < 3
char *PyUnicode_AsUTF8(PyObject *py_str);
#endif
bool PyUnicodeOrString_Check(PyObject *py_obj);
