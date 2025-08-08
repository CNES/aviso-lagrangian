# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify
# it under the terms of GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# lagrangian is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License
# along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
import datetime
import os
import pathlib
import unittest

import lagrangian

from . import BaseLagrangianTest


class TestTimeSerie(BaseLagrangianTest):

    def setUp(self):
        os.environ['ROOT'] = str(self.folder)
        self.ini = str(pathlib.Path(__file__).parent / 'map.ini')

    def test(self):
        ts = lagrangian.field.TimeSerie(
            self.ini,
            unit_type=lagrangian.UnitType.METRIC,
            reader_type=lagrangian.reader.Type.NETCDF)
        self.assertEqual(ts.unit_type, lagrangian.UnitType.METRIC)
        self.assertEqual(ts.coordinate_type,
                         lagrangian.CoordinatesType.SPHERICAL_EQUATORIAL)
        self.assertEqual(ts.unit(), 'm/s')
        self.assertEqual(ts.start_time(), datetime.datetime(2009, 12, 30))
        self.assertEqual(ts.end_time(), datetime.datetime(2010, 4, 7))
        start = datetime.datetime(2010, 1, 1)
        end = datetime.datetime(2010, 2, 1)
        with self.assertRaises(RuntimeError):
            u, v = ts.compute(start, 0.0, 0.0)
        ts.fetch(start, end)
        u, v = ts.compute(start, 0, 0)
        self.assertAlmostEqual(u, -0.22053186488468596, delta=1e-3)
        self.assertAlmostEqual(v, -0.06173804191683556, delta=1e-3)
        with self.assertRaises(IndexError):
            start = datetime.datetime(2015, 1, 1)
            u, v = ts.compute(start, 0, 0)


if __name__ == '__main__':
    unittest.main()
