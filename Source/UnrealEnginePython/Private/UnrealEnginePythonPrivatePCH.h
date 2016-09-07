// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

//#define UEPY_MEMORY_DEBUG	1

#include "UnrealEnginePython.h"

// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.

#include "Engine.h"
#if WITH_EDITOR
#include "Editor.h"
#endif


#if UNREAL_ENGINE_PYTHON_ON_MAC
#include <python3.5m/Python.h>
#else
#include <include/Python.h>
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
#include "UEPyENumsImporter.h"


#define ue_py_check(py_u) if (!py_u->ue_object || !py_u->ue_object->IsValidLowLevel() || py_u->ue_object->IsPendingKillOrUnreachable())\
							return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state")

