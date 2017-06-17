import unittest
import unreal_engine as ue
from unreal_engine.classes import Blueprint

class TestPlotterPlatform(unittest.TestCase):

    def setUp(self):
        ue.allow_actor_script_execution_in_editor(True)
        ue.begin_transaction('test')
        self.world = ue.get_editor_world()
        self.blueprint = ue.load_object(Blueprint, '/Game/PlotterPlatforms.PlotterPlatforms')

    def tearDown(self):
        ue.end_transaction()
        ue.editor_undo()
        ue.allow_actor_script_execution_in_editor(False)

    def test_red_cube_overlap(self):
        actor = self.world.actor_spawn(self.blueprint.GeneratedClass)
        self.assertEqual(actor.RedCubeCounter, 0)
        actor.RedCube.broadcast('OnComponentBeginOverlap')
        self.assertEqual(actor.RedCubeCounter, 1)
        self.assertEqual(actor.GreenCubeCounter, 0)
        self.assertEqual(actor.BlueCubeCounter, 0)

    def test_green_cube_overlap(self):
        actor = self.world.actor_spawn(self.blueprint.GeneratedClass)
        self.assertEqual(actor.GreenCubeCounter, 0)
        actor.GreenCube.broadcast('OnComponentBeginOverlap')
        self.assertEqual(actor.RedCubeCounter, 0)
        self.assertEqual(actor.GreenCubeCounter, 1)
        self.assertEqual(actor.BlueCubeCounter, 0)

    def test_blue_cube_overlap(self):
        actor = self.world.actor_spawn(self.blueprint.GeneratedClass)
        self.assertEqual(actor.BlueCubeCounter, 0)
        actor.BlueCube.broadcast('OnComponentBeginOverlap')
        self.assertEqual(actor.RedCubeCounter, 0)
        self.assertEqual(actor.GreenCubeCounter, 0)
        self.assertEqual(actor.BlueCubeCounter, 1)

if __name__ == '__main__':
    unittest.main(exit=False)
