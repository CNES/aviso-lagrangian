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
import unittest

import lagrangian


class Field(lagrangian.field.Python):

    def __init__(self):
        super().__init__(self)

    def compute(self, _, x, y):
        return (x * 1e-9, y * 1e-9)


class TestIntegration(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_integration(self):
        start = datetime.datetime(2000, 1, 1)
        end = datetime.datetime(2000, 12, 31)
        integration = lagrangian.Integration(start, end,
                                             datetime.timedelta(days=1),
                                             lagrangian.field.Vonkarman())
        x = []
        for item in integration.iterator():
            x.append(item)
        self.assertEqual(len(x), (end - start).days + 1)

        integration.fetch(x[2])
        it = integration.iterator()
        x1, y1 = integration.compute(it, 1, 1)
        self.assertAlmostEqual(x1, 13.29742324283439)
        self.assertAlmostEqual(y1, 1.492759792006086)

    def test_path_integration(self):
        start = datetime.datetime(2000, 1, 1)
        end = datetime.datetime(2000, 12, 31)
        integration = lagrangian.Path(start, end, datetime.timedelta(days=1),
                                      lagrangian.field.Vonkarman())
        x = []
        for item in integration.iterator():
            x.append(item)

        self.assertEqual(len(x), (end - start).days + 1)
        integration.fetch(x[2])
        it = integration.iterator()
        x1, y1 = integration.compute(it, 1, 1)
        self.assertAlmostEqual(x1, 13.29742324283439)
        self.assertAlmostEqual(y1, 1.492759792006086)

    def test_fle_integration(self):
        start = datetime.datetime(2000, 1, 1)
        end = datetime.datetime(2000, 12, 31)
        integration = lagrangian.FiniteLyapunovExponentsIntegration(
            start, end, datetime.timedelta(days=1),
            lagrangian.IntegrationMode.FSLE, 1, 0.5, Field())
        position = integration.set_initial_point(0.01, 0.01,
                                                 lagrangian.Stencil.TRIPLET)

        self.assertIsInstance(position, lagrangian.Position)
        self.assertIsInstance(position, lagrangian.Triplet)
        self.assertNotIsInstance(position, lagrangian.Quintuplet)
        self.assertFalse(integration.separation(position))
        a00, a01, a10, a11 = position.strain_tensor()
        self.assertEqual(a00, 0.5)
        self.assertEqual(a01, 0)
        self.assertEqual(a10, 0)
        self.assertEqual(a11, 0.5)

        it = integration.iterator()
        for value in it:
            self.assertTrue(
                integration.compute(it, position, lagrangian.CellProperties()))

        self.assertTrue(
            integration.compute(it, position, lagrangian.CellProperties()))
        fle = lagrangian.FiniteLyapunovExponents()
        self.assertTrue(integration.exponents(position, fle))
        self.assertAlmostEqual(fle.lambda1, 0)
        self.assertAlmostEqual(fle.lambda2, 0)
        self.assertAlmostEqual(fle.theta1, 90)
        self.assertAlmostEqual(fle.theta2, 0)
        self.assertAlmostEqual(fle.final_separation, 0.5161084399903032)
        self.assertAlmostEqual(fle.delta_t, 31622400)

        integration = lagrangian.FiniteLyapunovExponentsIntegration(
            start, end, datetime.timedelta(days=1),
            lagrangian.IntegrationMode.FSLE, 0.2, 0.5,
            lagrangian.field.Vonkarman())
        position = integration.set_initial_point(0, 0,
                                                 lagrangian.Stencil.TRIPLET)
        self.assertTrue(integration.separation(position))
        self.assertTrue(
            integration.compute(it, position, lagrangian.CellProperties()))
        fle = lagrangian.FiniteLyapunovExponents()
        self.assertTrue(integration.exponents(position, fle))


if __name__ == '__main__':
    unittest.main()
