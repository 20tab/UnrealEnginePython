#pragma once



#include "UEPyModule.h"

PyObject *py_ue_get_anim_instance(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_skeletal_mesh(ue_PyUObject *, PyObject *);

PyObject *py_ue_skeleton_get_parent_index(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_bones_get_num(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_get_bone_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_find_bone_index(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_get_ref_bone_pose(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeleton_add_bone(ue_PyUObject *, PyObject *);

PyObject *py_ue_skeletal_mesh_set_soft_vertices(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_get_soft_vertices(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_set_skeleton(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_get_lod(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_get_raw_indices(ue_PyUObject *, PyObject *);

PyObject *py_ue_skeletal_mesh_get_bone_map(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_set_bone_map(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_set_active_bone_indices(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_set_required_bones(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_get_active_bone_indices(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_get_required_bones(ue_PyUObject *, PyObject *);

PyObject *py_ue_skeletal_mesh_lods_num(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_sections_num(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_build_lod(ue_PyUObject *, PyObject *, PyObject *);

PyObject *py_ue_skeletal_mesh_register_morph_target(ue_PyUObject *, PyObject *);

PyObject *py_ue_morph_target_populate_deltas(ue_PyUObject *, PyObject *);
PyObject *py_ue_morph_target_get_deltas(ue_PyUObject *, PyObject *);
PyObject *py_ue_skeletal_mesh_to_import_vertex_map(ue_PyUObject *, PyObject *);
