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

import sys
import re
import os
import site
import distutils.sysconfig
import zipfile

import SCons
from SCons.Script.SConscript import SConsEnvironment
from zipfile import ZipFile

SConsEnvironment.Chmod = SCons.Action.ActionFactory(os.chmod,
    lambda dest, mode: 'Chmod("%s", 0%o)' % (dest, mode))

def permissions(env, dest, files, perm):
    obj = env.Install(dest, files)
    for item in obj:
        env.AddPostAction(item, env.Chmod(str(item), perm))
    return dest

def source_list(dir, mask='.*'):
    result = []
    regexp = re.compile(mask)
    for root, dirs, files in os.walk(dir):
        for f in files:
            if regexp.search(f):
                result.append(os.path.join(root, f))
    return result

def load_cfg(env):
    CFG='lagrangian.cfg'
    values = {}
    if os.path.exists(CFG):
        exec open(CFG, 'rU').read() in {}, values
        for key, value in values.iteritems():
            env[key] = value

def zipdist(target, source, env):
    file = zipfile.ZipFile("lagrangian.zip", mode="w")
    for item in source:
        item = str(item)
        file.write(item, item, zipfile.ZIP_DEFLATED)
    file.close()
    return None

EnsureSConsVersion(1, 2)

SConsEnvironment.InstallPerm = permissions

env = Environment(CPPPATH=[distutils.sysconfig.get_python_inc()],
                  SHLIBPREFIX='')

load_cfg(env)

env.Decider('MD5-timestamp')

env.AppendUnique(CXXFLAGS=['-I./include', '-I.'])
env.AppendUnique(LIBS=['boost_python',
                       'boost_date_time',
                       'boost_regex',
                       'boost_thread',
                       'udunits2',
                       'netcdf',
                       'netcdf_c++'])

Progress(['-\r', '\\\r', '|\r', '/\r'], interval = 5)

dist_files = ['COPYING', 'configure.py', 'SConstruct', 'SConfigure']
dist_files += source_list('src', mask='\.(cpp|hpp|h)$')
dist_files += source_list('include', mask='\.(hpp|h)$')
dist_files += source_list('src/etc')

prefix = env['prefix'] if env.has_key('prefix') else site.PREFIXES[0]
bin_prefix = os.path.join(prefix, 'bin')
lib_prefix = distutils.sysconfig.get_python_lib()
lib_prefix = os.path.join(prefix, lib_prefix.replace(site.PREFIXES[1], '')[1:])

lagrangian = env.SharedLibrary(target='lagrangian',
                               source=source_list('src', mask='\.cpp$'))
env.Install(bin_prefix, 'src/etc/map_of_fle')
env.Install(bin_prefix, 'src/etc/mapping')
env.Install(lib_prefix, lagrangian[0])
env.Alias('install', bin_prefix)
env.Alias('install', lib_prefix)
env.InstallPerm(bin_prefix, ['src/etc/map_of_fle', 'src/etc/mapping'], 0555)
env.InstallPerm(lib_prefix, [lagrangian[0]], 0555)
env.Command("uninstall", None, Delete(FindInstalledFiles()))
env.Command("zipdist", dist_files, zipdist)