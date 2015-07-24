import lagrangian
import unittest
import numpy


class TestAxis(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_latitude_unit(self):
        obj = lagrangian.LatitudeUnit()
        self.assertTrue(obj("degrees_north"))
        self.assertTrue(obj("degree_north"))
        self.assertTrue(obj("degree_N"))
        self.assertTrue(obj("degrees_N"))
        self.assertTrue(obj("degreeN"))
        self.assertTrue(obj("degreesN"))
        self.assertFalse(obj('foo'))

    def test_longitude_unit(self):
        obj = lagrangian.LongitudeUnit()
        self.assertTrue(obj("degrees_east"))
        self.assertTrue(obj("degree_east"))
        self.assertTrue(obj("degree_E"))
        self.assertTrue(obj("degrees_E"))
        self.assertTrue(obj("degreeE"))
        self.assertTrue(obj("degreesE"))
        self.assertFalse(obj('foo'))

    def test_axis(self):
        axis = lagrangian.Axis(numpy.arange(10, dtype='f8'),
                               lagrangian.kX,
                               "m")
        self.assertEqual(axis.type, lagrangian.kX)
        self.assertEqual(axis.get_coordinate_value(5), 5)
        try:
            axis.get_coordinate_value(20)
        except IndexError:
            pass
        else:
            self.fail('Did not see IndexError')
        self.assertEqual(axis.get_min_value(), 0)
        self.assertEqual(axis.get_max_value(), 9)
        self.assertEqual(axis.get_num_elements(), 10)
        self.assertTrue(axis.is_regular)
        self.assertEqual(axis.find_index(5), 5)
        self.assertEqual(axis.find_index(50), -1)
        self.assertEqual(axis.find_index_bounded(5), 5)
        self.assertEqual(axis.find_index_bounded(50), 9)
        self.assertEqual(axis.units, "m")
        axis.convert("dm")
        try:
            axis.convert("x")
        except RuntimeError:
            pass
        else:
            self.fail('Did not see RuntimeError')
        self.assertEqual(axis.find_index(50), 5)
        self.assertEqual(axis.get_min_value(), 0)
        self.assertEqual(axis.get_max_value(), 90)
        self.assertTrue(axis.is_regular)
        axis = lagrangian.Axis(
            numpy.array([1, 2, 4, 16, 32, 64], dtype='f8'),
            lagrangian.kX,
            "m")
        self.assertFalse(axis.is_regular)
        self.assertEqual(axis.find_index(16), 3)
        self.assertEqual(axis.find_index(17), 3)
        self.assertEqual(axis.find_index(31), 4)
        self.assertEqual(axis.find_indexes(16), (3, 4))
        self.assertEqual(axis.find_indexes(20), (3, 4))
        self.assertEqual(axis.find_indexes(128), None)
        axis = lagrangian.Axis(
            numpy.array([-180, -90, 0, 90], dtype='f8'),
            lagrangian.kLongitude,
            "m")
        self.assertEqual(axis.get_min_value(), -180)
        self.assertEqual(axis.get_max_value(), 90)
        self.assertEqual(axis.normalize(270, 360), -90)
        self.assertEqual(axis.start, -180)
        self.assertEqual(axis.increment, 90)
        self.assertEqual(
            axis,
            lagrangian.Axis(
                numpy.array([-180, -90, 0, 90], dtype='f8'),
                lagrangian.kLongitude,
                "m"))
        self.assertNotEqual(
            axis,
            lagrangian.Axis(
                numpy.array([-180, -90, 0, 90], dtype='f8'),
                lagrangian.kX,
                "m"))
        self.assertNotEqual(
            axis,
            lagrangian.Axis(
                numpy.array([-180, -90, 0, 90], dtype='f8'),
                lagrangian.kLongitude))


if __name__ == "__main__":
    unittest.main()