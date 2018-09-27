map_of_fle
==========

The **map_of_fle** script computes a map of FSLE or FTLE from a set of NetCDF
grids containing velocity fields.

The files required to run this example are located in the
`data.zip <https://bitbucket.org/cnes_aviso/lagrangian/downloads/data.zip>`_
archive.

Then you must create a configuration file as follows:

.. code-block:: cfg

    U = dt_upd_global_merged_madt_uv_20091230_20091230_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100113_20100113_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100120_20100120_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100127_20100127_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100203_20100203_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100210_20100210_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100217_20100217_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100224_20100224_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100303_20100303_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100310_20100310_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100317_20100317_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100324_20100324_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100331_20100331_20110329.nc
    U = dt_upd_global_merged_madt_uv_20100407_20100407_20110329.nc
    V = dt_upd_global_merged_madt_uv_20091230_20091230_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100113_20100113_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100120_20100120_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100127_20100127_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100203_20100203_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100210_20100210_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100217_20100217_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100224_20100224_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100303_20100303_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100310_20100310_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100317_20100317_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100324_20100324_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100331_20100331_20110329.nc
    V = dt_upd_global_merged_madt_uv_20100407_20100407_20110329.nc
    U_NAME = Grid_0001
    V_NAME = Grid_0002
    FILL_VALUE = 0


The file contains several keys representing the following data:

* **U** defines the netCDF files containing the eastward velocities.
* **V** defines the netCDF files containing the northward velocities.
* **U_NAME** defines the NetCDF variable containing the eastward velocities.
* **V_NAME** defines the NetCDF variable containing the northward velocities.
* **FILL_VALUE** value to be taken into account when the reader encounters an
  undefined value. This value must be **0** if you do not wish to generate
  undefined values ​​when integrating or **nan** if the calculation must
  generate undefined values​​.

The path to the NetCDF file must contain an absolute path, for example:

.. code-block:: cfg

    U = /home/lagrangian/dt_upd_global_merged_madt_uv_20100407_20100407_20110329.nc

The path may also contain environment variables using the shell syntax, for
example:

.. code-block:: cfg

    U = ${DATA}/dt_upd_global_merged_madt_uv_20100407_20100407_20110329.nc

To compute a map of FSLE, in forwards mode, on selected area enter the
following command:

.. code-block:: bash

    map_of_fle.py list.ini fsle.nc "2010-01-01" --advection_time 89 --resolution=0.05 \
        --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
        --final_separation 0.2 --verbose --time_direction forward

.. image:: /images/fsle.png
    :scale: 50 %
    :align: center

or in backwards mode:

.. code-block:: bash

    map_of_fle.py list.ini fsle_backwards.nc "2010-03-31" --advection_time 89 \
        --resolution=0.05 --x_min 40 --x_max 60 \
        --y_min -60 --y_max -40 --final_separation 0.2 --verbose \
        --time_direction backward

.. image:: /images/fsle_backwards.png
    :scale: 50 %
    :align: center

Type ``map_of_fle.py --help`` to see the available options.


Troubleshooting
---------------

PYTHONPATH
**********

If you see this error message:

.. code-block:: text

    Traceback (most recent call last):
      File "map_of_fle.py", line 17, in <module>     import lagrangian
    ImportError: No module named lagrangian

You must set the ``PYTHONPATH`` variable with the directory that contains the
lagrangian library (lagrangian.so)

UDUNITS2_XML_PATH
*****************

If you see this error message:

.. code-block:: text

    Traceback (most recent call last):
    File "/home/lagrangian/anaconda3/lib/python3.6/multiprocessing/process.py", line 258, in _bootstrap
        self.run()
    File "/home/lagrangian/anaconda3/lib/python3.6/multiprocessing/process.py", line 93, in run
        self._target(*self._args, **self._kwargs)
    File "src/etc/map_of_fle.py", line 306, in calculation
        SYSTEM_UNITS[args.unit])
    File "src/wrapper/lagrangian.pyx", line 1244, in lagrangian.TimeSerie.__cinit__
        self.wrapped = new cpp_lagrangian.TimeSerie(
    RuntimeError: The variable UDUNITS2_XML_PATH is unset, and the installed, default unit, database couldn't be opened: No such file or directory

This means that you must position variable UDUNITS2_XML_PATH to the database
required for udunits2. For example: ::

    export UDUNITS2_XML_PATH=/home/lagrangian/anaconda3/share/udunits/udunits2.xml
