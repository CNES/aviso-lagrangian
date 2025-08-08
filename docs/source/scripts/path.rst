path
====

Usage
-----

.. help_directive:: lagrangian.console_scripts.path
    :program_name: path

Overview
--------

The ``path`` program computes the drift (trajectory) of a set of points over a
geostrophic velocity field time series. It reads an input table of positions
(longitude, latitude) and advects them from a start time to an end time.

Quick start
-----------

.. code-block:: bash

    path list.ini buoys.txt "2010-01-01" "2010-01-02" --output tracks.tsv

- ``list.ini``: configuration of the velocity time series
- ``buoys.txt``: initial positions
- Start and end times in any ISO-like format (parsed with ``dateutil``)
- Output is written to ``tracks.tsv`` (tab-separated). Without ``--output``,
  results are printed to stdout.

Inputs
------

Configuration file
^^^^^^^^^^^^^^^^^^

Use a simple configuration file (one item per line) to describe the U/V time
series, similar to the one used by ``map_of_fle``:

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

- ``U``/``V``: paths to NetCDF files containing eastward/northward velocities
  for each time step (accepts absolute paths or ``${ENV}`` variables).
- ``U_NAME``/``V_NAME``: variable names inside the NetCDF files.
- ``FILL_VALUE``: value used for missing data (``0`` to avoid propagating
  missing values, or ``nan`` to propagate them).

If you need sample data, see
`data.zip <https://github.com/CNES/aviso-lagrangian/wiki/data.zip>`_ and the
`AVISO product page <https://www.aviso.altimetry.fr/en/data/products/auxiliary-products/merged-madt.html>`_.

Buoys file format
^^^^^^^^^^^^^^^^^

A plain text file with one point per line:

- First column: longitude in degrees (range -180..180)
- Second column: latitude in degrees (range -90..90)
- Lines may contain comments starting with ``#``
- Columns separated by whitespace (spaces or tabs)

Example:

.. code-block:: text

    # lon   lat
    0       0
    1.433333 43.6
    70      1

Command examples
----------------

.. code-block:: bash

    # Drift for 1 day, default 6 h time step
    path list.ini buoys.txt "2010-01-01" "2010-01-02"

    # Write output to a file
    path list.ini buoys.txt "2010-01-01" "2010-01-02" --output tracks.tsv

Options
-------

- ``--output FILE``: write results to a file (tab-separated). Defaults to
  stdout if omitted.
- ``--version``: print version and exit.

Positional arguments:

- ``configuration``: path to the configuration file.
- ``input``: path to the buoys file.
- ``start_time``: start time of the drift.
- ``end_time``: end time of the drift.

Notes
-----

- The internal integration time step is fixed to 6 hours.
- The start/end times must lie within the velocity time series; otherwise a
  runtime error is raised.
- Longitudes must be in [-180, 180] and latitudes in [-90, 90].

Output
------

Tab-separated values with the following columns on each printed time step:

- Index of the buoy (0-based)
- Longitude [degree]
- Latitude [degree]
- Timestamp in ISO format

Example output snippet:

.. code-block:: text

    0	0.000000	0.000000	2010-01-01T01:00:00
    1	1.433333	43.600000	2010-01-01T01:00:00
    0	-0.038031	-0.013479	2010-01-01T07:00:00

Help
----

Type ``path --help`` or ``path.py --help`` to see all options.

Troubleshooting
---------------

PYTHONPATH
^^^^^^^^^^

If you see this error message:

.. code-block:: text

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

See also
--------

- :doc:`map_of_fle`: compute FSLE/FTLE maps from the same velocity time series.
