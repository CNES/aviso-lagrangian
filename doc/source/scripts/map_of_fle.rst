map_of_fle
==========

The **map_of_fle** script computes a map of FSLE or FTLE from a set of NetCDF
grids containing velocity fields.

The files required to run this example are located in the ``test/data`` at the
root of source distribution.

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
 
To compute a map of FSLE, in forwards mode, on selected area enter the command
(don't forget to set ``OMP_NUM_THREADS`` to enable parallelization of code with
the number of threads defined.

.. code-block:: bash

    map_of_fle list.ini fsle.nc "2010-01-01 0" "2010-03-31 0" --step=0.05 \
        --max_separation 0.2 --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
        --delta 6 --verbose
 
or in backwards mode:

.. code-block:: bash

    map_of_fle list.ini fsle_backwards.nc "2010-03-31 0" "2010-01-01 0" \
        --step=0.05 --max_separation 0.2 --x_min 40 --x_max 60 --y_min -60 \
        --y_max -40 --delta 6 --verbose

Type ``map_of_fle --help`` to see the available options.

If you see this error message: ::

    Traceback (most recent call last):
      File "../src/etc/map_of_fle", line 17, in <module>     import lagrangian
    ImportError: No module named lagrangian
 
You must set the ``PYTHONPATH`` variable with the directory that contains the
lagrangian library (lagrangian.so)
