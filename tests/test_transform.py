import unittest
import unreal_engine as ue
from unreal_engine import FVector, FRotator, FTransform

class TestVector(unittest.TestCase):

    def test_add(self):
        vector0 = FVector(1, 2, 3)
        vector1 = FVector(4, 5, 6)
        self.assertEqual( vector0 + vector1, FVector(5, 7, 9))

    def test_add_negative(self):
        vector0 = FVector(1, 2, 3)
        vector1 = FVector(-4, -5, -6)
        self.assertEqual( vector0 + vector1, FVector(-3, -3, -3))

    def test_scale(self):
        vector0 = FVector(1, 2, -3)
        self.assertEqual( vector0 * 2, FVector(2, 4, -6))

class TestRotator(unittest.TestCase):

	def test_roll_pitch_yaw(self):
		rotator0 = FRotator(10, 20, 30)
		self.assertEqual( rotator0.roll, 10)
		self.assertEqual( rotator0.pitch, 20)
		self.assertEqual( rotator0.yaw, 30)

class TestTransform(unittest.TestCase):

	def test_default_values(self):
		transform0 = FTransform()
		self.assertEqual( transform0.translation, FVector(0, 0, 0))
		self.assertEqual( transform0.rotation.roll, 0)
		self.assertEqual( transform0.rotation.pitch, 0)
		self.assertEqual( transform0.rotation.yaw, 0)
		self.assertEqual( transform0.scale, FVector(1, 1, 1))


