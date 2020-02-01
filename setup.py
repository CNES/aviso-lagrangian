#!/usr/bin/env python3
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
import pathlib
import distutils.command.build
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

WORKING_DIRECTORY = pathlib.Path(__file__).parent.absolute()


class SDist(distutils.command.sdist.sdist):
    """
    Creation of the version tag from information provided by mercurial
    """
    #: Path to file that contains the release number
    PATH = os.path.join("src", "lib", "trace.cpp")

    #: Pattern for searching the version number.
    PATTERN = re.compile(r'return "(.*)"').search

    #: Design to be replaced by the version number.
    VERSION = "__VERSION__"

    @classmethod
    def get_version(cls):
        """
        Read information tag from git
        """
        process = subprocess.Popen(
            "git describe --tags --dirty --long --always",
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)
        stdout, _ = process.communicate()
        pattern = re.compile(r'([\w\d\.]+)-(\d+)-g([\w\d]+)(?:-(dirty))?')

        # If the information is unavailable (execution of this function
        # outside the development environment), file generation is not
        # possible
        if not stdout:
            with open(cls.PATH) as stream:
                for item in stream:
                    match = cls.PATTERN(item)
                    if match is not None:
                        return match.group(1)
        match = pattern.search(stdout.decode("utf8"))
        return match.group(1)

    @classmethod
    def patch_trace_cpp(cls, src, dst):
        """
        Replace the string "__VERSION__" with the version tag read from
        mercurial
        """
        version = cls.get_version()
        os.unlink(dst)
        with open(src, 'r') as h_src:
            with open(dst, 'w') as h_dst:
                for line in h_src:
                    if cls.VERSION in line:
                        line = line.replace(cls.VERSION, version)
                    h_dst.write(line)

    def make_release_tree(self, base_dir, files):
        """
        Creating files list to insert in distribution package
        """
        distutils.command.sdist.sdist.make_release_tree(self, base_dir, files)
        for item in files:
            if item == self.PATH:
                dest = os.path.join(base_dir, item)
                self.patch_trace_cpp(item, dest)


def sources():
    """
    Returns the list of necessary files to build the extension
    """
    result = []
    cxx4 = os.path.join("third_party", "netcdf-cxx4", "cxx4")
    for item in os.listdir(cxx4):
        if item.startswith("nc") and item.endswith(".cpp"):
            result.append(os.path.join(cxx4, item))
    for root, dirs, files in os.walk(os.path.join(WORKING_DIRECTORY, 'src')):
        if 'wrapper' in dirs:
            dirs.remove('wrapper')
        for item in files:
            if item.endswith('.cpp'):
                path = os.path.join(root, item)
                result.append(os.path.relpath(path, WORKING_DIRECTORY))
    return result


def get_build_directory(dirname):
    """
    Returns the Python build directory name
    """
    return "{dirname}.{platform}-{version[0]}.{version[1]}".format(
        dirname=dirname,
        platform=sysconfig.get_platform(),
        version=sys.version_info)


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
        sources=[os.path.join("src", "wrapper", "core.pyx")] + sources(),
        extra_link_args=EXTRA_LINK_ARGS,
        libraries=["netcdf", "udunits2", "boost_date_time"],
        library_dirs=[os.path.join('build', get_build_directory('lib'))])
]

with open(os.path.join(os.path.abspath(os.path.dirname(__file__)),
                       'README.md')) as stream:
    LONG_DESCRIPTION = "".join(stream.readlines())

distutils.core.setup(
    name="lagrangian",
    version=SDist.get_version(),
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
    scripts=[
        os.path.join('src', 'etc', item)
        for item in os.listdir(os.path.join('src', 'etc'))
    ],
    url='https://bitbucket.org/cnes_aviso/lagrangian',
    classifiers=CLASSIFIERS,
    cmdclass={'sdist': SDist},
    ext_modules=Cython.Build.cythonize(
        EXTENSIONS,
        include_path=[
            os.path.join(WORKING_DIRECTORY, "src", "include"),
            os.path.join(WORKING_DIRECTORY, "src", "wrapper")
        ]))
