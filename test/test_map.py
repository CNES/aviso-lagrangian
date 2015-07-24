import datetime
import lagrangian
import math
import os
import unittest


class TestMapProperties(unittest.TestCase):
    def test(self):
        map_properties = lagrangian.MapProperties(360, 180, -180, -90, 1)
        self.assertEqual(map_properties.get_x_value(180), 0)
        self.assertEqual(map_properties.get_y_value(90), 0)
        self.assertEqual(map_properties.nx, 360)
        self.assertEqual(map_properties.ny, 180)
        self.assertEqual(map_properties.step, 1)
        self.assertEqual(map_properties.x_min, -180)
        self.assertEqual(map_properties.y_min, -90)
        x = (map_properties.get_x_axis())
        self.assertEqual(x[0], -180)
        self.assertEqual(x[-1], 179)
        self.assertEqual(len(x), 360)
        y = (map_properties.get_y_axis())
        self.assertEqual(y[0], -90)
        self.assertEqual(y[-1], 89)
        self.assertEqual(len(y), 180)


class TestMapOfFiniteLyapunovExponents(unittest.TestCase):
    def setUp(self):
        os.environ['ROOT'] = os.path.dirname(__file__)
        self.ini = os.path.join(os.environ['ROOT'], 'map.ini')
        self.path = os.path.join(
            os.path.dirname(__file__),
            'data',
            'dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc')
        os.environ['OMP_NUM_THREADS'] = '4'

    def test(self):
        ts = lagrangian.TimeSerie(self.ini,
                                  lagrangian.kMetric,
                                  lagrangian.kNetCDF)
        map_properties = lagrangian.MapProperties(360, 180, -180, -90, 1)
        start = datetime.datetime(2010, 1, 1)
        end = datetime.datetime(2010, 3, 30)
        integration = lagrangian.FiniteLyapunovExponents(
            start,
            end,
            datetime.timedelta(days=1),
            lagrangian.kFSLE,
            0.1,
            0.05,
            ts)
        reader = lagrangian.Netcdf()
        reader.open(self.path)
        reader.load("Grid_0001")
        map_of_fsle = lagrangian.MapOfFiniteLyapunovExponents(
            map_properties,
            integration,
            netcdf_reader=reader)
        # lagrangian.set_verbose(True)
        map_of_fsle.compute()
        lambda1 = map_of_fsle.get_map_of_lambda1(0)
        lambda2 = map_of_fsle.get_map_of_lambda2(0)
        teta1 = map_of_fsle.get_map_of_theta1(0)
        teta2 = map_of_fsle.get_map_of_theta2(0)


if __name__ == "__main__":
    unittest.main()