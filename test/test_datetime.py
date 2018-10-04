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
import lagrangian.core as core


class TestDateTime(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test(self):
        a = datetime.datetime.now()
        b = core.DateTime(a)()
        self.assertEqual(a, b)
        b = core.DateTime(a.isoformat(' '))()
        self.assertEqual(a, b)
        try:
            core.DateTime(1)
        except TypeError:
            pass
        else:
            self.fail('Did not see TypeError')
        self.assertTrue(isinstance(str(core.DateTime()), str))

if __name__ == "__main__":
    unittest.main()
