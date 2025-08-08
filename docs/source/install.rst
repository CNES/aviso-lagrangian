Building and installing
=======================

This page explains how to install, build from source, and test the project. For
most users, a conda-based environment (Miniforge or Mambaforge) is recommended
because it reliably provides all native dependencies.

.. note::

   Windows is supported, but only via a conda-based environment. Building
   against system-wide dependencies on Windows is not supported.

Building with Miniforge3
------------------------

.. important::

   If you are on Windows, use the conda-based workflow below. Other installation
   methods on Windows are not supported.

To install the prerequisites and set up the build environment using conda,
follow these steps:

#. Install Miniforge3 from the official source:
   https://github.com/conda-forge/miniforge
#. Clone the repository:
    .. code-block:: bash

        git clone https://github.com/CNES/aviso-lagrangian.git

#. Navigate to the project directory:
    .. code-block:: bash

        cd aviso-lagrangian

#. Create the build environment using the provided environment file:
    .. code-block:: bash

        mamba env create -f conda/environment.yml

    This will install all necessary dependencies for building, testing, and
    generating documentation.

#. Activate the environment:
    .. code-block:: bash

        mamba activate lagrangian

#. To install this software in the created environment, run:
    .. code-block:: bash

        pip install .

Building from Source
--------------------

.. warning::

   On Windows, building from source outside a conda environment is not
   supported. Use the conda-based installation described above.

This section explains how to compile the code, install it, and run the various
scripts.

This software uses Python's Setuptools for building and installation. For more
details, refer to the official documentation on
`Installing Python Modules <https://docs.python.org/3/install/>`_.

Build Requirements
##################

To build this software, you need Python and a C++ compiler installed on your
system. The C++ compiler must support the ISO C++ 2017 standard.

Additionally, the following development libraries are required:

* `Boost.Date_Time <http://www.boost.org>`_
* `NetCDF <http://www.unidata.ucar.edu/software/netcdf>`_
* `UDUNITS-2 <http://www.unidata.ucar.edu/software/udunits>`_

On Ubuntu, you can install these dependencies using the following command:

.. code-block:: bash

    sudo apt-get install libboost-date-time-dev libboost-python-dev \
        libboost-regex-dev libboost-thread-dev libnetcdf-dev \
        libudunits2-dev

Build Instructions
##################

Once all requirements are installed, you can build the library by running the
following command at the root of the project:

.. code-block:: bash

    python3 setup.py build

You can customize the build process by specifying additional options:

* ``--boost-root``: Specify the Boost installation prefix.
* ``--cxx-compiler``: Specify the C++ compiler to use.
* ``--netcdf-dir``: Specify the NetCDF installation prefix.
* ``--reconfigure``: Force CMake to reconfigure the project.
* ``--udunits2-root``: Specify the UDUNITS-2 installation prefix.

.. code-block:: bash

    python3 setup.py build_ext --inplace --boost-root=/path/to/boost \
        --cxx-compiler=g++ --netcdf-dir=/path/to/netcdf --reconfigure \
        --udunits2-root=/path/to/udunits2

To view all available build options, run:

.. code-block:: bash

    python3 setup.py build_ext --help

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

    pytest

Install
#######

To install the package, use ``pip`` . Run the following command at the root of
the project:

.. code-block:: bash

    pip install .

You can specify an alternate installation path with the ``--prefix`` option:

.. code-block:: bash

    pip install . --prefix=/usr/local
