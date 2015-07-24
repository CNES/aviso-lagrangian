Python API
==========

.. autoclass:: lagrangian.LatitudeUnit()
    :show-inheritance:

     .. py:method:: __call__(unit)

        Checks if the unit can define this type of axis.

        :param unit: Unit to check
        :type unit: str
        :return: if the unit can define this type of axis
        :rtype: bool

.. autoclass:: lagrangian.LongitudeUnit()
    :show-inheritance:

     .. py:method:: __call__(unit)

        Checks if the unit can define this type of axis.

        :param unit: Unit to check
        :type unit: str
        :return: if the unit can define this type of axis
        :rtype: bool

.. py:class:: lagrangian.AxisType

    Type of axis

    .. py:data:: kUnknown

        Unknown axis type

    .. py:data:: kLatitude

        Latitude axis

    .. py:data:: kLongitude

        Longitude axis

    .. py:data:: kTime

        Temporal axis

    .. py:data:: kX

        X axis

    .. py:data:: kY

        Y axis

.. autoclass:: lagrangian.Axis(points, axis_type, unit=None)
    :show-inheritance:
    :members: type, is_regular, units, start, increment, __eq__, __ne__

    :param points: axis values
    :type points: numpy.ndarray
    :param axis_type: axis type
    :type axis_type: lagrangian.AxisType
    :param unit: Unit of the axis
    :type unit: str

    .. automethod:: lagrangian.Axis.get_coordinate_value(index)

        :param index: which coordinate. Between 0 and get_num_elements()-1 inclusive
        :type index: int
        :return: coordinate value
        :rtype: float


    .. automethod:: lagrangian.Axis.get_min_value()

        :return: minimum coordinate value
        :rtype: float


    .. automethod:: lagrangian.Axis.get_max_value()

        :return: maximum coordinate value
        :rtype: float

    .. automethod:: lagrangian.Axis.get_num_elements()

        :return: Get the number of values for this axis
        :rtype: int

    .. automethod:: lagrangian.Axis.find_index(coordinate)

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :return: the index of the axis if the point is contained in the axis
            or -1 if it is located outside of the axis
        :rtype: int

    .. automethod:: lagrangian.Axis.find_index_bounded(coordinate)

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :return: index of the grid point containing it or -1 if outside grid
            area
        :rtype: int

    .. automethod:: lagrangian.Axis.normalize(coordinate, circle)

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :param circle: value of the trigonometric circle in this coordinate system
        :type circle: float
        :return: longitude between: ::

            [self.get_min_value(), self.get_max_value() + circle]

        :rtype: float

    .. automethod:: lagrangian.Axis.convert(unit)

        :param unit: the new unit
        :type unit: str
    

    .. automethod:: lagrangian.Axis.find_indexes(coordinate)

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :return: `i0` and `i1` if the coordinate is inside the axis otherwise
            None
        :rtype: tuple

.. autoclass:: lagrangian.DateTime(value)
    :show-inheritance:

    :param value: Value used to build the new instance
    :type value: datetime.datetime, str

    .. py:method:: __call__()

        Get the Python representation of the instance

        :return: Python object that represents the instance
        :rtype: datetime.datetime

.. autoclass:: lagrangian.Field(*args)
    :show-inheritance:
    :members: unit_type

    .. py:method:: get_unit()

        Unit used by this field.

        :return: unit
        :rtype: str


.. autoclass:: lagrangian.PythonField(*args)
    :show-inheritance:


.. autoclass:: lagrangian.Vonkarman(a=1, w=35.06, r0=0.35, tc=1, alpha=2, y0=0.3, l=2, u0=14)
    :show-inheritance:

    .. automethod:: lagrangian.Vonkarman.compute(t, x, y)

        :param t: Time expressed as a number of seconds elapsed since 1970.
        :type t: float
        :param x: Longitude expressed as degree
        :type x: float
        :param y: Latitude expressed as degree
        :type y: float
        :return: u & v interpolated
        :rtype: tuple

.. autoclass:: lagrangian.CellProperties()
    :show-inheritance:

    .. automethod:: lagrangian.CellProperties.contains(x, y)

        :param x: Longitude
        :type x: float
        :param y: Latitude
        :type y: float
        :return: True if the coordinate is in the cell.
        :rtype: bool

    .. automethod:: lagrangian.CellProperties.update(x0, x1, y0, y1, ix0, ix1, iy0, iy1)

        :param x0: First longitude of the cell in degrees
        :type x0: float
        :param x1: Last longitude of the cell in degress
        :type x1: float
        :param y0: First latitude of the cell in degrees
        :type y0: float
        :param y1: Last latitude of the cell in degrees
        :type y1: float
        :param ix0: Index of the first longitude in the grid
        :type ix0: int
        :param ix1: Index of the last longitude in the grid
        :type ix1: int
        :param iy0: Index of the first latitude in the grid
        :type iy0: int
        :param iy1: Index of the last latitude in the grid
        :type iy1: int

    .. autoattribute:: lagrangian.CellProperties.iy1

        :return: The index of the last latitude
        :type: int

    .. autoattribute:: lagrangian.CellProperties.x0

        :return: The first longitude
        :type: float

    .. autoattribute:: lagrangian.CellProperties.x1

        :return: The last longitude
        :type: float

    .. autoattribute:: lagrangian.CellProperties.y0

        :return: The first latitude
        :type: float

    .. autoattribute:: lagrangian.CellProperties.y1

        :return: The last latitude
        :type: float

.. autoclass:: lagrangian.RungeKutta(size_of_interval, field)
    :show-inheritance:

    :param size_of_interval: Number of time interval
    :type size_of_interval: float
    :param field: Field reader
    :type field: lagrangian.Field

    .. automethod:: lagrangian.RungeKutta.compute(t, x, y, cell=None)

        :param t: Time in number of seconds elapsed since 1970
        :type t: float
        :param x: Longitude in degrees
        :type x: float
        :param y: Latitude in degrees
        :type y: float
        :param cell: Cell properties of the grid used for the interpolation
        :type cell: lagrangian.CellProperties
        :return: a tuple which contains the longitude and latitude after
            moving or None if the read field is undefined for the requested
            position
        :rtype: tuple

.. autoclass:: lagrangian.Iterator(begin, end, inc)
    :show-inheritance:

    :param begin: Begin of the period expressed in number of seconds
        elapsed since 1970
    :type begin: float
    :param end: End of the period expressed in number of seconds
        elapsed since 1970
    :type end: float
    :param inc: Time increment in seconds
    :type inc: float

    .. py:method:: __iter__()

        Iterate over the defined period

        :return: The number of seconds elapsed since 1970
        :rtype: float

.. autoclass:: lagrangian.Position()
    :show-inheritance:

    .. automethod:: lagrangian.Position.get_xi(idx)

        :param idx: position
        :type idx: int
        :return: The longitude in degrees
        :rtype: float

    .. automethod:: lagrangian.Position.get_yi(idx)

        :param idx: position
        :type idx: int
        :return: The latitude in degrees
        :rtype: float

    .. autoattribute:: lagrangian.Position.time

        :return: The time expressed in number of seconds elapsed since 1970
        :rtype: float

    .. autoattribute:: lagrangian.Position.completed

        :return: True if the integration is over
        :rtype: bool

    .. automethod:: lagrangian.Position.set_completed()

    .. automethod:: lagrangian.Position.missing()

    .. automethod:: lagrangian.Position.is_missing(self)

        :return: True if the integration is defined
        :rtype: bool

    .. automethod:: lagrangian.Position.max_distance()

        :return: The max distance
        :rtype: float

    .. automethod:: lagrangian.Position.compute(rk, it, cell)

        :param rk: Runge-Kutta handler
        :type rk: lagrangian.RungeKutta
        :param it: Iterator
        :type it: lagrangian.Iterator
        :param cell: Cell properties of the grid used for the interpolation.
        :type cell: lagrangian.CellProperties
        :return: True if the particle could be moved otherwise false
        :rtype: bool

    .. automethod:: lagrangian.Position.strain_tensor()


.. autoclass:: lagrangian.Triplet(x, y, delta)
    :show-inheritance:

    :param x: Longitude of the initial point
    :type x: float
    :param y: Latitude of the initial point
    :type y: float
    :param delta: Initial initial separation in degrees of neighboring
        particles
    :type delta: float


.. autoclass:: lagrangian.Quintuplet(x, y, delta)
    :show-inheritance:

    :param x: Longitude of the initial point
    :type x: float
    :param y: Latitude of the initial point
    :type y: float
    :param delta: Initial initial separation in degrees of neighboring
        particles
    :type delta: float


.. py:class:: lagrangian.Stencil

    Type of stencils known

    .. py:attribute:: kTriplet

        Define a stencil with 3 points

    .. py:attribute:: kQuintuplet

        Define a stencil with 5 points

.. autoclass:: lagrangian.AbstractIntegration(*args)

    .. automethod:: lagrangian.Integration.get_iterator()

        :return: The iterator
        :rtype: :py:class:`lagrangian::Iterator`

    .. automethod:: lagrangian.Integration.fetch(t)

        :param t: t Time step in seconds
        :type t: float

    .. automethod:: lagrangian.Integration.compute(it, x0, x1)

        :param it: Iterator
        :type it: :py:class:`lagrangian.Iterator`
        :param x0: Longitude in degrees
        :type x0: float
        :param y0: Latitude in degrees
        :type y0: float
        :return: A tuple that contains the new position (x1, y1) of the
            particle or None if the read field is undefined for the requested
            position
        :rtype: tuple

.. autoclass:: lagrangian.Integration(start_time, end_time, delta_t, field)
    :show-inheritance:

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.DateTime`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.DateTime`
    :param delta_t: Time interval, in seconds
    :type delta_t: :py:class:`datetime.timedelta`
    :param field: Field to use for computing the velocity of a point.
    :type field: :py:class:`lagrangian.Field`


.. autoclass:: lagrangian.Path(start_time, end_time, delta_t, field)
    :show-inheritance:

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.DateTime`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.DateTime`
    :param delta_t: Time interval, in seconds
    :type delta_t: :py:class:`datetime.timedelta`
    :param field: Field to use for computing the velocity of a point.
    :type field: :py:class:`lagrangian.Field`


.. py:class:: lagrangian.Mode

    Mode of integration

    .. py:attribute:: kFSLE

        Finite Size Lyapunov Exponent

    .. py:attribute:: kFTLE

        Finite Time Lyapunov Exponent

.. autoclass:: lagrangian.FiniteLyapunovExponents(start_time, end_time, delta_t, mode, min_separation, delta, field)
    :show-inheritance:

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.DateTime`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.DateTime`
    :param delta_t: Time interval, in seconds
    :type delta_t: :py:class:`datetime.timedelta`
    :param mode: Integration mode
    :type mode: :py:class:`lagrangian.Mode`
    :param min_separation: Minimal separation in degrees
    :type min_separation: float
    :param delta: The gap between two consecutive dots, in degrees, of the grid
    :type delta: float
    :param field: Field to use for computing the velocity of a point.
    :type field: :py:class:`lagrangian.Field`

    .. automethod:: lagrangian.FiniteLyapunovExponents.set_initial_point(y, y, stencil)

        :param x: Longitude in degrees
        :type x: float
        :param y: Latitude in degrees
        :type y: float
        :param stencil: The type of stencil choosen
        :type stencil: lagrangian.Stencil
        :return: The position of the initial point
        :rtype: :py:class:`lagrangian.Position`

    .. autoattribute:: lagrangian.FiniteLyapunovExponents.mode

        :return: The mode of the integration
        :rtype: lagragian.Mode

    .. automethod:: lagrangian.FiniteLyapunovExponents.compute(it, position, cell)

        :param it: Iterator
        :type it: lagrangian.Iterator
        :param position: Position of the particle
        :type position: lagrangian.Position
        :param cell: Cell properties of the grid used for the interpolation
        :type cell: lagrangian.CellProperties
        :return: True if the integration is defined otherwise false
        :rtype: bool

    .. automethod:: lagrangian.FiniteLyapunovExponents.exponent(position)

        :param position: Position of the particle
        :type position: lagrangian.Position
        :return: True if the exponents are defined
        :rtype: bool

    .. autoattribute:: lagrangian.FiniteLyapunovExponents.lambda1

        :return: λ₁ (unit 1/day)
        :type: float

    .. autoattribute:: lagrangian.FiniteLyapunovExponents.lambda2

        :return: λ₂ (unit 1/day)
        :type: float

    .. autoattribute:: lagrangian.FiniteLyapunovExponents.theta1

        :return: θ₁ (unit degrees)
        :type: float

    .. autoattribute:: lagrangian.FiniteLyapunovExponents.theta2

        :return: θ₂ (unit degrees)
        :type: float

.. autoclass:: lagrangian.Netcdf()

    .. py:method:: Interpolate(longitude, latitude, fill_value=0, cell=lagrangian.CellProperties.NONE)

        Computes the velocity of the grid point requested

        :param longitude: Longitude in degrees.
        :type longitude: float
        :param latitude: Latitude in degrees
        :type latitude: float
        :param fill_value: Value to be taken into account for fill values
        :type fill_value: float
        :param cell: Cell properties of the grid used for the interpolation.
        :type cell: lagrangian.CellProperties

    .. py:method:: Load(name, unit)

        Load data into memory

        :param name: Name of the grid who contains data
        :type name: str
        :param unit: Unit of data loaded into memory.
        :type unit: str

    .. py:method:: Open(filename)

        Opens a file in read-only.

        :param filename: Path to the grid
        :type filename: str


.. autoclass:: lagrangian.PythonReader()


.. py:class:: lagrangian.ReaderType

    Type of fields reader known

    .. py:attribute:: kNetCDF

        The velocity field is read from NetCDF grids.

.. autoclass:: lagrangian.Factory()

    .. automethod:: lagrangian.Factory.NewReader(reader_type)

        :param reader_type: Name of the grid who contains data
        :type reader_type: Reader type
        :return: An instance of a reader
        :rtype: lagrangian.AbstractReader

.. autoclass:: lagrangian.TimeSerie(configuration_file, unit_type=lagrangian.kMetric, reader_type=lagrangian.kNetCDF)
    :show-inheritance:

    :param configuration_file: The configuration file contains the list of
        files to take into account to interpolate speeds.
    :type configuration_file: str
    :param unit_type: Unit fields.
    :type unit_type: lagrangian.UnitType
    :param reader_type: The reader used to read grids containing speeds.
    :type reader_type: lagrangian.ReaderType

    .. automethod:: lagrangian.TimeSerie.start_time()

        :return: the date of the first date
        :type: datetime.datetime

    .. automethod:: lagrangian.TimeSerie.end_time()

        :return: the date of the last date
        :type: datetime.datetime

