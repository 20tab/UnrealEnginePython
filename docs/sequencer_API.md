The Sequencer API
-----------------

The sequencer API development has been sponsored by Matthew Whelan (http://www.mattwhelan.com/)

Credits should go to Ron Fischer too, for having explained me the sequencer api ;)

Creating a Sequencer Asset
--------------------------

You can create a new 'Level Sequence' asset using the LevelSequencerFactoryNew class:

```python
from unreal_engine.classes import LevelSequenceFactoryNew

factory = LevelSequenceFactoryNew()
seq = factory.factory_create_new('/Game/MovieMaster')
```

Adding a master track to a Level Sequence
-----------------------------------------

In the following example we add a MovieSceneAudioTrack as a master track

```python
from unreal_engine.classes import MovieSceneAudioTrack

audio_track = seq.sequencer_add_master_track(MovieSceneAudioTrack)
```

Getting the available track classes
-----------------------------------

the following snippet prints the name of the MovieSceneTrack classes

```python
import unreal_engine as ue
from unreal_engine.classes import MovieSceneTrack

for uclass in ue.all_classes():
    if uclass.is_child_of(MovieSceneTrack):
        print(uclass.get_name())
```

Getting the list of master tracks
---------------------------------

```python
tracks = seq.sequencer_master_tracks()
```

Possessables and Spawnables
---------------------------

Adding tracks
-------------

Adding a section to a track
---------------------------

This is a pretty complex example, we add a skeletal animation track and then we add a bunch of sections (each of them should point to an animation asset using the AnimSequence field)

```python
from unreal_engine.classes import MovieSceneSkeletalAnimationTrack

# add an animation track mapped to the just added actor
anim = seq.sequencer_add_track(MovieSceneSkeletalAnimationTrack, guid)

# create 3 animations sections (assign AnimSequence field to set the animation to play)
anim_sequence = anim.sequencer_track_add_section()
anim_sequence.StartTime = 1
anim_sequence.EndTime = 3
anim_sequence.RowIndex = 0

anim_sequence2 = anim.sequencer_track_add_section()
anim_sequence2.RowIndex = 1
anim_sequence2.StartTime = 2
anim_sequence2.EndTime = 5

anim_sequence3 = anim.sequencer_track_add_section()
anim_sequence3.RowIndex = 1
anim_sequence3.SlotName = 'Hello'
anim_sequence3.StartTIme = 0
anim_sequence3.EndTime = 30
```

Adding Keyframes to sections
----------------------------

```python
from unreal_engine import FTransform, FVector
from unreal_engine.classes import MovieScene3DTransformTrack

# possess an actor and get its binding guid
guid = seq.sequencer_add_actor(an_actor_in_the_scene)

transform_track = seq.sequencer_add_track(MovieScene3DTransformTrack, guid)
transform_section = transform_track.sequencer_track_add_section()

# set section size
transform_section.StartTime = 0
transform_section.EndTime = 5

# adding keyframes to section
transform_section.sequencer_section_add_key(0.0, FTransform(FVector(17, 22, 30)))
transform_section.sequencer_section_add_key(0.17, FTransform(FVector(30, 17, 22)))
```

Managing the camera cut track
-----------------------------

Folders
-------

listing folders

```python

# get the root folders
root_folders = seq.sequencer_folders()

# get subfolders
sub_folders = seq.sequencer_folders(root_folders[0])
```

creating folders

```python
new_folder = seq.sequencer_create_folder('Folder001')

# create subfolder
new_sub_folder = seq.sequencer_create_folder('SubFolder001', new_folder)
```

Notify changes to the editor
----------------------------

Some of the sequencer api operations do not update the editor, if you need to force an update you can use

```python
seq.sequencer_changed(True)
```

the boolean argument (if True) move the editor focus to the sequencer

Example
-------

Check it here: https://github.com/20tab/UnrealEnginePython/blob/master/examples/sequencer_scripting.py
