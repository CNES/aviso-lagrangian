import datetime
import lagrangian
import unittest
import os


class Field(lagrangian.PythonField):
    def compute(self, t, x, y):
        return (x * 1e-9, y * 1e-9, t >= 0)


class TestIntegration(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def testIntegration(self):
        start = datetime.datetime(2000, 1, 1)
        end = datetime.datetime(2000, 12, 31)
        integration = lagrangian.Integration(start,
                                             end,
                                             datetime.timedelta(days=1),
                                             lagrangian.Vonkarman())
        it = integration.get_iterator()
        x = []
        for value in it:
            x.append(value)
        self.assertEqual(len(x), (end - start).days + 1)

        integration.fetch(x[2])
        it = integration.get_iterator()
        x1, y1 = integration.compute(it, 1, 1)
        self.assertAlmostEqual(x1, 13.29742324283439)
        self.assertAlmostEqual(y1, 1.492759792006086)

    def testIntegration(self):
        start = datetime.datetime(2000, 1, 1)
        end = datetime.datetime(2000, 12, 31)
        integration = lagrangian.Path(start,
                                      end,
                                      datetime.timedelta(days=1),
                                      lagrangian.Vonkarman())
        it = integration.get_iterator()
        x = []
        for value in it:
            x.append(value)
        self.assertEqual(len(x), (end - start).days + 1)

        integration.fetch(x[2])
        it = integration.get_iterator()
        x1, y1 = integration.compute(it, 1, 1)
        self.assertAlmostEqual(x1, 13.29742324283439)
        self.assertAlmostEqual(y1, 1.492759792006086)

    def testFiniteLyapunovExponents(self):
        start = datetime.datetime(2000, 1, 1)
        end = datetime.datetime(2000, 12, 31)
        integration = lagrangian.FiniteLyapunovExponents(
            start,
            end,
            datetime.timedelta(days=1),
            lagrangian.kFSLE,
            1,
            0.5,
            Field(lagrangian.kAngular))
        position = integration.set_initial_point(
            0.01,
            0.01,
            lagrangian.kTriplet)
        self.assertEqual(type(position), lagrangian.Position)
        self.assertFalse(integration.separation(position))
        a00, a01, a10, a11 = position.strain_tensor()
        self.assertEqual(a00, 0.5)
        self.assertEqual(a01, 0)
        self.assertEqual(a10, 0)
        self.assertEqual(a11, 0.5)

        it = integration.get_iterator()
        for value in it:
            self.assertTrue(integration.compute(
                it,
                position,
                lagrangian.CellProperties()))

        self.assertTrue(integration.compute(
            it,
            position,
            lagrangian.CellProperties()))
        self.assertTrue(integration.exponents(position))
        self.assertAlmostEqual(integration.lambda1, 0)
        self.assertAlmostEqual(integration.lambda2, 0)
        self.assertAlmostEqual(integration.theta1, 90)
        self.assertAlmostEqual(integration.theta2, 0)

        integration = lagrangian.FiniteLyapunovExponents(
            start,
            end,
            datetime.timedelta(days=1),
            lagrangian.kFSLE,
            0.2,
            0.5,
            lagrangian.Vonkarman())
        position = integration.set_initial_point(0, 0, lagrangian.kTriplet)
        self.assertTrue(integration.separation(position))
        self.assertTrue(integration.compute(
            it,
            position,
            lagrangian.CellProperties()))


class TestPath(unittest.TestCase):
    def setUp(self):
        os.environ['ROOT'] = os.path.dirname(__file__)
        self.ini = os.path.join(os.environ['ROOT'], 'path.ini')

    def test(self):
        ts = lagrangian.TimeSerie(self.ini,
                                  lagrangian.kMetric,
                                  lagrangian.kNetCDF)
        start = datetime.datetime(2010, 1, 1)
        end = datetime.datetime(2010, 3, 30)
        path = lagrangian.Path(
            start,
            end,
            datetime.timedelta(0, 6 * 60 * 60),
            ts)
        it = path.get_iterator()
        for time_step in it:
            path.fetch(time_step)
            self.assertTrue(isinstance(path.compute(it, 0, 0), tuple))
            self.assertTrue(path.compute(it, 1, 43) is None)


if __name__ == "__main__":
    unittest.main()
