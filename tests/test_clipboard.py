import unittest
import unreal_engine as ue

class TestClipboard(unittest.TestCase):

    def test_copy_and_paste(self):
        ue.clipboard_copy('Hello from python unit test')
        self.assertEqual(ue.clipboard_paste(), 'Hello from python unit test')
    
    def test_copy_and_paste_red_light(self):
        ue.clipboard_copy('Hello from red light test')
        self.assertNotEqual(ue.clipboard_paste(), 'Hello from python unit test')

if __name__ == '__main__':
    unittest.main(exit=False)
