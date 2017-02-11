import unittest
import unreal_engine as ue

class TestPlugin(unittest.TestCase):

    def test_find(self):
    	uep = ue.find_plugin('UnrealEnginePython')
    	self.assertEqual(uep.get_name(), 'UnrealEnginePython')

    def test_enabled(self):
    	uep = ue.find_plugin('UnrealEnginePython')
    	self.assertTrue(uep.is_enabled)




