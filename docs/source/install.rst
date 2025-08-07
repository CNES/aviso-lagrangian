Building and installing
=======================

Pre-built binaries
------------------

The easiest route to installing Lagrangion is through conda. For all platforms
installing this package can be done with:

.. code-block:: bash

    conda install -c conda-forge lagrangian

Building from source
--------------------

We will present how to compile the code, install and run the various scripts

This software uses the Python Distribution Utilities (â€œDistutilsâ€) to build
and install this software. Please see the associated documentation on how to
install a `Python package <https://docs.python.org/2/install/>`_.

Build requirements
##################

Because programs are written in Python, libraries in C++ you must obviously
have Python and a C++ compiler installed on your system to use these programs.

.. note::

   The C++ compiler must support the ISO C++ 2017 standard

The compiling C++ requires the following development libraries:

* `boost_date_time <http://www.boost.org>`_
* `NetCDF <http://www.unidata.ucar.edu/software/netcdf>`_
* `UDUNITS-2 <http://www.unidata.ucar.edu/software/udunits>`_

You can install these packages on ubuntu by typing the following command:

.. code-block:: bash

    sudo apt-get install libboost-date-time-dev libboost-python-dev \
        libboost-regex-dev libboost-thread-dev libnetcdf-dev \
        libudunits2-dev

Build
#####

Once you have satisfied the requirements detailed above, to build the library,
type the command ``python3 setup.py build`` at the root of the project.

You can specify, among other things, the following options:

* ``--boost-root``  to specify the preferred Boost installation prefix
* ``--cxx-compiler``  to specify the preferred C++ compiler
* ``--netcdf-dir``  to specify the preferred NETCDF installation prefix
* ``--reconfigure``  to force CMake to reconfigure this project
* ``--udunits2-root``  to specify the preferred UDUNITS-2 installation prefix

Run the ``python setup.py build --help`` command to view all the options
available for building the library.

Testing
#######

Requirements
------------

Running tests require the following Python libraries:

* `pytest <https://docs.pytest.org/en/latest/>`_
* `numpy <https://www.numpy.org/>`_
* `netCDF4 <https://unidata.github.io/netcdf4-python/>`_

Running tests
-------------

The distribution contains a set of test cases that can be processed with the
standard Python test framework. To run the full test suite,
use the following at the root of the project:

.. code-block:: bash

    python setup.py test

Install
#######

To install just type the command ``python setup.py``. You can specify an
alternate installation path, with:

.. code-block:: bash

    python setup.py install --prefix=/usr/local
