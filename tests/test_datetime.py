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


class TestDateTime(unittest.TestCase):

    def test_init(self):
        date = datetime.datetime.now()
        self.assertEqual(date, lagrangian.DateTime(date).to_datetime())

        td = datetime.timedelta(1, 2, 3)
        self.assertEqual(td, lagrangian.TimeDuration(td).to_timedelta())


if __name__ == '__main__':
    unittest.main()
