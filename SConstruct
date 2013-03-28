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
import re
import os
import site
import distutils.sysconfig
import zipfile

import SCons
from SCons.Script.SConscript import SConsEnvironment

SConsEnvironment.Chmod = SCons.Action.ActionFactory(os.chmod,
    lambda dest, mode: 'Chmod("%s", 0%o)' % (dest, mode))


def permissions(env, dest, files, perm):
    obj = env.Install(dest, files)
    for item in obj:
        env.AddPostAction(item, env.Chmod(str(item), perm))
    return dest


def source_list(dir, mask='.*', replace=None):
    result = []
    regexp = re.compile(mask)
    for root, dirs, files in os.walk(dir):
        for f in files:
            if regexp.search(f):
                path = os.path.join(root, f)
                if replace:
                    path = path.replace(replace[0], replace[1])
                result.append(path)
    return result


def load_cfg(env):
    CFG = 'lagrangian.cfg'
    values = {}
    if os.path.exists(CFG):
        exec open(CFG, 'rU').read() in {}, values
        for key, value in values.iteritems():
            values = value.split(' ') if not isinstance(value, list) else value
            if len(values) == 1:
                env[key] = value
            else:
                env[key] = []
                for item in values:
                    env[key] += item.split()

def zipdist(target, source, env):
    file = zipfile.ZipFile("lagrangian.zip", mode="w")
    for item in source:
        src = str(item)
        dst = os.path.join("lagrangian", src)
        file.write(src, dst, zipfile.ZIP_DEFLATED)
    file.close()
    return None

EnsureSConsVersion(1, 2)

Help("""
To configure and verify that the prerequisites required for the construction
of the library type:

./configure.py [ OPTIONS ]

The following options allow you to define paths to the required libraries to
build the software:

    boost_includes      Location of boost C++ headers
    boost_libraries     Location of boost C++ libraries
    boost_mt            Set to 1 to use boost C++ libraries with multithreading
                        support enabled
    netcdf_includes     Location of NetCDF headers
    netcdf_libraries    Location of NetCDF libraries
    udunits_includes    Location of UDUNITS-2 headers
    udunits_librairies  Location of UDUNITS-2 libraries
""")

SConsEnvironment.InstallPerm = permissions

VariantDir('build', 'src')

env = Environment(CPPPATH=[distutils.sysconfig.get_python_inc()],
                  ENV=os.environ,
                  SHLIBPREFIX='')

load_cfg(env)

env.Decider('MD5-timestamp')

env.AppendUnique(CXXFLAGS=['-I./include', '-I.'])
print str(env['CXXFLAGS'])

dist_files = ['COPYING', 'configure.py', 'SConstruct', 'SConfigure']
dist_files += source_list('src', mask='\.(cpp|hpp|h)$')
dist_files += source_list('include', mask='\.(hpp|h)$')
dist_files += source_list('src/etc')

prefix = env['prefix'] if 'prefix' in env else site.PREFIXES[0]
bin_prefix = os.path.join(prefix, 'bin')
lib_prefix = distutils.sysconfig.get_python_lib()
lib_prefix = os.path.join(prefix, lib_prefix.replace(site.PREFIXES[1], '')[1:])

lagrangian = env.SharedLibrary(target='lagrangian',
                               SHLIBSUFFIX='.so',
                               source=source_list('src',
                                                  mask='\.cpp$',
                                                  replace=('src', 'build')))

for header in source_list('include', mask='\.(hpp|h)$'):
    env.Depends(lagrangian, header)

for header in source_list('src', mask='\.(hpp|h)$'):
    env.Depends(lagrangian, header)

env.Install(bin_prefix, 'src/etc/map_of_fle')
env.Install(bin_prefix, 'src/etc/mapping')
env.Install(bin_prefix, 'src/etc/path')
env.Install(lib_prefix, lagrangian[0])
env.Alias('install', bin_prefix)
env.Alias('install', lib_prefix)
env.InstallPerm(bin_prefix,
                ['src/etc/map_of_fle',
                 'src/etc/mapping',
                 'src/etc/path'],
                0555)
env.InstallPerm(lib_prefix, [lagrangian[0]], 0555)

env.Clean('distclean',
          ['.sconsign.dblite',
           '.sconf_temp',
           'config.log',
           'lagrangian.cfg',
           'lagrangian.zip'])
Clean('.', 'build')

if 'uninstall' in COMMAND_LINE_TARGETS:
    env.Command("uninstall", None, Delete(FindInstalledFiles()))

if 'zipdist' in COMMAND_LINE_TARGETS:
    env.Command("zipdist", dist_files, zipdist)
