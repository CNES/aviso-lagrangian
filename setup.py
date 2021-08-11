#!/usr/bin/env python3

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
import distutils.command.build
import os
import pathlib
import platform
import re
import setuptools
import setuptools.command.build_ext
import setuptools.command.install
import setuptools.command.test
import shlex
import subprocess
import sys
import sysconfig

# Python version
MAJOR = sys.version_info[0]
MINOR = sys.version_info[1]

# Working directory
WORKING_DIRECTORY = pathlib.Path(__file__).parent.absolute()


def build_dirname(extname=None):
    """Returns the name of the build directory"""
    extname = '' if extname is None else os.sep.join(extname.split(".")[:-1])
    return str(
        pathlib.Path(WORKING_DIRECTORY, "build",
                     "lib.%s-%d.%d" % (sysconfig.get_platform(), MAJOR, MINOR),
                     extname))


class CMakeExtension(setuptools.Extension):
    """Python extension to build"""
    def __init__(self, name):
        super(CMakeExtension, self).__init__(name, sources=[])


class BuildExt(setuptools.command.build_ext.build_ext):
    """Build the Python extension using cmake"""

    #: Preferred BOOST root
    BOOST_ROOT = None

    #: Preferred C++ compiler
    CXX_COMPILER = None

    #: Run CMake to configure this project
    RECONFIGURE = None

    #: Preferred UDUNITS-2 root
    UDUNITS2_ROOT = None

    #: Preferred NetCDF prefix
    NETCDF_DIR = None

    def run(self):
        """A command’s raison d’etre: carry out the action"""
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    @staticmethod
    def boost():
        """Get the default boost path in Anaconda's environnement."""
        # Do not search system for Boost & disable the search for boost-cmake
        boost_option = "-DBoost_NO_SYSTEM_PATHS=TRUE " \
            "-DBoost_NO_BOOST_CMAKE=TRUE"
        boost_root = sys.prefix
        if os.path.exists(os.path.join(boost_root, "include", "boost")):
            return "{boost_option} -DBOOST_ROOT={boost_root}".format(
                boost_root=boost_root, boost_option=boost_option).split()
        boost_root = os.path.join(sys.prefix, "Library", "include")
        if not os.path.exists(boost_root):
            raise RuntimeError(
                "Unable to find the Boost library in the conda distribution "
                "used.")
        return "{boost_option} -DBoost_INCLUDE_DIR={boost_root}".format(
            boost_root=boost_root, boost_option=boost_option).split()

    @staticmethod
    def udunits2():
        """Get the default UDUNITS2 path in Anaconda's environnement."""
        root = sys.prefix
        if os.path.exists(os.path.join(root, "include", "udunits2.h")):
            return "-DUDUNITS2_ROOT=" + root
        root = os.path.join(sys.prefix, "Library", "include")
        if not os.path.exists(root):
            raise RuntimeError(
                "Unable to find the UDUNITS2 library in the conda "
                "distribution used.")
        return "-DUDUNITS2_ROOT=" + root

    @staticmethod
    def netcdf():
        """Get the default NETCDF path in Anaconda's environnement."""
        root = sys.prefix
        include_dir = os.path.join(root, "include", "netcdf.h")
        if os.path.exists(include_dir):
            return "-DNETCDF_DIR=" + root
        include_dir = os.path.join(sys.prefix, "Library", "include",
                                   "netcdf.h")
        if not os.path.exists(include_dir):
            raise RuntimeError(
                "Unable to find the NETCDF library in the conda distribution "
                "used.")
        return "-DNETCDF_DIR=" + os.path.join(sys.prefix, "Library")

    @staticmethod
    def is_conda():
        """Detect if the Python interpreter is part of a conda distribution."""
        result = os.path.exists(os.path.join(sys.prefix, 'conda-meta'))
        if not result:
            try:
                import conda
            except:
                result = False
            else:
                result = True
        return result

    def set_cmake_user_options(self):
        """Sets the options defined by the user."""
        is_conda = self.is_conda()
        result = []

        if self.CXX_COMPILER is not None:
            result.append("-DCMAKE_CXX_COMPILER=" + self.CXX_COMPILER)

        if self.BOOST_ROOT is not None:
            result.append("-DBOOST_ROOT=" + self.BOOST_ROOT)
        elif is_conda:
            result += self.boost()

        if self.UDUNITS2_ROOT is not None:
            result.append("-DUDUNITS2_ROOT=" + self.UDUNITS2_ROOT)
        elif is_conda:
            result.append(self.udunits2())

        if self.NETCDF_DIR is not None:
            result.append("-DNETCDF_DIR=" + self.NETCDF_DIR)
        elif is_conda:
            result.append(self.netcdf())

        if is_conda:
            result.append("-DCMAKE_PREFIX_PATH=" + sys.prefix)

        return result

    def build_cmake(self, ext):
        """Execute cmake to build the Python extension"""
        # These dirs will be created in build_py, so if you don't have
        # any python sources to bundle, the dirs will be missing
        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        extdir = build_dirname(ext.name)

        cfg = 'Debug' if self.debug else 'Release'

        cmake_args = [
            "-DCMAKE_BUILD_TYPE=" + cfg, "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" +
            str(extdir), "-DPYTHON_EXECUTABLE=" + sys.executable
        ] + self.set_cmake_user_options()

        build_args = ['--config', cfg]

        if self.verbose:
            os.environ["VERBOSE"] = "1"

        if platform.system() != 'Windows':
            build_args += ['--', '-j%d' % os.cpu_count()]
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            if platform.system() == 'Darwin':
                cmake_args += ['-DCMAKE_OSX_DEPLOYMENT_TARGET=10.14']
        else:
            cmake_args += [
                '-G', 'Visual Studio 15 2017',
                '-DCMAKE_GENERATOR_PLATFORM=x64',
                '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
                    cfg.upper(), extdir)
            ]
            build_args += ['--', '/m']

        os.chdir(str(build_temp))

        # Has CMake ever been executed?
        if pathlib.Path(build_temp, "CMakeFiles",
                        "TargetDirectories.txt").exists():
            # The user must force the reconfiguration
            configure = self.RECONFIGURE is not None
        else:
            configure = True

        if configure:
            self.spawn(['cmake', str(WORKING_DIRECTORY)] + cmake_args)

        if not self.dry_run:
            self.spawn(['cmake', '--build', '.', '--target', 'core'] +
                       build_args)
        os.chdir(str(WORKING_DIRECTORY))


class Build(distutils.command.build.build):
    """Build everything needed to install"""
    user_options = distutils.command.build.build.user_options
    user_options += [
        ('boost-root=', None, 'Preferred Boost installation prefix'),
        ('cxx-compiler=', None, 'Preferred C++ compiler'),
        ('netcdf-dir=', None, 'Preferred NETCDF installation prefix'),
        ('reconfigure', None, 'Forces CMake to reconfigure this project'),
        ('udunits2-root=', None, 'Preferred UDUNITS-2 installation prefix'),
    ]

    def initialize_options(self):
        """Set default values for all the options that this command supports"""
        super().initialize_options()
        self.boost_root = None
        self.cxx_compiler = None
        self.netcdf_dir = None
        self.reconfigure = None
        self.udunits2_root = None

    def run(self):
        """A command’s raison d’etre: carry out the action"""
        if self.boost_root is not None:
            BuildExt.BOOST_ROOT = self.boost_root
        if self.cxx_compiler is not None:
            BuildExt.CXX_COMPILER = self.cxx_compiler
        if self.netcdf_dir is not None:
            BuildExt.NETCDF_DIR = self.netcdf_dir
        if self.reconfigure is not None:
            BuildExt.RECONFIGURE = True
        if self.udunits2_root is not None:
            BuildExt.UDUNITS2_ROOT = self.udunits2_root
        super().run()


def execute(cmd):
    """Executes a command and returns the lines displayed on the standard
    output"""
    process = subprocess.Popen(cmd,
                               shell=True,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    return process.stdout.read().decode()


# def update_sphinx(date, version):
#     # Updating the version number description for sphinx
#     conf = pathlib.Path(WORKING_DIRECTORY, 'doc', 'source', 'conf.py')
#     with open(conf, "r") as stream:
#         lines = stream.readlines()
#     pattern = re.compile(r'(\w+)\s+=\s+(.*)')

#     for idx, line in enumerate(lines):
#         match = pattern.search(line)
#         if match is not None:
#             if match.group(1) == 'version':
#                 lines[idx] = "version = %r\n" % ".".join(
#                     version.split(".")[:-1])
#             elif match.group(1) == 'release':
#                 lines[idx] = "release = %r\n" % version
#             elif match.group(1) == 'copyright':
#                 lines[idx] = "copyright = '(%s, CLS)'\n" % date.year

#     with open(conf, "w") as stream:
#         stream.write("".join(lines))


def update_meta(version):
    """Updating the version number description in conda/meta.yaml."""
    path = pathlib.Path(WORKING_DIRECTORY, 'conda', 'meta.yaml')

    with open(path, "r") as stream:
        lines = stream.readlines()
    pattern = re.compile(r'{% set version = ".*" %}')

    for idx, line in enumerate(lines):
        match = pattern.search(line)
        if match is not None:
            lines[idx] = '{%% set version = "%s" %%}\n' % version

    with open(path, "w") as stream:
        stream.write("".join(lines))


def release():
    """Returns the software version"""
    trace_cpp = pathlib.Path(WORKING_DIRECTORY, 'src', 'lib', 'trace.cpp')
    stdout = execute("git describe --tags --dirty --long --always").strip()
    pattern = re.compile(r'([\w\d\.]+)-(\d+)-g([\w\d]+)(?:-(dirty))?')
    match = pattern.search(stdout)

    # If the information is unavailable (execution of this function outside the
    # development environment), file creation is not possible
    if not stdout:
        pattern = re.compile(r'\{\s*return\s+\"(.*)\"\s*;\s*\}')
        with open(trace_cpp, "r") as stream:
            for line in stream:
                match = pattern.search(line)
                if match:
                    return match.group(1)
        raise AssertionError("Unable to decode version in %r" % trace_cpp)

    assert match is not None, "No tags have been recorded."
    version = match.group(1)

    # stdout = execute("git log  %s -1 --format=\"%%H %%at\"" % sha1)
    # stdout = stdout.strip().split()
    # date = datetime.datetime.utcfromtimestamp(int(stdout[1]))

    update_meta(version)
    # update_sphinx(date, version)

    # Finally, write the file containing the version number.
    pattern = re.compile(r'.*Version.*\{\s+return\s+"(.*)";\s+\}')
    lines = []
    write = False

    with open(trace_cpp, 'r') as stream:
        for line in stream:
            match = pattern.search(line)
            if match:
                write = version != match.group(1)
                lines.append(
                    '  static auto Version() -> std::string { return "' +
                    version + '"; }\n')
                continue
            lines.append(line)
    if write:
        with open(trace_cpp, "w") as stream:
            stream.writelines(lines)
    return version


class Test(setuptools.command.test.test):
    """Test runner"""
    user_options = [("pytest-args=", None, "Arguments to pass to pytest")]

    def initialize_options(self):
        """Set default values for all the options that this command
        supports"""
        super().initialize_options()
        self.pytest_args = None

    def finalize_options(self):
        """Set final values for all the options that this command supports"""
        dirname = pathlib.Path(pathlib.Path(__file__).absolute().parent)
        rootdir = "--rootdir=" + str(dirname)
        if self.pytest_args is None:
            self.pytest_args = ''
        self.pytest_args = rootdir + " tests " + self.pytest_args

    def run_tests(self):
        """Run tests"""
        import pytest
        sys.path.insert(0, build_dirname())

        errno = pytest.main(
            shlex.split(self.pytest_args,
                        posix=platform.system() != 'Windows'))
        if errno:
            sys.exit(errno)


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


def long_description():
    """Reads the README file"""
    with open(pathlib.Path(WORKING_DIRECTORY, "README.md")) as stream:
        return stream.read()


def main():
    setuptools.setup(
        name='lagrangian',
        version=release(),
        description=("Lagrangian Coherent Structures and Finite-Time "
                     "Lyapunov Exponents"),
        url='https://github.com/CNES/aviso-lagrangian',
        author='CLS/LOCEAN',
        author_email="fbriol@cls.fr",
        classifiers=CLASSIFIERS,
        license="License :: OSI Approved :: "
        "GNU Library or Lesser General Public License (LGPL)",
        ext_modules=[CMakeExtension(name="lagrangian.core")],
        keywords="oceanography lagrangian analysis fsle ftle",
        long_description=long_description(),
        long_description_content_type='text/markdown',
        packages=setuptools.find_namespace_packages(
            where=str(pathlib.Path("src").absolute()),
            exclude=(
                "lib*",
                "core*",
                "include*",
            )),
        package_dir={"": "src"},
        entry_points={
            "console_scripts": [
                "map_of_fle=lagrangian.console_scripts.map_of_fle:main",
                "metric_to_angular=lagrangian.console_scripts."
                "metric_to_angular:main",
                "path=lagrangian.console_scripts.path:main"
            ]
        },
        cmdclass={
            'build': Build,
            'build_ext': BuildExt,
            'test': Test
        },
        zip_safe=False)


if __name__ == "__main__":
    main()
