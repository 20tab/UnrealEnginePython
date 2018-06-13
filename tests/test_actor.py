import unittest
import unreal_engine as ue
from unreal_engine.classes import Actor, Character, CharacterMovementComponent, CapsuleComponent
from unreal_engine import FVector, FRotator
import time
import math

class TestActor(unittest.TestCase):

    def setUp(self):
    	self.world = ue.get_editor_world()

    def test_spawn(self):
    	new_actor = self.world.actor_spawn(Actor)
    	actor_label = 'ActorTest_' + str(int(time.time()))
    	new_actor.set_actor_label(actor_label)
    	found_actor = self.world.find_actor_by_label(actor_label)
    	self.assertEqual(found_actor, new_actor)

    def test_location(self):
    	new_actor = self.world.actor_spawn(Character, FVector(100, 200, 300))
    	self.assertEqual(new_actor.get_actor_location(), FVector(100, 200, 300))

    def test_rotation(self):
    	new_actor = self.world.actor_spawn(Character, FVector(), FRotator(0, 0, 90))
    	yaw = new_actor.get_actor_rotation().yaw
    	self.assertTrue(math.fabs(90 - yaw) < 0.1)

    def test_is_a(self):
    	new_actor = self.world.actor_spawn(Character, FVector(100, 200, 300))
    	self.assertTrue(new_actor.is_a(Actor))
    	self.assertTrue(new_actor.is_a(Character))
    	new_actor2 = self.world.actor_spawn(Actor, FVector(100, 200, 300))
    	self.assertTrue(new_actor2.is_a(Actor))
    	self.assertFalse(new_actor2.is_a(Character))

    def test_destroy(self):
    	new_actor = self.world.actor_spawn(Actor)
    	actor_label = 'ActorDestroyTest_' + str(int(time.time()))
    	new_actor.set_actor_label(actor_label)
    	found_actor = self.world.find_actor_by_label(actor_label)
    	self.assertEqual(found_actor, new_actor)
    	new_actor.actor_destroy()
    	found_actor = self.world.find_actor_by_label(actor_label)
    	self.assertIsNone(found_actor)

    def test_has_component(self):
    	new_actor = self.world.actor_spawn(Character, FVector(100, 200, 300))
    	self.assertTrue(new_actor.actor_has_component_of_type(CapsuleComponent))

    def test_get_component(self):
    	new_actor = self.world.actor_spawn(Character, FVector(100, 200, 300))
    	self.assertTrue(new_actor.get_actor_component_by_type(CharacterMovementComponent).bIsActive)

    def test_get_components(self):
    	new_actor = self.world.actor_spawn(Character, FVector(100, 200, 300))
    	self.assertTrue(len(new_actor.get_actor_components()), 4)



if __name__ == '__main__':
    unittest.main(exit=False)
