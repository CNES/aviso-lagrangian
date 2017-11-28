Python API
==========

.. py:module:: lagrangian

Library allows to compute Lagrangian analysis with Python.

Physical quantity units
-----------------------

.. py:class:: lagrangian.LatitudeUnit()

    Bases: object

     .. py:method:: __call__(unit)

        Checks if the unit can define this type of axis.

        :param unit: Unit to check
        :type unit: str
        :return: if the unit can define this type of axis
        :rtype: bool

.. py:class:: lagrangian.LongitudeUnit()

    Bases: object

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

Spatial and temporal axes
-------------------------

.. py:class:: lagrangian.Axis(points, axis_type, unit=None)

    Bases: object

    A coordinate axis is a Variable that specifies one of the coordinates
    of a Variable's values.

    Mathematically it is a vector function F from index space to Sn: ::

        F(i, j, k,...) -> (S1, S2, ...Sn)

    where i, j, k are integers, and S is the set of reals (R).

    The components of F are just its coordinate axes: ::

        F = (A1, A2, ...An)
         A1(i, j, k, ...) -> S1
         A2(i, j, k, ...) -> S1
         An(i, j, k, ...) -> Sn

    :param points: axis values
    :type points: numpy.ndarray
    :param axis_type: axis type
    :type axis_type: lagrangian.AxisType
    :param unit: Unit of the axis
    :type unit: str

    .. py:attribute:: increment

        Get increment value if is_regular

    .. py::attribute is_regular

        The axis values are spaced regularly. Return true if:

            value(i) = self.start + i * self.increment

    .. py:attribute:: start

        Get starting value if is_regular

    .. py:attribute:: type

        Get type of axis

    .. py:attribute units

        Get the unit string for this axis

    .. py:method:: lagrangian.Axis.get_coordinate_value(index)

        Get the ith coordinate value.

        :param index: which coordinate. Between 0 and get_num_elements()-1 inclusive
        :type index: int
        :return: coordinate value
        :rtype: float

    .. py:method:: lagrangian.Axis.get_min_value()

        Get the minimum coordinate value

        :return: minimum coordinate value
        :rtype: float

    .. py:method:: lagrangian.Axis.get_max_value()

        Get the maximum coordinate value

        :return: maximum coordinate value
        :rtype: float

    .. py:method:: lagrangian.Axis.get_num_elements()

        Get the number of values for this axis

        :return: Get the number of values for this axis
        :rtype: int

    .. py:method:: lagrangian.Axis.find_index(coordinate)

        Given a coordinate position, find what element contains it.
        This mean that: ::

             edge[i] <= pos < edge[i+1] (if values are ascending)
             edge[i] > pos >= edge[i+1] (if values are descending)

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :return: the index of the axis if the point is contained in the axis
            or -1 if it is located outside of the axis
        :rtype: int

    .. py:method:: lagrangian.Axis.find_index_bounded(coordinate)

        Given a coordinate position, find what element contains it or
        is closest to it.

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :return: index of the grid point containing it or -1 if outside grid
            area
        :rtype: int

    .. py:method:: lagrangian.Axis.normalize(coordinate, circle)

        Standardization of longitude

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :param circle: value of the trigonometric circle in this coordinate system
        :type circle: float
        :return: longitude between: ::

            [self.get_min_value(), self.get_max_value() + circle]

        :rtype: float

    .. py:method:: lagrangian.Axis.convert(unit)

        Converts the axis data from unit `self.units` to unit. Can
        be called only if the property `self.units` is not None

        :param unit: the new unit
        :type unit: str

    .. py:method:: lagrangian.Axis.find_indexes(coordinate)

        Given a coordinate position, find grids elements around it.
        This mean that: ::

            points[i0] <= coordinate < points[i1]

        :param coordinate: position in this coordinate system
        :type coordinate: float
        :return: `i0` and `i1` if the coordinate is inside the axis otherwise
            None
        :rtype: tuple

    .. py:method:: __eq__(rhs):

        Returns self == rhs.

    .. py:method:: __ne__(rhs):

        Returns self != rhs.

Dates and time
--------------

.. py:class:: lagrangian.DateTime(value)

    Bases: object

    A datetime object is a single object containing all the information from a
    date object and a time object. Like a date object, datetime assumes the
    current Gregorian calendar extended in both directions; like a time object,
    datetime assumes there are exactly 3600*24 seconds in every day.

    :param value: Value used to build the new instance
    :type value: datetime.datetime, str

    .. py:method:: __call__()

        Get the Python representation of the instance

        :return: Python object that represents the instance
        :rtype: datetime.datetime

Velocity fields
---------------

.. py:class:: lagrangian.Field(*args)

    Bases: object

    Abstract class defining a field where it is possible to calculate a
    speed

    .. py:attribute:: unit_type

        Unit type used by this field.

    .. py:method:: get_unit()

        Unit used by this field.

        :return: unit
        :rtype: str

.. py:class:: lagrangian.PythonField(unit_type=cpp_lagrangian.kMetric)

    Bases: lagrangian.Field

    Python base class for implementing the class a field where it is possible
    to calculate a speed.

    To implement this class, you must implement the method ``compute``
    respecting the following signature: ::

        def compute (double t, double x, double y):
            return (u, v, defined)

    where ``u`` and ``v`` are the velocities computed, and ``defined`` a boolean
    indicating whether the calculated velocities are valid or not.

    :param unit_type: Unit field
    :type unit_type: lagrangian.UnitType

.. py:class:: lagrangian.Vonkarman(a=1, w=35.06, r0=0.35, tc=1, alpha=2, y0=0.3, l=2, u0=14)

    Bases: lagrangian.Field

    Vonkarman field

    .. py:method:: lagrangian.Vonkarman.compute(t, x, y)

        Compute the field to the spatiotemporal position wanted

        :param t: Time expressed as a number of seconds elapsed since 1970.
        :type t: float
        :param x: Longitude expressed as degree
        :type x: float
        :param y: Latitude expressed as degree
        :type y: float
        :return: u & v interpolated
        :rtype: tuple

Numerical grids
---------------

.. py:class:: lagrangian.CellProperties()

    Bases: object

    Cell properties of the grid used for the interpolation.

    .. py:method:: lagrangian.CellProperties.contains(x, y)

        Test if the coordinate is in the cell.

        :param x: Longitude
        :type x: float
        :param y: Latitude
        :type y: float
        :return: True if the coordinate is in the cell.
        :rtype: bool

    .. py:method:: lagrangian.CellProperties.update(x0, x1, y0, y1, ix0, ix1, iy0, iy1)

        Update the cell properties

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

    .. py:attribute:: lagrangian.CellProperties.iy1

        Get the index of the last latitude in the grid

        :return: The index of the last latitude
        :type: int

    .. py:attribute:: lagrangian.CellProperties.x0

        Get the first longitude of the cell

        :return: The first longitude
        :type: float

    .. py:attribute:: lagrangian.CellProperties.x1

        Get the last longitude of the cell

        :return: The last longitude
        :type: float

    .. py:attribute:: lagrangian.CellProperties.y0

        Get the first latitude of the cell

        :return: The first latitude
        :type: float

    .. py:attribute:: lagrangian.CellProperties.y1

        Get the last latitude of the cell

        :return: The last latitude
        :type: float

.. py:class:: lagrangian.MapProperties(nx, ny, x_min, y_min, step)

    Bases: object

    Properties of a regular grid

    :param nx: Number of longitudes
    :type nx: int
    :param ny: Number of latitudes
    :type ny: int
    :param x_min: Minimal longitude
    :type x_min: float
    :param y_min: Minimal latitude
    :type y_min: float
    :param step: Step between two consecutive longitudes and latitudes
    :type step: float

    .. py:method:: lagrangian.MapProperties.get_x_value(idx)

        Get the longitude value

        :param idx: Index of the longitude in the grid
        :type idx: int
        :return: The longitude
        :type: float

    .. py:method:: lagrangian.MapProperties.get_y_value(idx)

        Get the latitude value

        :param idx: Index of the latitude in the grid
        :type idx: int
        :return: The latitude
        :type: float

    .. py:attribute:: lagrangian.MapProperties.nx

        Get the number of longitudes in the grid

        :return: The number of longitudes
        :rtype: int

    .. py:attribute:: lagrangian.MapProperties.ny

        Get the number of latitudes in the grid

        :return: The number of latitudes
        :rtype: int

    .. py:attribute:: lagrangian.MapProperties.step

        Get the step between two consecutive longitudes and latitudes

        :return: The step
        :rtype: float

    .. py:attribute:: lagrangian.MapProperties.x_min

        Get the minimal longitude

        :return: The minimal longitude
        :rtype: float

    .. py:attribute:: lagrangian.MapProperties.y_min

        Get the minimal latitude

        :return: The minimal latitude
        :rtype: float

    .. py:method:: lagrangian.MapProperties.get_x_axis()

        Get X axis values

        :return: X axis values
        :rtype: numpy.ndarray

    .. py:method:: lagrangian.MapProperties.get_y_axis()

        Get Y axis values

        :return: Y axis values
        :rtype: numpy.ndarray

Runge Kutta
-----------

.. py:class:: lagrangian.RungeKutta(size_of_interval, field)

    Bases: object

    Fourth-order Runge-Kutta method

    :param size_of_interval: Number of time interval
    :type size_of_interval: float
    :param field: Field reader
    :type field: lagrangian.Field

    .. py:method:: lagrangian.RungeKutta.compute(t, x, y, cell=None)

        Move a point in a field

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

Stencils
--------

.. py:class:: lagrangian.Position()

    Bases: object

    Define the position of N points Mₖ = (xₖ, yₖ): ::

                Mₖ₊₁
                |
        Mₖ₊ᵢ ⎯⎯ M₀ ⎯⎯  Mₖ
                |
                Mₖ₊ₙ

    .. py:method:: lagrangian.Position.get_xi(idx)

        Get the longitude of the point idx

        :param idx: position
        :type idx: int
        :return: The longitude in degrees
        :rtype: float

    .. py:method:: lagrangian.Position.get_yi(idx)

        Get the latitude of the point idx

        :param idx: position
        :type idx: int
        :return: The latitude in degrees
        :rtype: float

    .. py:attribute:: lagrangian.Position.time

        Get the time at the end of the integration

        :return: The time expressed in number of seconds elapsed since 1970
        :rtype: float

    .. py:attribute:: lagrangian.Position.completed

        Test if the integration is over

        :return: True if the integration is over
        :rtype: bool

    .. py:method:: lagrangian.Position.set_completed()

        Indicate that the integration is complete.

    .. py:method:: lagrangian.Position.missing()

        Set the instance to represent a missing position.

    .. py:method:: lagrangian.Position.is_missing()

        Test if the integration is defined.

        :return: True if the integration is defined
        :rtype: bool

    .. py:method:: lagrangian.Position.max_distance()

        Compute the distance max

        :return: The max distance
        :rtype: float

    .. py:method:: lagrangian.Position.compute(rk, it, cell)

        To move a particle with a velocity field.

        :param rk: Runge-Kutta handler
        :type rk: lagrangian.RungeKutta
        :param it: Iterator
        :type it: lagrangian.Iterator
        :param cell: Cell properties of the grid used for the interpolation.
        :type cell: lagrangian.CellProperties
        :return: True if the particle could be moved otherwise false
        :rtype: bool

    .. py:method:: lagrangian.Position.strain_tensor()

        TODO

.. py:class:: lagrangian.Triplet(x, y, delta)

    Bases: lagrangian.Position

    Define the position of 3 points

    :param x: Longitude of the initial point
    :type x: float
    :param y: Latitude of the initial point
    :type y: float
    :param delta: Initial initial separation in degrees of neighboring
        particles
    :type delta: float

.. py:class:: lagrangian.Quintuplet(x, y, delta)

    Bases: lagrangian.Position

    Define the position of 5 points

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

Integration
-----------

.. py:class:: lagrangian.Iterator(begin, end, inc)

    Bases: object

    Definition of an iterator over a time period

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

.. py:class:: lagrangian.AbstractIntegration(*args)

    Handles the time integration

    .. py:method:: lagrangian.Integration.get_iterator()

        Return an iterator that describes the integration period

        :return: The iterator
        :rtype: :py:class:`lagrangian::Iterator`

    .. py:method:: lagrangian.Integration.fetch(t)

        Perform the tasks before a new time step (eg load grids required)

        :param t: t Time step in seconds
        :type t: float

    .. py:method:: lagrangian.Integration.compute(it, x0, x1)

        Calculate the new position of the particle

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

.. py:class:: lagrangian.Integration(start_time, end_time, delta_t, field)

    Bases: lagrangian.AbstractIntegration

    Handles the time integration

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.DateTime`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.DateTime`
    :param delta_t: Time interval, in seconds
    :type delta_t: :py:class:`datetime.timedelta`
    :param field: Field to use for computing the velocity of a point.
    :type field: :py:class:`lagrangian.Field`

.. py:class:: lagrangian.Path(start_time, end_time, delta_t, field)

    Bases: lagrangian.AbstractIntegration

    Handles the movement of a particle using the Runge-Kutta method.

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.DateTime`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.DateTime`
    :param delta_t: Time interval, in seconds
    :type delta_t: :py:class:`datetime.timedelta`
    :param field: Field to use for computing the velocity of a point.
    :type field: :py:class:`lagrangian.Field`

Lyapunov Exponents
------------------

.. py:class:: lagrangian.Mode

    Mode of integration

    .. py:attribute:: kFSLE

        Finite Size Lyapunov Exponent

    .. py:attribute:: kFTLE

        Finite Time Lyapunov Exponent

.. py:class:: lagrangian.FiniteLyapunovExponents()

    Bases: object

    Storing Lyapunov coefficients calculated.

    .. seealso::

        FiniteLyapunovExponentsIntegration

    .. py:attribute:: lagrangian.FiniteLyapunovExponents.lambda1

        Get the FLE associated to the maximum eigenvalue of the Cauchy-Green
        strain tensor

        :return: λ₁ (unit 1/sec)
        :type: float

    .. py:attribute:: lagrangian.FiniteLyapunovExponents.lambda2

        Get the FLE associated to the minimum eigenvalue of the Cauchy-Green
        strain tensor

        :return: λ₂ (unit 1/sec)
        :type: float

    .. py:attribute:: lagrangian.FiniteLyapunovExponents.theta1

        Get the orientation of the eigenvector associated to the maximum
        eigenvalue of the Cauchy-Green strain tensor

        :return: θ₁ (unit degrees)
        :type: float

    .. py:attribute:: lagrangian.FiniteLyapunovExponents.theta2

        Get the orientation of the eigenvector associated to the minimum
        eigenvalue of the Cauchy-Green strain tensor

        :return: θ₂ (unit degrees)
        :type: float

Velocity readers
----------------

.. py:class:: lagrangian.Netcdf()

    Bases: object

    Grid NetCDF CF reader.

    The grid must contain at least one variable and two vectors defining the
    axes of the longitudes and latitudes of the variable. For example : ::

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
               v:long_name = "U" ;
               v:units = "cm/s" ;
               V:date = "2012-01-01 00:00:00.000000 UTC" ;

    .. note:: The variable to be read must set an attribute named "date" that
        define the date of data contained in the variable.

    .. py:method:: interpolate(longitude, latitude, fill_value=0, cell=lagrangian.CellProperties.NONE)

        Computes the velocity of the grid point requested

        :param longitude: Longitude in degrees.
        :type longitude: float
        :param latitude: Latitude in degrees
        :type latitude: float
        :param fill_value: Value to be taken into account for fill values
        :type fill_value: float
        :param cell: Cell properties of the grid used for the interpolation.
        :type cell: lagrangian.CellProperties

    .. py:method:: load(name, unit)

        Load data into memory

        :param name: Name of the grid who contains data
        :type name: str
        :param unit: Unit of data loaded into memory.
        :type unit: str

    .. py:method:: open(filename)

        Opens a file in read-only.

        :param filename: Path to the grid
        :type filename: str

.. py:class:: lagrangian.PythonReader()

    Python base class for implementing a velocity reader fields.

.. py:class:: lagrangian.ReaderType

    Type of fields reader known

    .. py:attribute:: kNetCDF

        The velocity field is read from NetCDF grids.

.. py:class:: lagrangian.Factory()

    Reader Factory

    .. py:staticmethod:: lagrangian.Factory.new_reader(reader_type)

        :param reader_type: Name of the grid who contains data
        :type reader_type: ReaderType
        :return: An instance of a reader
        :rtype: lagrangian.AbstractReader

.. py:class:: lagrangian.TimeSerie(configuration_file, unit_type=lagrangian.kMetric, reader_type=lagrangian.kNetCDF)

    Bases: lagrangian.Field

    Time series of velocity field

    :param configuration_file: The configuration file contains the list of
        files to take into account to interpolate speeds, the expected
        syntax is :

        .. code-block:: cfg

            U = <path to the NetCDF file>
            U = ...
            V = <path to the NetCDF file>
            V = ...
            U_NAME = <name of the NetCDF variable>
            V_NAME = <name of the NetCDF variable>
            FILL_VALUE = <value>

        Keys representing the following data:

        * ``U`` defines the netCDF files containing the eastward velocities.
        * ``V`` defines the netCDF files containing the northward velocities.
        * ``U_NAME`` defines the NetCDF variable containing the eastward velocities.
        * ``V_NAME`` defines the NetCDF variable containing the northward velocities.
        * ``FILL_VALUE`` value to be taken into account when the reader encounters an
          undefined value. This value must be ``0`` if you do not wish to generate
          undefined values ​​when integrating or ``nan`` if the calculation must
          generate undefined values​​.

        The path to the NetCDF file must contain an absolute path, for example:

        .. code-block:: cfg

            U = /home/lagrangian/file.nc

        The path may also contain environment variables using the shell syntax, for
        example:

        .. code-block:: cfg

            U = ${DATA}/file.nc

    :type configuration_file: str
    :param unit_type: Unit fields.
    :type unit_type: lagrangian.UnitType
    :param reader_type: The reader used to read grids containing speeds.
    :type reader_type: lagrangian.ReaderType

    .. py:method:: lagrangian.TimeSerie.start_time()

        Returns the date of the first grid constituting the time series.

        :return: the date of the first date
        :type: datetime.datetime

    .. py:method:: lagrangian.TimeSerie.end_time()

        Returns the date of the last grid constituting the time series.

        :return: the date of the last date
        :type: datetime.datetime

    .. py:method:: lagrangian.TimeSerie.compute(t, x, y, cell=None)

        Interpolates the velocity to the wanted spatio temporal position.

        :param t: Time expressed as a number of seconds elapsed since 1970.
        :type t: float
        :param x: Longitude expressed as degree
        :type x: float
        :param y: Latitude expressed as degree
        :type y: float
        :param cell: Cell properties of the grid used for the interpolation
        :type cell: lagrangian.CellProperties
        :return: A tuple that contains the interpolated velocity (u, v) of the
            to the wanted spatio temporal position or None if the field is
            undefined for the asked position
        :rtype: tuple

    .. py:method:: lagrangian.TimeSerie.fetch(t0, t1)

        Loads the grids used to interpolate the velocities in the
        interval [t0, t1]

        :param t0: First date of the interval expressed as a number of seconds
            elapsed since 1970.
        :type t0: float
        :param t1: Last date of the interval expressed as a number of seconds
            elapsed since 1970.
        :type t1: float

Map of Lyapunov Exponents
-------------------------

.. py:class:: lagrangian.MapOfFiniteLyapunovExponents(map_properties, fle, stencil=langrangian.kTriplet, netcdf_reader=None)

    Bases: object

    Handles a map of Finite Size or Time Lyapunov Exponents

    :param map_properties: Properties of the regular grid to create
    :type map_properties: lagrangian.MapProperties
    :param fle: FLE handler
    :type fle: lagrangian.FiniteLyapunovExponents
    :param stencil: Type of stencil used for the calculation of finite
        difference.
    :type stencil: lagrangian.Stencil
    :param netcdf_reader: NetCDF used to locate the hidden values
        ​​(eg continents). These cells identified will not be taken into
        account during the calculation process, in order to accelerate it.
        If this parameter is not defined, all cells will be processed in the
        calculation step.
    :type netcdf_reader: lagrangian.Netcdf

    .. py:method:: lagrangian.MapOfFiniteLyapunovExponents.compute()

        Compute the map

        .. note::

            You can set the environment variable ``OMP_NUM_THREADS`` to enable
            parallelization of code with the number of threads defined.

    .. py:method:: lagrangian.MapOfFiniteLyapunovExponents.get_map_of_lambda1(fill_value)

        Get the map of the FLE associated to the maximum eigenvalues of
        Cauchy-Green strain tensor

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map of λ₁ (unit 1/sec)
        :rtype: numpy.ndarray

    .. py:method:: lagrangian.MapOfFiniteLyapunovExponents.get_map_of_lambda2(fill_value)

        Get the map of the FLE associated to the minimum eigenvalues of
        Cauchy-Green strain tensor

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map of λ₂ (unit 1/sec)
        :rtype: numpy.ndarray

    .. py:method:: lagrangian.MapOfFiniteLyapunovExponents.get_map_of_theta1(fill_value)

        Get the map of the orientation of the eigenvectors associated
        to the maximum eigenvalues of Cauchy-Green strain tensor

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map of θ₁ (unit degrees)
        :rtype: numpy.ndarray

    .. py:method:: lagrangian.MapOfFiniteLyapunovExponents.get_map_of_theta2(fill_value)

        Get the map of the orientation of the eigenvectors associated
        to the minimum eigenvalues of Cauchy-Green strain tensor

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map of θ₂ (unit degrees)
        :rtype: numpy.ndarray

Utility functions
-----------------

.. py:function:: lagrangian.debug(msg)

    Display a debugging message

    :param msg: Message to display
    :type msg: str


.. py:function:: lagrangian.set_verbose(value)

    Enable or disable verbose mode

    :param value: True to enable verbose mode
    :type value: bool


.. py:function:: lagrangian.version()

    Return the version number

    :return: Version number
    :rtype: str
