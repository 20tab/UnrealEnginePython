import unittest
import unreal_engine as ue
from unreal_engine.classes import LevelSequenceFactoryNew, Character, MovieSceneAudioTrack, CineCameraActor

class TestSequencer(unittest.TestCase):

    def setUp(self):
    	factory = LevelSequenceFactoryNew()
    	self.asset = factory.factory_create_new('/Game/SequencerTests/001')

    def tearDown(self):
    	ue.delete_asset(self.asset.get_path_name())
    	
    def test_add_actor(self):
    	world = ue.get_editor_world()
    	character = world.actor_spawn(Character)
    	guid = self.asset.sequencer_add_actor(character)
    	self.assertTrue(guid in self.asset.sequencer_possessables_guid())

    def test_add_possessable(self):
    	world = ue.get_editor_world()
    	character = world.actor_spawn(Character)
    	guid = self.asset.sequencer_add_possessable(character)
    	self.assertTrue(guid in self.asset.sequencer_possessables_guid())

    def test_audio_track(self):
    	audio = self.asset.sequencer_add_master_track(MovieSceneAudioTrack)
    	self.assertTrue(audio in self.asset.sequencer_master_tracks())

    def test_camera_cut_track(self):
    	world = ue.get_editor_world()
    	camera_cut_track = self.asset.sequencer_add_camera_cut_track()
    	camera1 = camera_cut_track.sequencer_track_add_section()
    	cine_camera = world.actor_spawn(CineCameraActor)
    	camera_guid = self.asset.sequencer_add_possessable(cine_camera)
    	camera1.CameraGuid = ue.string_to_guid(camera_guid)
    	print(self.asset.properties())
    	self.assertEqual(camera_cut_track, self.asset.MovieScene.CameraCutTrack)
    	self.assertEqual(camera_cut_track.sequencer_track_sections(), [camera1])

    def test_spawnable(self):
    	world = ue.get_editor_world()
    	character = world.actor_spawn(Character)
    	spawnable_guid = self.asset.sequencer_make_new_spawnable(character)
    	found_spawnable = self.asset.sequencer_find_spawnable(spawnable_guid)
    	guid = ue.string_to_guid(spawnable_guid)
    	self.assertEqual(guid.A, found_spawnable.Guid.A)
    	self.assertEqual(guid.B, found_spawnable.Guid.B)
    	self.assertEqual(guid.C, found_spawnable.Guid.C)
    	self.assertEqual(guid.D, found_spawnable.Guid.D)
    	self.assertEqual(guid, found_spawnable.Guid)

    def test_folders(self):
    	new_folder = self.asset.sequencer_create_folder('Test001')
    	self.assertTrue(new_folder in self.asset.sequencer_folders())

    def test_subfolders(self):
    	new_folder = self.asset.sequencer_create_folder('Test002')
    	new_subfolder = self.asset.sequencer_create_folder('Test003', new_folder)
    	self.assertTrue(new_subfolder in self.asset.sequencer_folders(new_folder))

