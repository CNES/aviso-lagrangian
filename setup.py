"""
This script is the entry point for building, distributing and installing this
package using Distutils.
"""
# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See GNU Lesser General Public License for more
# details.
#
# You should have received a copy of GNU Lesser General Public License along
# with lagrangian. If not, see <http://www.gnu.org/licenses/>.
try:
    import configparser
except ImportError:
    import ConfigParser as configparser
import distutils.command.config
import distutils.command.sdist
import distutils.dist
import distutils.errors
import distutils.extension
import distutils.log
import distutils.version
import platform
import os
import re
import setuptools
import setuptools.command.install
import subprocess
import sysconfig
import sys
import Cython.Build


class SDist(distutils.command.sdist.sdist):
    """
    Creation of the version tag from information provided by mercurial
    """
    #: Path to file that contains the release number
    PATH = os.path.join("src", "lib", "trace.cpp")

    #: Pattern for searching the version number.
    PATTERN = re.compile(r'return "(.*)"').search

    #: Design to be replaced by the version number.
    VERSION = b"__VERSION__"

    @classmethod
    def get_version(cls):
        """
        Read information tag from mercurial
        """
        process = subprocess.Popen("LANG=C hg tags",
                                   shell=True,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        stdout, _ = process.communicate()

        for item in stdout.decode('utf8').split("\n"):
            if item and not item.startswith('tip'):
                return item.split()[0].strip().encode('utf8')
        with open(cls.PATH) as stream:
            for item in stream:
                match = cls.PATTERN(item)
                if match is not None:
                    result = match.group(1).encode("utf8")
                    if result != cls.VERSION:
                        return result
        return b'unofficial'

    @classmethod
    def patch_trace_cpp(cls, src, dst):
        """
        Replace the string "__VERSION__" with the version tag read from
        mercurial
        """
        version = cls.get_version()
        os.unlink(dst)
        with open(src, 'rb') as h_src:
            with open(dst, 'wb') as h_dst:
                for line in h_src:
                    if cls.VERSION in line:
                        line = line.replace(cls.VERSION, version)
                        if not isinstance(line, bytes):
                            line = line.encode('utf8')
                    h_dst.write(line)

    def make_release_tree(self, base_dir, files):
        """
        Creating files list to insert in distribution package
        """
        distutils.command.sdist.sdist.make_release_tree(self,
                                                        base_dir,
                                                        files)
        for item in files:
            if item == self.PATH:
                dest = os.path.join(base_dir, item)
                self.patch_trace_cpp(item, dest)


class SetupConfig(object):
    """
    Package configuration
    """
    CWD = os.path.dirname(os.path.abspath(__file__))

    def __init__(self):
        super(SetupConfig, self).__init__()
        self.path = os.path.join(self.CWD, 'setup.cfg')
        self.parser = configparser.ConfigParser()
        self.parser.read(self.path)

    @staticmethod
    def parse_boolean(v):
        """
        Parse a boolean value from a string
        """
        boolean_states = {
            '1': True, 'yes': True, 'true': True, 'on': True,
            '0': False, 'no': False, 'false': False, 'off': False
        }
        if v.lower() not in boolean_states:
            raise ValueError('Not a boolean: %s' % v)
        return boolean_states[v.lower()]

    def write(self):
        """
        Write the configuration file
        """
        with open(self.path, 'w') as handle:
            self.parser.write(handle)

    def get_build_ext(self):
        """
        Gets the parameters to build the extension
        """
        include_dirs, libraries, library_dirs = (None, None, None)
        if self.parser.has_section('build_ext'):
            if self.parser.has_option('build_ext', 'include_dirs'):
                include_dirs = self.parser.get('build_ext',
                                               'include_dirs')
                include_dirs = include_dirs.split(':') \
                    if include_dirs else None
            if self.parser.has_option('build_ext', 'libraries'):
                libraries = self.parser.get('build_ext',
                                            'libraries')
                libraries = libraries.split(' ') \
                    if libraries else None
            if self.parser.has_option('build_ext', 'library_dirs'):
                library_dirs = self.parser.get('build_ext',
                                               'library_dirs')
                library_dirs = library_dirs.split(':') \
                    if library_dirs else None

        return include_dirs, libraries, library_dirs

    def set_build_ext(self, include_dirs, libraries, library_dirs):
        """
        Sets the parameters to build the extension
        """
        if not self.parser.has_section('build_ext'):
            self.parser.add_section('build_ext')
        if library_dirs:
            self.parser.set('build_ext',
                            'library_dirs',
                            ":".join(library_dirs))
        elif self.parser.has_option('build_ext', 'library_dirs'):
            self.parser.remove_option('build_ext', 'library_dirs')
        include_dirs += ['src/include']
        try:
            import numpy.distutils.misc_util as np
            include_dirs += np.get_numpy_include_dirs()
        except ImportError:
            pass
        self.parser.set('build_ext',
                        'include_dirs',
                        ":".join(include_dirs))
        self.parser.set('build_ext',
                        'libraries',
                        " ".join(libraries))

    @classmethod
    def sources(cls):
        """
        Returns the list of necessary files to build the extension
        """
        result = []
        for root, dirs, files in os.walk(os.path.join(cls.CWD, 'src')):
            if 'wrapper' in dirs:
                dirs.remove('wrapper')
            for item in files:
                if item.endswith('.cpp'):
                    path = os.path.join(root, item)
                    result.append(os.path.relpath(path, cls.CWD))
        return result

    @classmethod
    def get_build_directory(cls, dirname):
        """
        Returns the Python build directory name
        """
        return "{dirname}.{platform}-{version[0]}.{version[1]}".format(
            dirname=dirname,
            platform=sysconfig.get_platform(),
            version=sys.version_info)


class Setup(setuptools.Command, SetupConfig):
    """
    Configuration commands
    """
    description = "Configuration commands"
    user_options = [
        ('boost-includes=', None,
         "Location of boost C++ headers"),
        ('boost-libraries=', None,
         "Location of boost C++ libraries"),
        ('boost-mt=', None,
         "Set to True to use boost C++ libraries with "
         "multithreading support enabled"),
        ('netcdf-includes=', None,
         "Location of NetCDF headers"),
        ('netcdf-libraries=', None,
         "Location of NetCDF libraries"),
        ('udunits-includes=', None,
         "Location of UDUNITS-2 headers"),
        ('udunits-librairies=', None,
         "Location of UDUNITS-2 libraries"),
    ]

    def __init__(self, *args, **kwargs):
        # super does not work because the class "setuptools.Command"
        # does not inherit of object.
        setuptools.Command.__init__(self, *args, **kwargs)
        SetupConfig.__init__(self)

    def initialize_options(self):
        """
        Initialize options to default values.
        """
        self.boost_includes = None
        self.boost_libraries = None
        self.boost_mt = str(sys.platform == 'darwin')
        self.netcdf_includes = None
        self.netcdf_libraries = None
        self.udunits_includes = None
        self.udunits_librairies = None

    def finalize_options(self):
        """
        Parse boolean values read from the command line.
        """
        self.boost_mt = self.parse_boolean(self.boost_mt)

    def run(self):
        """
        Set the values set by the user to build the library
        """
        # Needed boost libraries
        boost = ['boost_date_time']

        include_dirs = []
        library_dirs = []

        if self.boost_libraries:
            library_dirs.append(self.boost_libraries)
        if self.netcdf_libraries:
            library_dirs.append(self.netcdf_libraries)
        if self.udunits_librairies:
            library_dirs.append(self.udunits_librairies)

        if self.boost_includes:
            include_dirs.append(self.boost_includes)
        if self.netcdf_includes:
            include_dirs.append(self.netcdf_includes)
        if self.udunits_includes:
            include_dirs.append(self.udunits_includes)

        libraries = ['udunits2', "netcdf", "netcdf_c++4"]
        if self.boost_mt:
            libraries += ["%s-mt" % item for item in boost]
        else:
            libraries += boost

        self.set_build_ext(include_dirs, libraries, library_dirs)
        self.write()


class Config(distutils.command.config.config, SetupConfig):
    """
    Configuration commands
    """

    def __init__(self, *args, **kwargs):
        # old class style
        distutils.command.config.config.__init__(self, *args, **kwargs)
        SetupConfig.__init__(self)

    def run(self):
        """
        Verification of the development environment
        """
        include_dirs, libraries, library_dirs = self.get_build_ext()
        # Under Ubuntu sysconfig.get_path('include') does not return the path
        # to pyconfig.h
        include_py = os.path.dirname(sysconfig.get_config_h_filename())
        if include_dirs is None:
            include_dirs += []
        include_dirs.append(include_py)

        for header in ['netcdf.h', 'udunits2.h']:
            distutils.log.info('Checking for C header file %r' % header)
            if not self.check_header(header, include_dirs=include_dirs):
                raise distutils.errors.DistutilsPlatformError(
                    'Cannot find %r header.' % header)

        for header in ['netcdf',
                       os.path.join('boost', 'date_time.hpp'),
                       os.path.join('boost', 'version.hpp')]:
            distutils.log.info('Checking for C++ header file %r' % header)
            # Work around a bug of the "check_header" that does not take into
            # account the parameter "lang".
            # if not self.check_header(header, lang='c++'):
            if not self.try_cpp(body="/* No body */",
                                headers=[header],
                                include_dirs=include_dirs,
                                lang='c++'):
                raise distutils.errors.DistutilsPlatformError(
                    'Cannot find %r header.' % header)

        for library in libraries:
            distutils.log.info('Checking for library %r' % library)
            if not self.try_link("int main (void) { }",
                                 libraries=[library],
                                 library_dirs=library_dirs,
                                 lang='c++'):
                raise distutils.errors.DistutilsPlatformError(
                    'Cannot find library %r.' % library)


CLASSIFIERS = [
    'Description-Content-Type: text/markdown',
    'Development Status :: 5 - Production/Stable',
    'Intended Audience :: Science/Research',
    'OSI Approved :: GNU Lesser General Public License v3 or later (LGPLv3+)',
    'Platform: Linux',
    'Platform: Mac OS X',
    'Programming Language :: Python',
    'Programming Language :: Python :: 2',
    'Programming Language :: Python :: 3',
    'Topic :: Scientific/Engineering :: Physics',
]

EXTRA_COMPILE_ARGS = ['-std=c++11']
EXTRA_LINK_ARGS = []


if platform.system() == "Darwin":
    EXTRA_LINK_ARGS.append("-stdlib=libc++")
    if distutils.version.LooseVersion(platform.release()) >= \
            distutils.version.LooseVersion("18.0.0"):
        EXTRA_COMPILE_ARGS.append("-mmacosx-version-min=10.9")


EXTENSIONS = [
    distutils.extension.Extension(
        name='lagrangian.core',
        language='c++',
        extra_compile_args=EXTRA_COMPILE_ARGS,
        sources=[
            os.path.join("src", "wrapper", "core.pyx")
        ] + SetupConfig.sources(),
        extra_link_args=EXTRA_LINK_ARGS,
        library_dirs=[
            os.path.join(
                'build', SetupConfig.get_build_directory('lib'))]
    )
]


# Create the default setup configuration
SETUP = Setup(distutils.dist.Distribution())
if not os.path.exists(SETUP.path):
    SETUP.initialize_options()
    SETUP.run()

with open(
    os.path.join(
        os.path.abspath(os.path.dirname(__file__)),
        'README.md')) as stream:
    LONG_DESCRIPTION = "".join(stream.readlines())

distutils.core.setup(
    name="lagrangian",
    version=SDist.get_version().decode("utf8"),
    author="CLS/LOCEAN",
    author_email="fbriol@cls.fr",
    include_package_data=True,
    description=("Lagrangian Coherent Structures and Finite-Time "
                 "Lyapunov Exponents"),
    license="License :: OSI Approved :: "
            "GNU Library or Lesser General Public License (LGPL)",
    long_description=LONG_DESCRIPTION,
    keywords="oceanography lagrangian analysis fsle ftle",
    test_suite='test',
    install_requires=['netCDF4', 'numpy', 'python-dateutil'],
    setup_requires=['numpy'],
    packages=setuptools.find_packages(where='src'),
    package_dir={'lagrangian': os.path.join('src', 'lagrangian')},
    scripts=[os.path.join('src', 'etc', item)
             for item in os.listdir(os.path.join('src', 'etc'))],
    url='https://bitbucket.org/cnes_aviso/lagrangian',
    classifiers=CLASSIFIERS,
    cmdclass={
        'setup': Setup,
        'config': Config,
        'sdist': SDist
    },
    ext_modules=Cython.Build.cythonize(EXTENSIONS, include_path=[
        os.path.join(SetupConfig.CWD, "src", "include")
    ])
)
