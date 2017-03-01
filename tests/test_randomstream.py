import unittest
import unreal_engine as ue
from unreal_engine import FVector, FRandomStream

class TestRandomStream(unittest.TestCase):

    def test_frand(self):
        r = FRandomStream()
        self.assertTrue(type(r.frand()) is float)

    def test_frand_range(self):
        r = FRandomStream()
        value = r.frand_range(17, 22)
        self.assertTrue(value >= 17)
        self.assertTrue(value <= 22)

    def test_generate_new_seed(self):
        r = FRandomStream()
        first_seed = r.get_current_seed()
        r.generate_new_seed()
        second_seed = r.get_current_seed()
        self.assertTrue(first_seed != second_seed)

    def test_get_fraction(self):
        r = FRandomStream()
        value = r.get_fraction()
        self.assertTrue(type(value) is float)
        self.assertTrue(value >= 0.0)
        self.assertTrue(value <= 1.0)

    def test_get_fraction(self):
        r = FRandomStream()
        first_seed = r.get_current_seed()
        initial_seed = r.get_initial_seed()
        self.assertEqual(first_seed, initial_seed)

    def test_get_unit_vector(self):
        r = FRandomStream()
        self.assertTrue((1.0 - r.get_unit_vector().length()) < 0.1)

    def test_get_unsigned_int(self):
        r = FRandomStream()
        value = r.get_unsigned_int()
        self.assertTrue(type(value) is int)
        self.assertTrue(value >= 0)

    def test_initialize(self):
        r = FRandomStream()
        r.initialize(17)
        self.assertEqual(r.get_initial_seed(), 17)

    def test_rand_helper(self):
        r = FRandomStream()
        value = r.rand_helper(30)
        self.assertTrue(type(value) is int)
        self.assertTrue(value >= 0)
        self.assertTrue(value <= 30)

    def test_rand_range(self):
        r = FRandomStream()
        value = r.rand_range(17, 30)
        self.assertTrue(type(value) is int)
        self.assertTrue(value >= 17)
        self.assertTrue(value <= 30)

    def test_reset(self):
        r = FRandomStream()
        first_seed = r.get_current_seed()
        r.generate_new_seed()
        second_seed = r.get_current_seed()
        self.assertTrue(first_seed != second_seed)
        r.reset()
        self.assertEqual(r.get_current_seed(), r.get_initial_seed())

    def test_vrand(self):
        r = FRandomStream()
        self.assertTrue((1.0 - r.vrand().length()) < 0.1)

    def test_vrand_cone(self):
        r = FRandomStream()
        self.assertTrue((1.0 - r.vrand_cone(FVector(0, 0, 1), 30, 22).length()) < 0.1)
