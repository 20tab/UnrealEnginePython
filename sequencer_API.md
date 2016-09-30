The Sequencer API (Work in progress)
------------------------------------

Credits go to Ron Fischer for having explained me the sequencer api.

NOTE: seq_obj in the following docs is a reference to a ULevelSequence object.

NOTE2: GUID's are always managed as python strings

NOTE3: track_obj in the following docs is a reference to a UMovieSceneTrack object.

```py
seq_obj.sequencer_master_tracks()
```

returns a list of the sequencer master tracks

```py
seq_obj.sequencer_possessables()
```

returns the list of possessables mapped to a sequencer. The list is composed by 3-items tuples, first item is the possessable guid, second is the name, third is the reference to the possessable class



```py
seq_obj.sequencer_find_possessable(guid)
```

get the UObject given a GUID

```py
seq_obj.sequencer_possessable_tracks(guid)
```

returns the list of tracks mapped to a possessable guid

```py
track_obj.sequencer_track_sections()
```

get the list of sections in a track

```py
seq_obj.sequencer_sections()
```

get the list of all sections in a sequencer

```py
seq_obj.sequencer_folders()
```

get the list of folders in a sequencer

```py
new_track = seq_obj.sequencer_add_master_track(u_class)
```

add a new master track. u_class must be a class child of UMovieSceneTrack, like ue.find_class('MovieSceneAudioTrack')


```py
name = track_obj.sequencer_get_display_name()
```

(editor only) get the display name of various seqnecer objects (currently only tracks are supported)

```py
track_obj.sequencer_set_display_name(name)
```

(editor only) set the display name of various seqnecer objects (currently only tracks are supported)
