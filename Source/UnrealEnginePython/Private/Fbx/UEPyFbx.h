#pragma once

#if ENGINE_MINOR_VERSION > 12
#include "UEPyModule.h"

#if WITH_EDITOR

#include "UEPyFbxManager.h"
#include "UEPyFbxIOSettings.h"
#include "UEPyFbxImporter.h"
#include "UEPyFbxScene.h"
#include "UEPyFbxNode.h"
#include "UEPyFbxObject.h"
#include "UEPyFbxProperty.h"
#include "UEPyFbxPose.h"
#include "UEPyFbxMesh.h"

void ue_python_init_fbx(PyObject *);

#endif
#endif
