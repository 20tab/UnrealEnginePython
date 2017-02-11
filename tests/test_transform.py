import unittest
import unreal_engine as ue
from unreal_engine import FVector, FRotator

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
