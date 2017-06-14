import unittest
import unreal_engine as ue
from unreal_engine import IConsoleManager

class TestConsoleManager(unittest.TestCase):

    def setUp(self):
        if IConsoleManager.is_name_registered('uep_test_001'):
            IConsoleManager.unregister_object('uep_test_001')

    def test_history(self):
        history = IConsoleManager.get_history()
        IConsoleManager.add_history_entry('FooBar')
        self.assertEqual(IConsoleManager.get_history()[-1], 'FooBar')

    def test_variable_string_register(self):
        IConsoleManager.register_variable_string('uep_test_001', 'test help string 001', 'xyz')
        self.assertEqual(IConsoleManager.get_string('uep_test_001'), 'xyz')
        self.assertEqual(IConsoleManager.get_help('uep_test_001'), 'test help string 001')

    def test_variable_string_set_help(self):
        IConsoleManager.register_variable_string('uep_test_001', 'test help string 001', 'xyz')
        self.assertEqual(IConsoleManager.get_help('uep_test_001'), 'test help string 001')
        IConsoleManager.set_help('uep_test_001', 'test help string 002')
        self.assertEqual(IConsoleManager.get_help('uep_test_001'), 'test help string 002')

    def test_variable_int_register(self):
        IConsoleManager.register_variable_int('uep_test_001', 'test help string 001', 17)
        self.assertEqual(IConsoleManager.get_int('uep_test_001'), 17)
        self.assertEqual(IConsoleManager.get_help('uep_test_001'), 'test help string 001')

    def test_variable_float_register(self):
        IConsoleManager.register_variable_float('uep_test_001', 'test help string 001', 17.22)
        self.assertTrue((17.22 - IConsoleManager.get_float('uep_test_001')) < 0.1)
        self.assertEqual(IConsoleManager.get_help('uep_test_001'), 'test help string 001')

if __name__ == '__main__':
    unittest.main(exit=False)