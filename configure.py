#!/usr/bin/env python

# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# lagrangian.  If not, see <http://www.gnu.org/licenses/>.

import subprocess
import sys

argv = []

for item in sys.argv[1:]:
    try:
        ix = item.index('=')
    except ValueError:
        argv.append(item)
    else:
        argv.append("%s='%s'" % (item[:ix], item[ix+1:])) 

process = subprocess.Popen('scons --config=force -f SConfigure %s' %
                              ' '.join(argv), shell=True)
if process:
    process.wait()
    if process.returncode == 0:
        print "Type scons to build the project"
