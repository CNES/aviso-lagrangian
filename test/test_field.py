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
import unittest
import os
import time
import lagrangian.core as core


class Field(core.PythonField):
    PASS = False

    def compute(self, t, x, y):
        Field.PASS = True
        return (x * 1.1, y * 1.1, t >= 0)


class BadField(core.PythonField):
    pass


class TestBadField(unittest.TestCase):
    def test(self):
        try:
            BadField()
        except NotImplementedError:
            pass
        else:
            self.fail('Did not see NotImplementedError')


class TestVonkarman(unittest.TestCase):
    def test(self):
        v = core.Vonkarman()
        result = v.compute(1, 1, 1)
        self.assertAlmostEqual(result[0], 7.1994043111518895)
        self.assertAlmostEqual(result[1], -5.766799394736108)


class TestCellProperties(unittest.TestCase):
    def test(self):
        cell = core.CellProperties()
        self.assertFalse(cell.contains(0, 0))
        cell.update(-1, 1, -1.5, 1.5, 0, 1, 2, 3)
        self.assertTrue(cell.contains(0, 0))
        self.assertTrue(cell.contains(1, 1))
        self.assertFalse(cell.contains(1.1, 1.1))
        self.assertEqual(cell.x0, -1)
        self.assertEqual(cell.x1, 1)
        self.assertEqual(cell.y0, -1.5)
        self.assertEqual(cell.y1, 1.5)
        self.assertEqual(cell.ix0, 0)
        self.assertEqual(cell.ix1, 1)
        self.assertEqual(cell.iy0, 2)
        self.assertEqual(cell.iy1, 3)


class TestRungeKutta(unittest.TestCase):
    def setUp(self):
        Field.PASS = False

    def test(self):
        self.assertFalse(Field.PASS)
        rk = core.RungeKutta(2, Field())
        result = rk.compute(1, 1, 1)
        self.assertTrue(Field.PASS)
        self.assertAlmostEqual(result[0], 1.0000197882850568)
        self.assertAlmostEqual(result[1], 1.0000197852709962)
        result = rk.compute(-1, 1, 1)
        self.assertEqual(result, None)

        rk = core.RungeKutta(2, core.Vonkarman())
        result = rk.compute(1, 1, 1)
        self.assertAlmostEqual(result[0], 1.0001294980342101)
        self.assertAlmostEqual(result[1], 0.9998962711974987)


class TestIterator(unittest.TestCase):
    def test(self):
        it = core.Iterator(0, 10, 0.25)
        x = []
        for value in it:
            x.append(value)
        y = [ix * 0.25 for ix in range(41)]
        self.assertEqual(x, y)


class TestPosition(unittest.TestCase):
    def test(self):
        position = core.Position()

        for method in ['get_xi', 'get_yi']:
            try:
                method = getattr(position, method)
                position.get_xi(0)
            except IndexError:
                pass
            else:
                self.fail("Did not see IndexError")

        self.assertEqual(position.time, 0)
        self.assertFalse(position.completed)
        position.set_completed()
        self.assertTrue(position.completed)
        self.assertTrue(position.is_missing())
        self.assertEqual(position.max_distance(), 0)

        self.assertTrue(position.compute(
            core.RungeKutta(2, core.Vonkarman()),
            core.Iterator(0, 10, 0.25),
            core.CellProperties()))


class TestTriplet(unittest.TestCase):
    def test(self):
        position = core.Triplet(0, 0, 1, 2)
        self.assertEqual(
            (position.get_xi(0), position.get_yi(0)),
            (0, 0))
        self.assertEqual(
            (position.get_xi(1), position.get_yi(1)),
            (1, 0))
        self.assertEqual(
            (position.get_xi(2), position.get_yi(2)),
            (0, 1))
        self.assertEqual(position.time, 2)
        self.assertFalse(position.completed)
        self.assertFalse(position.is_missing())
        self.assertAlmostEqual(position.max_distance(), 1)

        self.assertTrue(position.compute(
            core.RungeKutta(2, Field()),
            core.Iterator(0, 10, 0.25),
            core.CellProperties()))

        self.assertAlmostEqual(position.get_xi(0), 0)
        self.assertAlmostEqual(position.get_yi(0), 0)
        self.assertAlmostEqual(position.get_xi(1), 1.000019785271056)
        self.assertAlmostEqual(position.get_yi(1), 0)
        self.assertAlmostEqual(position.get_xi(2), 0)
        self.assertAlmostEqual(position.get_yi(2), 1.000019785271056)

        position.set_completed()
        self.assertTrue(position.completed)

        for method in ['get_xi', 'get_yi']:
            try:
                method = getattr(position, method)
                position.get_xi(3)
            except IndexError:
                pass
            else:
                self.fail("Did not see IndexError")


class TestQuintuplet(unittest.TestCase):
    def test(self):
        position = core.Quintuplet(0, 0, 1, 2)
        self.assertEqual(
            (position.get_xi(0), position.get_yi(0)),
            (0, 0))
        self.assertEqual(
            (position.get_xi(1), position.get_yi(1)),
            (1, 0))
        self.assertEqual(
            (position.get_xi(2), position.get_yi(2)),
            (0, 1))
        self.assertEqual(
            (position.get_xi(3), position.get_yi(3)),
            (-1, 0))
        self.assertEqual(
            (position.get_xi(4), position.get_yi(4)),
            (0, -1))

        self.assertEqual(position.time, 2)
        self.assertFalse(position.completed)
        self.assertFalse(position.is_missing())
        self.assertAlmostEqual(position.max_distance(), 1)

        for method in ['get_xi', 'get_yi']:
            try:
                method = getattr(position, method)
                position.get_xi(5)
            except IndexError:
                pass
            else:
                self.fail("Did not see IndexError")


class TestTimeSerie(unittest.TestCase):
    def setUp(self):
        os.environ['ROOT'] = os.path.dirname(__file__)
        self.ini = os.path.join(os.environ['ROOT'], 'map.ini')

    def test(self):
        ts = core.TimeSerie(self.ini,
                            core.kMetric,
                            core.kNetCDF)
        self.assertEqual(ts.coordinates_type, core.kSphericalEquatorial)
        self.assertEqual(ts.start_time(),
                         datetime.datetime(2009, 12, 30))
        self.assertEqual(ts.end_time(),
                         datetime.datetime(2010, 4, 7))
        start = time.mktime(datetime.datetime(2010, 1, 1).timetuple())
        end = time.mktime(datetime.datetime(2010, 2, 1).timetuple())
        try:
            u, v = ts.compute(start, 0, 0)
        except RuntimeError:
            pass
        else:
            self.fail("Did not see RuntimeError")
        ts.fetch(start, end)
        u, v = ts.compute(start, 0, 0)
        self.assertAlmostEqual(u, -0.22053186488468596)
        self.assertAlmostEqual(v, -0.06173804191683556)
        try:
            start = time.mktime(datetime.datetime(2015, 1, 1).timetuple())
            u, v = ts.compute(start, 0, 0)
        except IndexError:
            pass
        else:
            self.fail("Did not see IndexError")


if __name__ == "__main__":
    unittest.main()
