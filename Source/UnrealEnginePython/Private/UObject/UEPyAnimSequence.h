#pragma once

#include "UEPyModule.h"
#include "Animation/AnimSequence.h"
#include "Animation/BlendSpaceBase.h"
#include "Animation/AnimMontage.h"
#include "Wrappers/UEPyFRawAnimSequenceTrack.h"


PyObject *py_ue_anim_get_skeleton(ue_PyUObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_ue_anim_sequence_get_raw_animation_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_get_raw_animation_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_add_new_raw_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_update_raw_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_anim_composite_section(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_update_compressed_track_map_from_raw(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_sequence_apply_raw_anim_changes(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_add_key_to_sequence(ue_PyUObject *, PyObject *);
#endif
PyObject *py_ue_anim_set_skeleton(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_get_bone_transform(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_extract_bone_transform(ue_PyUObject *, PyObject *);
PyObject *py_ue_anim_extract_root_motion(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_blend_parameter(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_blend_parameter(ue_PyUObject *, PyObject *);
