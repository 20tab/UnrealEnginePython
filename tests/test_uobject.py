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
        mat_name = 'MatTest_' + str(int(time.time()))
        new_material.set_name(mat_name)
        self.assertEqual(new_material.get_name(), mat_name)

    def test_save_package(self):
        new_material = Material()
        mat_name = 'MatTestSaved_' + str(int(time.time()))
        new_material.set_name(mat_name)
        package_name = '/Game/Tests/MaterialsTest' + str(int(time.time()))
        new_material.save_package(package_name)
        asset_name = '{0}.{1}'.format(package_name, mat_name)
        try:
            asset = ue.get_asset(asset_name)
        except:
            asset = None
        self.assertIsNotNone(asset)
        ue.delete_asset(asset_name)

   



