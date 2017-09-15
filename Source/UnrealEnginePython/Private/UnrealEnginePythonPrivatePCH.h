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

#include "Runtime/Launch/Resources/Version.h"


#if defined(UNREAL_ENGINE_PYTHON_ON_MAC)
#include <Headers/Python.h>
#include <Headers/structmember.h>
#elif defined(UNREAL_ENGINE_PYTHON_ON_LINUX)
#include <Python.h>
#include <structmember.h>
#else
#include <include/Python.h>
#include <include/structmember.h>
#endif

#include "UEPyModule.h"

#include "Wrappers/UEPyFVector.h"
#include "Wrappers/UEPyFHitResult.h"
#include "Wrappers/UEPyFRotator.h"
#include "Wrappers/UEPyFTransform.h"
#include "Wrappers/UEPyFColor.h"
#include "Wrappers/UEPyFLinearColor.h"
#include "Wrappers/UEPyFSocket.h"
#include "Wrappers/UEPyFQuat.h"

#include "Wrappers/UEPyFRawAnimSequenceTrack.h"

#include "Wrappers/UEPyFRandomStream.h"

#include "Wrappers/UEPyFPythonOutputDevice.h"
#include "Wrappers/UEPyFSoftSkinVertex.h"
#include "Wrappers/UEPyFMorphTargetDelta.h"

#include "UEPyCallable.h"
#include "UEPyUClassesImporter.h"
#include "UEPyEnumsImporter.h"
#include "UEPyUStructsImporter.h"

#include "UEPyUScriptStruct.h"

#if WITH_EDITOR
#include "Wrappers/UEPyFAssetData.h"
#include "Wrappers/UEPyFARFilter.h"
#include "Wrappers/UEPyFRawMesh.h"
#include "Wrappers/UEPyFStringAssetReference.h"
#include "UObject/UEPyAnimSequence.h"
#include "Blueprint/UEPyEdGraphPin.h"
#include "UEPyIPlugin.h"
#endif

#include "Slate/UEPySlate.h"
#include "Http/UEPyIHttp.h"
#include "ConsoleManager/UEPyIConsoleManager.h"
#include "Voice/UEPyIVoiceCapture.h"


#define ue_py_check(py_u) if (!py_u->ue_object || !py_u->ue_object->IsValidLowLevel() || py_u->ue_object->IsPendingKillOrUnreachable())\
							return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state")

#if PY_MAJOR_VERSION < 3
char *PyUnicode_AsUTF8(PyObject *py_str);
int PyGILState_Check();
#endif
bool PyUnicodeOrString_Check(PyObject *py_obj);
