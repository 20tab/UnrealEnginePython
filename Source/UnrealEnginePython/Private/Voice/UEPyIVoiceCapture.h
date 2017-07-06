#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Online/Voice/Public/VoiceModule.h"
#include "Runtime/Online/Voice/Public/Interfaces/VoiceCapture.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	TSharedRef<IVoiceCapture> voice_capture;
} ue_PyIVoiceCapture;


void ue_python_init_ivoice_capture(PyObject *);
