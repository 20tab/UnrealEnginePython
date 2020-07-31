#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR

#if ENGINE_MINOR_VERSION > 21

// This is not true - in 4.22 its in Runtime??
//#if ENGINE_MINOR_VERSION < 25
//#include "Developer/MeshDescription/Public/MeshDescription.h"
//#else
#include "Runtime/MeshDescription/Public/MeshDescription.h"
//#endif
#if ENGINE_MINOR_VERSION > 24
// this not defined in 4.22 - it does exist in 4.25
#include "Runtime/StaticMeshDescription/Public/StaticMeshAttributes.h"
#endif


struct ue_PyFMeshDescription
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FMeshDescription mesh_description;
		//FStaticMeshAttributes mesh_attributes;
};


void ue_python_init_fmesh_description(PyObject *);

PyObject *py_ue_new_fmesh_description(FMeshDescription);

#if 0

// so I added these but not clear I need them now
// leave in for the moment in case may need them later

// subcomponent wrappers


struct ue_PyFMeshVertex
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FMeshVertex mesh_vertex;
};


void ue_python_init_fmeshvertex(PyObject *);

PyObject *py_ue_new_fmeshvertex(FMeshVertex);



struct ue_PyFMeshVertexInstance
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FMeshVertexInstance mesh_vertex_instance;
};


void ue_python_init_fmeshvertexinstance(PyObject *);

PyObject *py_ue_new_fmeshvertexinstance(FMeshVertexInstance);



struct ue_PyFMeshPolygon
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FMeshPolygon mesh_polygon;
};


void ue_python_init_fmeshpolygon(PyObject *);

PyObject *py_ue_new_fmeshpolygon(FMeshPolygon);

#endif

#endif
#endif
