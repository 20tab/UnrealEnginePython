#include "UnrealEnginePythonPrivatePCH.h"
#include "Runtime/Landscape/Classes/LandscapeProxy.h"
#include "Runtime/Landscape/Classes/LandscapeInfo.h"

#if WITH_EDITOR
PyObject *py_ue_create_landscape_info(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ALandscapeProxy *landscape = ue_py_check_type<ALandscapeProxy>(self);
	if (!landscape)
		return PyErr_Format(PyExc_Exception, "uobject is not a ULandscapeProxy");

	ue_PyUObject *ret = ue_get_python_wrapper(landscape->CreateLandscapeInfo());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_get_landscape_info(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	ALandscapeProxy *landscape = ue_py_check_type<ALandscapeProxy>(self);
	if (!landscape)
		return PyErr_Format(PyExc_Exception, "uobject is not a ULandscapeProxy");

	ULandscapeInfo *info = landscape->GetLandscapeInfo();
	if (!info)
		Py_RETURN_NONE;

	ue_PyUObject *ret = ue_get_python_wrapper(info);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

PyObject *py_ue_landscape_import(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int section_size;
	int sections_per_component;
	int component_x;
	int component_y;
	Py_buffer heightmap_buffer;
	int layer_type = (int)ELandscapeImportAlphamapType::Additive;

	if (!PyArg_ParseTuple(args, "iiiiy*|i:landscape_import", &section_size, &sections_per_component, &component_x, &component_y, &heightmap_buffer, &layer_type))
		return nullptr;

	ALandscapeProxy *landscape = ue_py_check_type<ALandscapeProxy>(self);
	if (!landscape)
		return PyErr_Format(PyExc_Exception, "uobject is not a ULandscapeProxy");

	int quads_per_component = sections_per_component * section_size;
	int size_x = component_x * quads_per_component + 1;
	int size_y = component_y * quads_per_component + 1;

	if (heightmap_buffer.len < (Py_ssize_t)(size_x * size_y * sizeof(uint16)))
		return PyErr_Format(PyExc_Exception, "not enough heightmap data, expecting %lu bytes", size_x * size_y * sizeof(uint16));

	uint16 *data = (uint16 *)heightmap_buffer.buf;

	TArray<FLandscapeImportLayerInfo> infos;

	landscape->Import(FGuid::NewGuid(), 0, 0, size_x - 1, size_y - 1, sections_per_component, section_size, data, nullptr, infos, (ELandscapeImportAlphamapType)layer_type);

	Py_RETURN_NONE;
}

PyObject *py_ue_landscape_export_to_raw_mesh(ue_PyUObject *self, PyObject * args)
{

	ue_py_check(self);

	int lod = 0;

	if (!PyArg_ParseTuple(args, "|i:landscape_import", &lod))
		return nullptr;

	ALandscapeProxy *landscape = ue_py_check_type<ALandscapeProxy>(self);
	if (!landscape)
		return PyErr_Format(PyExc_Exception, "uobject is not a ULandscapeProxy");

	FRawMesh raw_mesh;
	if (!landscape->ExportToRawMesh(lod, raw_mesh))
		return PyErr_Format(PyExc_Exception, "unable to export landscape to FRawMesh");

	return py_ue_new_fraw_mesh(raw_mesh);
}
#endif
