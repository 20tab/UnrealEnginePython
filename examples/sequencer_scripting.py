# the Sequencer API support has been sponsored by Matthew Whelan (http://www.mattwhelan.com/)
import unreal_engine as ue

from unreal_engine.classes import MovieSceneAudioTrack, LevelSequenceFactoryNew, MovieSceneSkeletalAnimationTrack, Character, SkeletalMesh, MovieScene3DTransformTrack, CineCameraActor
import time
from unreal_engine.structs import FloatRange, FloatRangeBound, MovieSceneObjectBindingID
from unreal_engine import FTransform, FVector
from unreal_engine.enums import EMovieSceneObjectBindingSpace


# create a new level sequence asset
factory = LevelSequenceFactoryNew()
seq = factory.factory_create_new('/Game/MovieMaster' + str(int(time.time())))

# add an audio track (without sound section ;) to the sequence
audio = seq.sequencer_add_master_track(MovieSceneAudioTrack)

# get a reference to the editor world (to spawn actors)
world = ue.get_editor_world()

# spawn a new character and modify it (post_edit_change will allow the editor/sequencer to be notified of actor updates)
character = world.actor_spawn(Character)
# notify modifications are about to happen...
character.modify()
character.Mesh.SkeletalMesh = ue.load_object(SkeletalMesh, '/Game/InfinityBladeAdversaries/Enemy/Enemy_Bear/Enemy_Bear.Enemy_Bear')
# finalize the actor
character.post_edit_change()

# add to the sequencer as a possessable (shortcut method returning the guid as string)
guid = seq.sequencer_add_actor(character)


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

# add a transform track/section in one shot to the actor
transform = seq.sequencer_add_track(MovieScene3DTransformTrack, guid).sequencer_track_add_section()
transform.StartTime = 0
transform.EndTime = 5

# add keyframes to the transform section
transform.sequencer_section_add_key(0, FTransform(FVector(0, 0, 17 * 100)))
transform.sequencer_section_add_key(1, FTransform(FVector(0, 0, 22 * 100)))
transform.sequencer_section_add_key(2, FTransform(FVector(0, 0, 26 * 100)))
transform.sequencer_section_add_key(2.5, FTransform(FVector(0, 0, 30 * 100)))

# set playback range
float_range = FloatRange(LowerBound=FloatRangeBound(Value=0), UpperBound=FloatRangeBound(Value=10))
seq.MovieScene.PlaybackRange = float_range

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

# set cameras time slots
camera1.StartTime = 0
camera1.EndTime = 3.5

camera2.StartTime = 3.5
camera2.EndTime = 5

# notify the sequence editor that something heavily changed (True will focus to the sequence editor)
seq.sequencer_changed(True)
