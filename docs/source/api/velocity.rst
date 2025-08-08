Velocity Fields
===============

Classes for handling velocity field data from various sources and formats.

.. currentmodule:: lagrangian.core

Base Field Class
-----------------

.. class:: Field

    Base class for all velocity field implementations.

    .. automethod:: __init__

    **Key Methods**

    * :py:meth:`compute()`: Interpolate
      velocity at given time and position
    * :py:meth:`fetch()`: Load field data for
      specified time range
    * :py:meth:`unit()`: Get velocity units as
      string

    **Properties**

    * :py:attr:`coordinate_type`: Type of coordinate system used
    * :py:attr:`unit_type`: Type of units (metric or angular)

    ----

    .. automethod:: compute

    ----

    .. automethod:: fetch

    ----

    .. automethod:: unit

    ----

    .. autoproperty:: coordinate_type

    ----

    .. autoproperty:: unit_type

.. class:: Reader

    Base class for velocity field data readers.


.. currentmodule:: lagrangian.core.field

Field Implementations
---------------------

.. class:: Python

    Bases: :py:class:`lagrangian.Field`

    Python-based velocity field implementation allowing custom velocity
    functions.

    .. automethod:: __init__


.. class:: TimeSerie

    Bases: :py:class:`lagrangian.Field`

    Time-dependent velocity field loaded from configuration files.

    **Methods**

    * :py:meth:`start_time`: Get start time of available data
    * :py:meth:`end_time`: Get end time of available data

    **Examples**

    Loading a time series from NetCDF files::

        import lagrangian

        # Create field from configuration
        field = lagrangian.core.field.TimeSerie(
            configuration_file="velocity_config.ini",
            unit_type=lagrangian.UnitType.METRIC,
            coordinates_type=lagrangian.CoordinatesType.SPHERICAL_EQUATORIAL
        )

        # Get available time range
        start = field.start_time()
        end = field.end_time()

    ----

    .. automethod:: __init__

    ----

    .. automethod:: start_time

    ----

    .. automethod:: end_time


.. class:: Vonkarman

    Bases: :py:class:`lagrangian.Field`

    Analytical von Kármán vortex street velocity field.

    This provides an analytical representation of a von Kármán vortex street,
    useful for testing and validation of integration algorithms.


Data Readers
============

Classes for reading velocity field data from files.

.. currentmodule:: lagrangian.core.reader

.. class:: NetCDF

    Reader for NetCDF velocity field files.

    The grid must include at least one data variable and two coordinate vectors
    that define the longitude and latitude axes of the grid. For example: ::

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
           float u(x, y) ;
               u:_FillValue = 999f ;
               u:long_name = "U" ;
               u:units = "cm/s" ;
               u:date = "2012-01-01 00:00:00.000000 UTC" ;
            float v(y, x) ;
               v:_FillValue = 999f ;
               v:long_name = "V" ;
               v:units = "cm/s" ;
               v:date = "2012-01-01 00:00:00.000000 UTC" ;

    .. note::

        The variable to be read must set an attribute named ``date`` that
        define the date of data contained in the variable.

    .. automethod:: __init__

    **Key Methods**

    * :py:meth:`open()`: Open a NetCDF file
    * :py:meth:`load()`: Load a variable from the file
    * :py:meth:`interpolate()`: Interpolate data at given coordinates
    * :py:meth:`date()`: Get time information

    **Examples**

    Reading velocity data from NetCDF::

        import lagrangian

        reader = lagrangian.core.reader.NetCDF()
        reader.open("velocity_field.nc")
        reader.load("u", "m/s")  # Load u-component
        reader.load("v", "m/s")  # Load v-component

        # Interpolate velocity at specific location
        u_vel = reader.interpolate(lon=10.0, lat=45.0)

    ----

    .. automethod:: open

    ----

    .. automethod:: load

    ----

    .. automethod:: interpolate

    ----

    .. automethod:: date


.. class:: Type

    Enumeration of available reader types:

    * ``NETCDF``: NetCDF file reader
