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
import os
import platform
import sys
import tempfile
import unittest

import netCDF4
import numpy

import lagrangian

from . import SampleDataHandler


class TestConsoleScripts(unittest.TestCase):
    pos: str
    ini: str
    exe: str
    tst: str
    ref: str
    new: str

    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        lagrangian_module_dir = os.path.dirname(lagrangian.__file__)
        paths = list(sys.path)
        paths.insert(0, lagrangian_module_dir)
        sep = ';' if platform.system() == 'Windows' else ':'
        os.environ['PYTHONPATH'] = sep.join(paths)

        folder = SampleDataHandler.folder()
        os.environ['ROOT'] = str(folder)
        here = os.path.dirname(__file__)
        cls.pos = os.path.join(here, 'buoys.txt')
        cls.ini = os.path.join(here, 'map.ini')
        cls.exe = sys.executable
        with tempfile.NamedTemporaryFile() as stream:
            cls.tst = stream.name
        cls.ref = str(folder / 'fsle.nc')
        cls.new = os.path.join(tempfile.gettempdir(), 'fsle.nc')

    @classmethod
    def tearDownClass(cls) -> None:
        try:
            os.unlink(cls.tst)
            os.unlink(cls.new)
        except FileNotFoundError:
            pass

    def test_map_of_fle(self):
        from lagrangian.console_scripts.map_of_fle import main

        sys.argv = [
            'map_of_fle', self.ini, self.new, '2010-01-01',
            '--advection_time=89', '--resolution=0.05', '--x_min=40',
            '--x_max=59.95', '--y_min=-60', '--y_max=-40.05',
            '--final_separation=0.2', '--verbose', '--time_direction=forward'
        ]
        main()

        with netCDF4.Dataset(self.ref) as ref:
            with netCDF4.Dataset(self.new) as new:
                for item in ref.variables:
                    if item not in ['theta1', 'theta2', 'lambda1', 'lambda2']:
                        continue
                    x = ref.variables[item][:]
                    y = new.variables[item][:]
                    self.assertAlmostEqual(numpy.mean(x - y), 0, delta=1e-9)

    def test_path(self):
        from lagrangian.console_scripts.path import main

        with tempfile.NamedTemporaryFile(delete=False) as temp_output:
            output_file = temp_output.name

        sys.argv = [
            'path', self.ini, self.pos, '2010-01-01', '2010-01-02', '--output',
            output_file
        ]

        try:
            main()

            with open(output_file) as f:
                output_lines = [line.strip() for line in f.readlines()]

            expected = [
                item.strip()
                for item in """0\t0.000000\t0.000000\t2010-01-01T00:00:00
1\t1.433333\t43.600000\t2010-01-01T00:00:00
2\t2.000000\t2.000000\t2010-01-01T00:00:00
3\t4.000000\t4.000000\t2010-01-01T00:00:00
4\t8.000000\t8.000000\t2010-01-01T00:00:00
5\t16.000000\t16.000000\t2010-01-01T00:00:00
6\t32.000000\t32.000000\t2010-01-01T00:00:00
7\t64.000000\t64.000000\t2010-01-01T00:00:00
8\t70.000000\t12.000000\t2010-01-01T00:00:00
0\t-0.038031\t-0.013479\t2010-01-01T06:00:00
1\t1.433333\t43.600000\t2010-01-01T06:00:00
2\t1.991477\t1.978912\t2010-01-01T06:00:00
3\t4.022234\t3.996796\t2010-01-01T06:00:00
4\t8.000000\t8.000000\t2010-01-01T06:00:00
5\t16.000000\t16.000000\t2010-01-01T06:00:00
6\t32.000000\t32.000000\t2010-01-01T06:00:00
7\t64.000000\t64.000000\t2010-01-01T06:00:00
8\t70.012015\t12.020717\t2010-01-01T06:00:00
0\t-0.074492\t-0.026498\t2010-01-01T12:00:00
1\t1.433333\t43.600000\t2010-01-01T12:00:00
2\t1.983383\t1.957937\t2010-01-01T12:00:00
3\t4.044936\t3.993897\t2010-01-01T12:00:00
4\t8.000000\t8.000000\t2010-01-01T12:00:00
5\t16.000000\t16.000000\t2010-01-01T12:00:00
6\t32.000000\t32.000000\t2010-01-01T12:00:00
7\t64.000000\t64.000000\t2010-01-01T12:00:00
8\t70.024020\t12.041076\t2010-01-01T12:00:00
0\t-0.110257\t-0.038725\t2010-01-01T18:00:00
1\t1.433333\t43.600000\t2010-01-01T18:00:00
2\t1.975714\t1.937067\t2010-01-01T18:00:00
3\t4.068077\t3.991302\t2010-01-01T18:00:00
4\t8.000000\t8.000000\t2010-01-01T18:00:00
5\t16.000000\t16.000000\t2010-01-01T18:00:00
6\t32.000000\t32.000000\t2010-01-01T18:00:00
7\t64.000000\t64.000000\t2010-01-01T18:00:00
8\t70.036031\t12.061077\t2010-01-01T18:00:00
0\t-0.145335\t-0.050205\t2010-01-02T00:00:00
1\t1.433333\t43.600000\t2010-01-02T00:00:00
2\t1.968466\t1.916294\t2010-01-02T00:00:00
3\t4.091630\t3.989012\t2010-01-02T00:00:00
4\t8.000000\t8.000000\t2010-01-02T00:00:00
5\t16.000000\t16.000000\t2010-01-02T00:00:00
6\t32.000000\t32.000000\t2010-01-02T00:00:00
7\t64.000000\t64.000000\t2010-01-02T00:00:00
8\t70.048066\t12.080719\t2010-01-02T00:00:00""".split('\n') if item.strip()
            ]
            self.assertEqual(set(output_lines), set(expected),
                             'Output does not match expected results')
        finally:
            os.unlink(output_file)


if __name__ == '__main__':
    unittest.main()
