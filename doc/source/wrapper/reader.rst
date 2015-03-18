Read velocity fields
====================

CellProperties
--------------

.. py:class:: lagrangian.CellProperties()

    Cell properties of the grid used for the interpolation.

    .. py:method:: Contains(x, y)

        Test if the coordinate is in the cell

        :param x: Longitude
        :type x: float
        :param y: Latitude
        :type y: float
        :return: True if the coordinate is in the cell.
        :rtype: bool

    .. py:staticmethod:: NONE()

        Return the representation of a cell unhandled

        :return: A cell unhandled
        :rtype: :py:class:`lagrangian.CellProperties`


    .. py:method:: Update(x0, x1, y0, y1, ix0, ix1, iy0, iy1)

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
        :type it1: int

    .. py:method:: ix0()

        Get the index of the first longitude in the grid

        :return: The index of the first longitude
        :type: int

    .. py:method:: ix1()

        Get the index of the last longitude in the grid

        :return: The index of the last longitude
        :type: int

    .. py:method:: iy0()

        Get the index of the first latitude in the grid

        :return: The index of the first latitude
        :type: int

    .. py:method:: iy1()

        Get the index of the last latitude in the grid

        :return: The index of the last latitude
        :type: int

    .. py:method:: x0()

        Get the first longitude of the cell

        :return: The first longitude
        :type: float

    .. py:method:: x1()

        Get the last longitude of the cell

        :return: The last longitude
        :type: float

    .. py:method:: y0()

        Get the first latitude of the cell

        :return: The first latitude
        :type: float

    .. py:method:: y1()

        Get the last latitude of the cell

        :return: The last latitude
        :type: float

Reader
------

.. py:class:: lagrangian.Reader()

    Abstract class that defines a velocity reader fields.

    .. py:method:: GetJulianDay(name)

        Returns the date of the field

        :return: The date of the field
        :rtype: lagrangian.JulianDay

    .. py:method:: Interpolate(longitude, latitude, fill_value=0, cell=lagrangian.CellProperties.NONE)

        Computes the velocity of the grid point requested

        :param longitude: Longitude in degrees.
        :type longitude: float
        :param latitude: Latitude in degrees
        :type latitude: float
        :param fill_value: Value to be taken into account for fill values
        :type fill_value: float
        :param cell: Cell properties of the grid used for the interpolation.
        :type cell: :py:class:`lagrangian.CellProperties`

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

NetCDFReader
------------

Handles reading netCDF CF grids.

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

.. note::
    The variable to be read must set an attribute named "date" that
    define the date of data contained in the variable.

.. py:class:: lagrangian.NetcdfReader()
    
    Bases: :py:class:`lagrangian.Reader`

    Grid NetCDF CF reader.

    .. py:method:: Load(name, unit="")

        Load data into memory

        :param name: name of the NetCDF grid who contains grid data
        :type name: str
        :param unit: Unit of data loaded into memory. If the parameter is
            undefined or contains an empty string, the object will not do unit
            conversion(i.e. the unit of the data loaded into memory is the
            unit defined in the NetCDF file)
        :type unit: str