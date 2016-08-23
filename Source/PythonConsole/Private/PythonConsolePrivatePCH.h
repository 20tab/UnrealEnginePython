// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UnrealEd.h"
#include "PythonConsoleModule.h"
#include "SlateBasics.h"

#if UNREAL_ENGINE_PYTHON_ON_MAC
#include <python3.5m/Python.h>
#else
#include <include/Python.h>
#endif
