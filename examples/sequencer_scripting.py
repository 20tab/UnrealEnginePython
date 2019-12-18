# the Sequencer API support has been sponsored by Matthew Whelan (http://www.mattwhelan.com/)
import unreal_engine as ue

from unreal_engine.classes import MovieSceneAudioTrack, LevelSequenceFactoryNew, MovieSceneSkeletalAnimationTrack, Character, SkeletalMesh, MovieScene3DTransformTrack, CineCameraActor, AnimSequence
import time
from unreal_engine.structs import FloatRange, FloatRangeBound, MovieSceneObjectBindingID
from unreal_engine import FTransform, FVector
from unreal_engine.enums import EMovieSceneObjectBindingSpace


# create a new level sequence asset
factory = LevelSequenceFactoryNew()
seq = factory.factory_create_new('/Game/MovieMaster' + str(int(time.time())))

if ue.ENGINE_MINOR_VERSION >= 20
    print(seq.MovieScene.TickResolution.Numerator)

seq.sequencer_set_playback_range(0, 30)

# add an audio track (without sound section ;) to the sequence
audio = seq.sequencer_add_master_track(MovieSceneAudioTrack)

# get a reference to the editor world (to spawn actors)
world = ue.get_editor_world()

# spawn a new character and modify it (post_edit_change will allow the editor/sequencer to be notified of actor updates)
character = world.actor_spawn(Character)
# notify modifications are about to happen...
character.modify()
character.Mesh.SkeletalMesh = ue.load_object(SkeletalMesh, '/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin')
# finalize the actor
character.post_edit_change()

# add to the sequencer as a possessable (shortcut method returning the guid as string)
guid = seq.sequencer_add_actor(character)


# add an animation track mapped to the just added actor
anim = seq.sequencer_add_track(MovieSceneSkeletalAnimationTrack, guid)

# create 3 animations sections (assign AnimSequence field to set the animation to play)
anim_sequence = anim.sequencer_track_add_section()
anim_sequence.sequencer_set_section_range(1, 3)
anim_sequence.Params.Animation = ue.load_object(AnimSequence, '/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun')
anim_sequence.RowIndex = 0

anim_sequence2 = anim.sequencer_track_add_section()
anim_sequence2.RowIndex = 1
anim_sequence2.sequencer_set_section_range(2, 5)

anim_sequence3 = anim.sequencer_track_add_section()
anim_sequence3.RowIndex = 1
anim_sequence3.SlotName = 'Hello'
anim_sequence3.sequencer_set_section_range(0, 30)

# add a transform track/section in one shot to the actor
transform = seq.sequencer_add_track(MovieScene3DTransformTrack, guid).sequencer_track_add_section()
transform.sequencer_set_section_range(0, 50)

# add keyframes to the transform section (from 4.20 you can directly use teh reflection api, and the methods returns the frame numbers)
print(transform.sequencer_section_add_key(0, FTransform(FVector(0, 0, 17 * 100))))
print(transform.sequencer_section_add_key(1.1, FTransform(FVector(0, 0, 22 * 100))))
print(transform.sequencer_section_add_key(2.2, FTransform(FVector(0, 0, 26 * 100))))
print(transform.sequencer_section_add_key(3.3, FTransform(FVector(0, 0, 30 * 100))))

# add camera cut track (can be only one)
camera_cut_track = seq.sequencer_add_camera_cut_track()

# add two camera views
camera1 = camera_cut_track.sequencer_track_add_section()
camera2 = camera_cut_track.sequencer_track_add_section()

# spawn 2 cine cameras in the stage and posses them with the sequencer
cine_camera = world.actor_spawn(CineCameraActor)
camera_guid = seq.sequencer_add_actor(cine_camera)

cine_camera2 = world.actor_spawn(CineCameraActor)
camera2_guid = seq.sequencer_add_actor(cine_camera2)

# assign the two cameras to the camera cut sections (via binding id)
camera1.CameraBindingID = MovieSceneObjectBindingID( Guid=ue.string_to_guid( camera_guid ), Space=EMovieSceneObjectBindingSpace.Local )
camera2.CameraBindingID = MovieSceneObjectBindingID( Guid=ue.string_to_guid( camera2_guid ), Space=EMovieSceneObjectBindingSpace.Local )

# set cameras ranges
camera1.sequencer_set_section_range(3.5, 5)
camera2.sequencer_set_section_range(0.5, 17)

# notify the sequence editor that something heavily changed (True will focus to the sequence editor)
seq.sequencer_changed(True)
