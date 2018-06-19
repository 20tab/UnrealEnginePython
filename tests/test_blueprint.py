import unittest
import unreal_engine as ue
from unreal_engine.classes import Actor, Character
from unreal_engine import FVector
import time
import math

class TestBlueprint(unittest.TestCase):

    def setUp(self):
        self.world = ue.get_editor_world()
        self.random_string = str(int(time.time()))


    def tearDown(self):
        ue.allow_actor_script_execution_in_editor(False)

    def test_creation(self):
        new_blueprint = ue.create_blueprint(Actor, '/Game/Tests/Blueprints/Test0_' + self.random_string)
        ue.log(new_blueprint.ParentClass)
        self.assertEqual(new_blueprint.ParentClass, Actor)
        self.assertNotEqual(new_blueprint.ParentClass, Character)

    def test_spawn(self):
        new_blueprint = ue.create_blueprint(Character, '/Game/Tests/Blueprints/Test1_' + self.random_string)
        new_actor = self.world.actor_spawn(new_blueprint.GeneratedClass)
        self.assertTrue(new_actor.is_a(Character))

    def test_variable(self):
        new_blueprint = ue.create_blueprint(Character, '/Game/Tests/Blueprints/Test2_' + self.random_string)
        ue.blueprint_add_member_variable(new_blueprint, 'TestValue', 'int')
        ue.compile_blueprint(new_blueprint)
        new_actor = self.world.actor_spawn(new_blueprint.GeneratedClass)
        new_actor.TestValue = 17
        self.assertEqual(new_actor.get_property('TestValue'), 17)

    def test_event(self):
        new_blueprint = ue.create_blueprint(Character, '/Game/Tests/Blueprints/Test3_' + self.random_string)
        uber_page = new_blueprint.UberGraphPages[0]
        x, y = uber_page.graph_get_good_place_for_new_node()
        test_event = uber_page.graph_add_node_custom_event('TestEvent', x, y)
        x, y = uber_page.graph_get_good_place_for_new_node()
        node_set_actor_location = uber_page.graph_add_node_call_function(Actor.K2_SetActorLocation, x, y)
        test_event.node_find_pin('then').make_link_to(node_set_actor_location.node_find_pin('execute'))
        node_set_actor_location.node_find_pin('NewLocation').default_value = '17,30,22'
        ue.compile_blueprint(new_blueprint)
        new_actor = self.world.actor_spawn(new_blueprint.GeneratedClass)
        self.assertEqual(new_actor.get_actor_location(), FVector(0, 0, 0))
        ue.allow_actor_script_execution_in_editor(True)
        new_actor.TestEvent()
        self.assertEqual(new_actor.get_actor_location(), FVector(17, 30, 22))


if __name__ == '__main__':
    unittest.main(exit=False)
