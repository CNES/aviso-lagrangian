# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See GNU Lesser General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License along
# with lagrangian. If not, see <http://www.gnu.org/licenses/>.
import datetime
import math
import os
import unittest
import lagrangian.core as core


class BadReader(core.PythonReader):
    pass


class Reader(core.PythonReader):
    def open(self, filename):
        pass

    def load(self, name, unit):
        pass

    def interpolate(self, longitude, latitude, fill_value, cell):
        return fill_value

    def get_datetime(self, varname):
        return datetime.datetime.now()


class TestNetcdf(unittest.TestCase):
    def setUp(self):
        self.path = os.path.join(
            os.path.dirname(__file__),
            'data',
            'dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc')

    def test(self):
        reader = core.Netcdf()
        reader.open(self.path)
        reader.load("Grid_0001")

        self.assertEqual(reader.interpolate(-90, -180), 0)
        self.assertTrue(math.isnan(reader.interpolate(-90, -180,
                        float("nan"))))

        self.assertEqual(reader.interpolate(0, 100), 0)
        self.assertTrue(math.isnan(reader.interpolate(0, 100,
                        float("nan"))))

        self.assertAlmostEqual(reader.interpolate(0, 0),
                               -14.691391615783461)
        reader.load("Grid_0001", "m/s")
        self.assertAlmostEqual(reader.interpolate(0, 0),
                               -0.146913916157834)

        self.assertEqual(reader.get_datetime('Grid_0001'),
                         datetime.datetime(2010, 1, 6))

        try:
            reader.load("Grid_0001", "degree")
        except RuntimeError:
            pass
        else:
            self.fail("Did not see RuntimeError")

        try:
            reader.load("X")
        except RuntimeError:
            pass
        else:
            self.fail("Did not see RuntimeError")

        try:
            reader.open("")
        except RuntimeError:
            pass
        else:
            self.fail("Did not see RuntimeError")


class TestBadReader(unittest.TestCase):
    def test(self):
        try:
            BadReader()
        except NotImplementedError:
            pass
        else:
            self.fail('Did not see NotImplementedError')


class TestReader(unittest.TestCase):
    def test(self):
        Reader()
        # TODO Write tests case when methods using C ++ readers will be
        # exposed


class TestFactory(unittest.TestCase):
    def setUp(self):
        self.path = os.path.join(
            os.path.dirname(__file__),
            'data',
            'dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc')

    def test(self):
        reader = core.Factory.new_reader(core.kNetCDF)
        self.assertTrue(type(reader).__name__, 'Reader')
        reader = core.Netcdf()
        reader.open(self.path)
        reader.load("Grid_0001")


if __name__ == "__main__":
    unittest.main()
