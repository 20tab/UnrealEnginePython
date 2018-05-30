#pragma once

#include "UEPySlate.h"

#include "Runtime/Slate/Public/Framework/Commands/UIAction.h"
#if WITH_EDITOR
#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Developer/AssetTools/Public/IAssetTools.h"
#endif

#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FMenuBuilder menu_builder;
} ue_PyFMenuBuilder;

void ue_python_init_fmenu_builder(PyObject *);

PyObject *py_ue_new_fmenu_builder(FMenuBuilder);
