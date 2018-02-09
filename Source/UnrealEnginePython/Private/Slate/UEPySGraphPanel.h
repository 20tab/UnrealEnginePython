#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 15
#include "UEPySNodePanel.h"

#include "Editor/GraphEditor/Public/SGraphPanel.h"

extern PyTypeObject ue_PySGraphPanelType;

typedef struct {
    ue_PySNodePanel s_nodePanel;
    /* Type-specific fields go here. */
} ue_PySGraphPanel;


void ue_python_init_sgraph_panel(PyObject *);
#endif
#endif