Map of FLE computation
======================

MapProperties
-------------

.. py:class:: lagrangian.MapProperties(nx, ny, x_min, y_min, step)

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

    .. py:method:: GetXAxis()

        Get X axis values

        :return: X axis values
        :rtype: numpy.ndarray

    .. py:method:: GetYAxis()

        Get Y axis values

        :return: Y axis values
        :rtype: numpy.ndarray

    .. py:method:: get_nx()

        Get the number of longitudes in the grid

        :return: The number of longitudes
        :rtype: int

    .. py:method:: get_ny()

        Get the number of latitudes in the grid

        :return: The number of latitudes
        :rtype: int

    .. py:method:: get_step()

        Get the step between two consecutive longitudes and latitudes

        :return: The step
        :rtype: float

    .. py:method:: get_x_min()

        Get the minimal longitude

        :return: The minimal longitude
        :rtype: float

    .. py:method:: get_y_min()

        Get the minimal latitudes

        :return: The minimal latitude
        :rtype: float

MapOfFiniteLyapunovExponents
----------------------------

.. py:class:: lagrangian.MapOfFiniteLyapunovExponents(nx, ny, x_min, y_min, step)

    Handles a map of Finite Size or Time Lyapunov Exponents

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

    .. py:method:: Compute(fle)

        Compute the map

        :param fle: Finite Lyapunov exponents
        :type fle: lagrangian.FiniteLyapunovExponents

    .. py:method:: GetMapOfLambda1(fill_value, fle)

        Get map of FLE associated to the maximum eigenvalues of Cauchy-Green
        strain tensor

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map
        :rtype: numpy.ndarray

    .. py:method:: GetMapOfLambda2(fill_value, fle)

        Get map of FLE associated to the minimum eigenvalues of Cauchy-Green
        strain tensor 

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map
        :rtype: numpy.ndarray

    .. py:method:: GetMapOfTheta1(fill_value, fle)

        Get map of the orientation of the eigenvectors associated to the
        maximum eigenvalues of Cauchy-Green strain tensor 

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map
        :rtype: numpy.ndarray

    .. py:method:: GetMapOfTheta2(fill_value, fle)

        Get map of the orientation of the eigenvectors associated to the
        minimum eigenvalues of Cauchy-Green strain tensor 

        :param fill_value: value used for missing cells
        :type fill_value: float
        :return: The map
        :rtype: numpy.ndarray


    .. py:method:: Initialize(fle)

        Initializing the grid cells

        :param fle: Finite Lyapunov exponents
        :type fle: :py:class:`lagrangian.FiniteLyapunovExponents`

    .. py:method:: Initialize(fle, reader)

        Initializing the grid cells. Cells located on the hidden values
        ​​(eg. continents) will be deleted from the calculation

        :param fle: Finite Lyapunov exponents
        :type fle: :py:class:`lagrangian.FiniteLyapunovExponents`
        :param reader: NetCDF reader allow to access of the mask's value.
        :type reader: :py:class:`lagrangian.NetcdfReader`

    .. py:method:: get_map_properties()

        Get the map properties

        :return: The map properties
        :rtype: :py:class:`lagrangian.MapProperties`
    
