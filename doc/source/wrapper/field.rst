Velocity field
==============

UnitType
--------

.. py:class:: lagrangian.UnitType()

    Known unit velocity field

    .. py:data:: kAngular = lagrangian.UnitType.kAngular

        The field velocity is expressed in the angular system (eg. deg/s)

    .. py:data:: kMetric = lagrangian.UnitType.kMetric

        The field velocity is expressed in the metric system (eg. m/s)

ReaderType
----------

.. py:class:: lagrangian.ReaderType()

    Type of fields reader known

    .. py:data:: kNetCDF = lagrangian.ReaderType.kNetCDF

        The velocity field is read from NetCDF grids.

Field
-----

.. py:class:: lagrangian.Field(unit_type)

    Abstract class defining a field where it is possible to calculate a
    speed

    :param unit_type: Unit velocity field
    :type: :py:class:`lagrangian.UnitType`


    .. py:method:: Compute(t, x, y, u, v, cell)

        Calculate the value of the speed to the spatio temporal position
        requested.

        :param t: Time in number of seconds elapsed since 1970
        :type t: float
        :param x: Longitude in degree
        :type x: float
        :param y: Latitude in degree
        :type y: float
        :param cell: Cell properties of the grid used for the interpolation
        :type cell: :py:class:`lagrangian.CellProperties`
        :return: A tuple containing three values : a boolean indicating 
            whether the speed could be interpolated point requested, U
            or the velocity toward east interpolated and V or the velocity
            toward north interpolated
        :type: tuple

    .. py:method:: Fetch(t0, t1)

        Loads the data in the interval [t0, t1]

        :param t0: Begin date (number of seconds elapsed since 1/1/1970)
        :type t0: float
        :param t1: End date (number of seconds elapsed since 1/1/1970)
        :type t1: float


    .. py:method:: get_unit_type()

        Unit type used by this field.

        :return: unit type.
        :rtype: :py:class:`lagrangian.UnitType`


    .. py:method:: GetUnit()

        The string representation of the unit used by this field.

        :return: unit type
        :rtype: str

VonkarmanField
--------------

.. py:class:: lagrangian.VonkarmanField(a=1, w=35.06, r0=0.35, tc=1, alpha=2, y0=0.3, l=2, u0=14)

    Base: :py:class:`lagrangian.Field`

    TODO

TimeSerieField
--------------

.. py:class:: lagrangian.TimeSerieField(configuration_file, unit_type=lagrangian.UnitType.kMetric, reader_type=lagrangian.ReaderType.kNetCDF)

    Base: :py:class:`lagrangian.Field`

    Time series of velocity field

    :param configuration_file: The configuration file contains the list of
        files to take into account to interpolate speeds.
    :type configuration_file: str
    :param unit_type: Unit fields.
    :type unit_type: :py:class:`lagrangian.UnitType`
    :param reader_type: The reader used to read grids containing speeds.
    :type reader_type: :py:class:`lagrangian.ReaderType`

    .. py:method:: StartTime()

        Returns the date of the first grid constituting the time series.

        :return: the julian day of the first date
        :type: :py:class:`lagrangian.JulianDay`

    .. py:method:: EndTime()

        Returns the date of the last grid constituting the time series.

        :return: the julian day of the last date
        :type: :py:class:`lagrangian.JulianDay`
