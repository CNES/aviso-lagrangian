Building and installing
=======================

We will present how to compile the code, install and run the various scripts

This software uses the Python Distribution Utilities (“Distutils”) to build
and install this software. Please see the associated documentation on how to
install a `Python package <https://docs.python.org/2/install/>`_.

Build requirements
##################

Because programs are written in Python, libraries in C++ you must obviously
have Python and a C++ compiler installed on your system to use these programs.

The compiling C++ requires the following development libraries:

    * `boost_date_time <http://www.boost.org>`_
    * `boost_python <http://www.boost.org>`_
    * `boost_regexp <http://www.boost.org>`_
    * `boost_thread <http://www.boost.org>`_
    * `NetCDF <http://www.unidata.ucar.edu/software/netcdf>`_
    * `NetCDF-4 C++ <http://www.unidata.ucar.edu/downloads/netcdf/netcdf-cxx/index.jsp>`_
    * `UDUNITS-2 <http://www.unidata.ucar.edu/software/udunits>`_

You can install these packages on ubuntu by typing the following command:

.. code-block:: bash

    sudo apt-get install libboost-date-time-dev libboost-python-dev \
        libboost-regexp-dev libboost-thread-dev libnetcdf-dev \
        libudunits2-dev

The *NetCDF-4 C++* package is not yet distributed under Ubuntu, so you must
install it manually.

Configure
#########

Grab the latest tar.gz release file from the `download page
<https://bitbucket.org/cnes_aviso/lagrangian/downloads>`_. Once you have
satisfied the requirements detailed above, you can configure the build process
with :

.. code-block:: bash

    python setup.py setup

The configuration process allows you to set options to specify, for example,
the paths to the various required libraries. The available options are
displayed using the option ``python setup.py setup --help``.

For example, to compile the library under Mac OS X enter the following command
:

.. code-block:: bash

    python setup.py setup netcdf-includes=/opt/local/include netcdf-
    libraries=/opt/local/lib udunits-includes=/opt/local/include/udunits2
    boost_mt=1

You can check your build environment with :

.. code-block:: bash

    python setup.py config

Building
########

To build the library type the command ``python setup.py build`` at the root of
the project.

Install
#######

To install just type the command ``python setup.py``. You can specify an
alternate installation path, with:

.. code-block:: bash

    python setup.py install --prefix=/usr/local