map_of_fle
==========

The ``map_of_fle`` script computes global or regional maps of Finite-Size (FSLE)
or Finite-Time (FTLE) Lyapunov Exponents from a time series of velocity fields
stored in NetCDF files.

Usage
-----

.. help_directive:: lagrangian.console_scripts.map_of_fle
    :program_name: map_of_fle

Summary
-------

- Input: a list of NetCDF files providing eastward (U) and northward (V)
  velocities on a lon/lat grid over time.
- Output: a NetCDF file with variables ``lambda1``, ``lambda2`` (FLE/FTLE) and
  ``theta1``, ``theta2`` (eigenvector orientations), plus optional diagnostics.
- Modes: ``fsle`` (default) or ``ftle`` via ``--mode``.

Quick start
-----------

Example: FSLE map over a subdomain, forward in time, 0.05° resolution, 89 days
of advection and 0.2° final separation:

.. code-block:: bash

    map_of_fle list.ini fsle.nc "2010-01-01" \
      --mode fsle --time_direction forward --advection_time 89 \
      --resolution 0.05 --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
      --final_separation 0.2 --verbose

The output NetCDF will contain variables described in "Output" below.

Inputs
------

Data description
^^^^^^^^^^^^^^^^

The files required to run this example are located in the
`data.zip <https://github.com/CNES/aviso-lagrangian/wiki/data.zip>`_
archive. These files describe a time series of eastward and northward
velocities on a global grid. You can download up-to-date files from the
`AVISO website <https://www.aviso.altimetry.fr/en/data/products/auxiliary-products/merged-madt.html>`_.

Each file describes a single time step. The NetCDF header typically contains
variables similar to: ::

    dimensions:
      y = 915 ;
      x = 1080 ;
    variables:
      double y(y) ;
        y:long_name = "Latitudes" ;
        y:units = "degrees_north" ;
      double x(x) ;
        x:long_name = "Longitudes" ;
        x:units = "degrees_east" ;
      float u(y, x) ;
        u:_FillValue = 999.f ;
        u:long_name = "eastward velocity" ;
        u:units = "cm/s" ;
      float v(y, x) ;
        v:_FillValue = 999.f ;
        v:long_name = "northward velocity" ;
        v:units = "cm/s" ;

Note: Units like ``cm/s`` are metric. Keep ``--unit metric`` (default) for such
inputs. Use ``--unit angular`` only if your U/V are provided in angular units
(e.g., degrees per time).

Configuration file
^^^^^^^^^^^^^^^^^^

You must provide a simple ``.ini``-like configuration file listing the U and V
files and variable names. Example:

.. code-block:: cfg

    U = /path/to/dt_upd_global_merged_madt_uv_20091230_20091230_20110329.nc
    U = /path/to/dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc
    ... (one line per time slice)
    V = /path/to/dt_upd_global_merged_madt_uv_20091230_20091230_20110329.nc
    V = /path/to/dt_upd_global_merged_madt_uv_20100106_20100106_20110329.nc
    ... (one line per time slice)
    U_NAME = Grid_0001
    V_NAME = Grid_0002
    FILL_VALUE = 0

- ``U``: paths to NetCDF files containing eastward velocities (one per line).
- ``V``: paths to NetCDF files containing northward velocities (one per line).
- ``U_NAME``/``V_NAME``: variable names inside each NetCDF file for U and V.
- ``FILL_VALUE``: value to use when encountering missing data. Use ``0`` to
  avoid propagation of missing values, or ``nan`` if you want missing values to
  propagate through the computation.

Paths can be absolute or use environment variables, e.g.:

.. code-block:: cfg

    U = ${DATA}/dt_upd_global_merged_madt_uv_20100407_20100407_20110329.nc

Options and modes
-----------------

Common options
^^^^^^^^^^^^^^

- ``--resolution``: output grid step in degrees (default: 1.0). Also used as
  default ``--initial_separation`` if not provided.
- ``--x_min/x_max/y_min/y_max``: lon/lat bounds of the output grid.
- ``--integration_time_step``: particle integration time step in hours
  (default: 6).
- ``--unit``: velocity unit system (``metric`` or ``angular``), default
  ``metric``. Choose according to U/V variable units in your files.
- ``--mask PATH VARNAME``: a NetCDF grid; cells that are masked in this grid
  are skipped to speed up computation.
- ``--threads N``: number of CPU threads. ``0`` uses all CPUs; ``1`` disables
  multithreading.
- ``--verbose``: print debug information.

FSLE vs FTLE
^^^^^^^^^^^^

- ``--mode fsle`` (default): FSLE computation requires a
  ``--final_separation`` (in degrees). ``--advection_time`` sets the maximum
  advection time.
- ``--mode ftle``: FTLE computation ignores ``--final_separation`` and uses
  the exact ``--advection_time``.
- ``--time_direction``: ``forward`` or ``backward`` integration.

Constraints
^^^^^^^^^^^

- ``--final_separation`` is not allowed in FTLE mode and will raise an error.
- ``--initial_separation`` defaults to ``--resolution`` when unspecified.

Distributed execution (Dask)
----------------------------

You can split the computation by latitude chunks and distribute it with Dask:

- ``--local-cluster``: start a local Dask cluster automatically (for testing or
  single-node workflows).
- ``--scheduler_file PATH``: connect to an existing Dask cluster using a
  scheduler file.

Examples:

.. code-block:: bash

    # Local cluster
    map_of_fle list.ini fsle.nc "2010-01-01" --mode fsle \
      --time_direction forward --advection_time 89 --final_separation 0.2 \
      --resolution 0.05 --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
      --local-cluster --verbose

.. code-block:: bash

    # External cluster
    map_of_fle list.ini fsle.nc "2010-01-01" --mode fsle \
      --time_direction forward --advection_time 89 --final_separation 0.2 \
      --resolution 0.05 --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
      --scheduler_file /path/to/scheduler.json

Output
------

The output NetCDF contains:

- Dimensions: ``lon``, ``lat`` (note the variable dimensionality is
  ``(lon, lat)``).
- Coordinates:
  - ``lon`` [degrees_east]
  - ``lat`` [degrees_north]
- Main variables:

  - ``theta1`` [degree]: orientation of eigenvectors associated with maximum
    eigenvalues of the Cauchy–Green strain tensor.
  - ``theta2`` [degree]: orientation of eigenvectors associated with minimum
    eigenvalues of the Cauchy–Green strain tensor.
  - ``lambda1`` [1/day]: FLE/FTLE associated with maximum eigenvalues.
  - ``lambda2`` [1/day]: FLE/FTLE associated with minimum eigenvalues.
- Optional diagnostics (``--diagnostic``):

  - ``separation_distance`` [degree]: effective final separation distance.
  - ``advection_time`` [day]: actual advection time (FSLE mode only), counted
    since the provided start time.

Examples
--------

FSLE forward
^^^^^^^^^^^^

.. code-block:: bash

    map_of_fle.py list.ini fsle.nc "2010-01-01" --advection_time 89 \
      --resolution 0.05 --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
      --final_separation 0.2 --verbose --time_direction forward

.. image:: /images/fsle.png
    :scale: 50 %
    :align: center

FSLE backward
^^^^^^^^^^^^^

.. code-block:: bash

    map_of_fle.py list.ini fsle_backwards.nc "2010-03-31" --advection_time 89 \
      --resolution 0.05 --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
      --final_separation 0.2 --verbose --time_direction backward

.. image:: /images/fsle_backwards.png
    :scale: 50 %
    :align: center

FTLE example
^^^^^^^^^^^^

.. code-block:: bash

    map_of_fle list.ini ftle.nc "2010-01-01" --mode ftle --advection_time 60 \
      --resolution 0.05 --x_min 40 --x_max 60 --y_min -60 --y_max -40 \
      --verbose --time_direction forward

Masking land/invalid cells
^^^^^^^^^^^^^^^^^^^^^^^^^^

Use a land/sea mask (or any boolean mask) to skip cells and speed up run time:

.. code-block:: bash

    map_of_fle list.ini fsle.nc "2010-01-01" --advection_time 89 \
      --final_separation 0.2 --mask mask.nc land

Help
----

Type ``map_of_fle --help`` or ``map_of_fle.py --help`` to see all options.

Troubleshooting
---------------

PYTHONPATH
^^^^^^^^^^

If you see this error message:

.. code-block:: text

    Traceback (most recent call last):
      File "map_of_fle.py", line 17, in <module>     import lagrangian
    ImportError: No module named lagrangian

Set the ``PYTHONPATH`` environment variable to include the directory that
contains the compiled ``lagrangian`` module (``lagrangian.so``).

UDUNITS2_XML_PATH
^^^^^^^^^^^^^^^^^

If you see this error message:

.. code-block:: text

    RuntimeError: The variable UDUNITS2_XML_PATH is unset, and the installed, default unit, database couldn't be opened: No such file or directory

Set the UDUNITS2 database path, for example: ::

    export UDUNITS2_XML_PATH=/path/to/share/udunits/udunits2.xml

Dask cluster connection
^^^^^^^^^^^^^^^^^^^^^^^

- If ``--local-cluster`` hangs with zero workers, ensure the ``dask`` and
  ``distributed`` packages are installed in your environment.
- When using ``--scheduler_file``, verify that the file is readable and points
  to a reachable scheduler.

See also
--------

- :doc:`metric_to_angular`: convert metric velocity fields to angular units when
  needed.
- :doc:`path`: compute particle trajectories using the same time series inputs.
