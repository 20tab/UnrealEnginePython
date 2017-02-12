import unittest
import unreal_engine as ue
from unreal_engine.classes import Material
import time
import math

class TestUObject(unittest.TestCase):

    def test_new_object(self):
        new_material = Material()
        self.assertIsNotNone(new_material)

    def test_set_name(self):
        new_material = Material()
        mat_name = 'MatTest_' + str(time.time())
        new_material.set_name(mat_name)
        self.assertEqual(new_material.get_name(), mat_name)

   



