#pragma once



#include "UEPyModule.h"

PyObject *py_ue_sequencer_master_tracks(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_possessable_tracks(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_track_sections(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_get_camera_cut_track(ue_PyUObject *, PyObject *);
#if WITH_EDITOR
PyObject *py_ue_sequencer_set_playback_range(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_set_view_range(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_set_working_range(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_set_section_range(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_get_playback_range(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_get_selection_range(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_folders(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_create_folder(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_set_display_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_get_display_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_changed(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_section_add_key(ue_PyUObject *, PyObject *);

PyObject *py_ue_sequencer_add_camera_cut_track(ue_PyUObject *, PyObject *);

PyObject *py_ue_sequencer_add_possessable(ue_PyUObject *, PyObject *);

PyObject *py_ue_sequencer_track_add_section(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_add_actor(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_add_actor_component(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_make_new_spawnable(ue_PyUObject *, PyObject *);

PyObject *py_ue_sequencer_remove_possessable(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_remove_spawnable(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_remove_camera_cut_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_remove_master_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_remove_track(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_import_fbx_transform(ue_PyUObject *, PyObject *);
#endif
PyObject *py_ue_sequencer_sections(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_possessables(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_possessables_guid(ue_PyUObject *, PyObject *);

PyObject *py_ue_sequencer_find_possessable(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_find_spawnable(ue_PyUObject *, PyObject *);

PyObject *py_ue_sequencer_add_master_track(ue_PyUObject *, PyObject *);



PyObject *py_ue_sequencer_add_track(ue_PyUObject *, PyObject *);

