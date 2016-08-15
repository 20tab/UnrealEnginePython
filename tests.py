import sys
import unittest
import unreal_engine as ue
import tests

class UnrealEngineOutput:
    def __init__(self, logger):
        self.logger = logger
    def write(self, buf):
        self.logger(buf)
    def flush(self):
        return

sys.stdout = UnrealEngineOutput(ue.log)
sys.stderr = UnrealEngineOutput(ue.log_error)

tester_actor = None

class TestTransform(unittest.TestCase):
    def setUp(self):
        self.actor = tests.tester_actor

    def test_location(self):
        self.actor.set_actor_location(1, 1, 1)
        location = self.actor.get_actor_location()
        self.assertEqual(location, (1, 1, 1))

    def test_rotation(self):
        self.actor.set_actor_rotation(17, 22, 30)
        roll, pitch, yaw = self.actor.get_actor_rotation()
        self.assertTrue(17 - roll < 0.01)
        self.assertTrue(22 - pitch < 0.01)
        self.assertTrue(30 - yaw < 0.01)

    def test_scale(self):
        self.actor.set_actor_scale(3, 5, 7)
        scale = self.actor.get_actor_scale()
        self.assertEqual(scale, (3, 5, 7))

class TestPlayer(unittest.TestCase):
    def setUp(self):
        self.actor = tests.tester_actor

    def test_create_player(self):
        self.assertEqual(self.actor.get_num_players(), 1)
        self.actor.create_player(-1)
        self.assertEqual(self.actor.get_num_players(), 2)

        
class TesterActor:
    def begin_play(self):
        tests.tester_actor = self.uobject
        unittest.main(module='tests', exit=False)
        self.uobject.quit_game()
    def tick(self, delta_time):
        pass
