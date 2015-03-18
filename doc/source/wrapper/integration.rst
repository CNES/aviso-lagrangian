Finite Lyapunov Exponents (FLE) computation
-------------------------------------------

Iterator
========

.. py:class:: lagrangian.Iterator(begin, end, inc)

    Definition of an iterator over a time period

    :param begin: Begin of the period expressed in number of seconds
        elapsed since 1970
    :type begin: float
    :param end: End of the period expressed in number of seconds
        elapsed since 1970
    :type end: float
    :param inc: Time increment in seconds
    :type inc: float

    .. py:method:: GoAfter()

        Test whether there is still a time step to cross into the time
        interval defined.

        :return: True if the path in the time interval is not complete.
        :type: bool

    .. py:method:: Next()

        Move to the next time step.

    .. py:method:: __call__()

        Get the current time in the time interval defined

        :return: The current time expressed in number of seconds elapsed since
            1970
        :rtype: float

Integration
===========

.. py:class:: lagrangian.Integration(start_time, end_time, delta_t, field)

    Handles the time integration

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.JulianDay`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.JulianDay`
    :param delta_t: Time interval, in seconds
    :type delta_t: :py:class:`datetime.timedelta`
    :param field: Field to use for computing the velocity of a point.
    :type field: :py:class:`lagrangian.Field`

    .. py:method:: GetIterator()

        Return an iterator that describes the integration period

        :return: The iterator
        :rtype: :py:class:`lagrangian::Iterator`

    .. py:method:: Fetch(t)

         Perform the tasks before a new time step (eg. load grids required)

        :param t: t Time step in seconds
        :type t: float

Path
====

.. py:class:: lagrangian.Path(start_time, end_time, delta_t, field)

    Base: :py:class:`lagrangian.Integration` 
    
    Handles the movement of a particle using the Runge-Kutta method.

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.JulianDay`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.JulianDay`
    :param delta_t: Time interval, in seconds
    :type delta_t: :py:class:`datetime.timedelta`
    :param field: Field to use for computing the velocity of a point.
    :type field: :py:class:`lagrangian.Field`

    .. py:method:: Compute(it, x0, x1)

        Calculate the new position of the particle

        :param it: Iterator
        :type it: :py:class:`lagrangian.Iterator`
        :param x0: Longitude in degrees
        :type x0: float
        :param y0: Latitude in degrees
        :type y0: float
        :return: A tuple that contains the new position (x1, y1) of the particle
        :rtype: tuple

Triplet
=======

.. py:class:: lagrangian.Triplet

    Define the position of three points

    .. py:method __init__()

        Default constructor

    .. py:method:: __init__(x0, x1, x2, y0, y1, y2, y3)

        Construct a new object defining the position of the 3 points

        :param x0: Longitude of point #1
        :param x1: Longitude of point #2
        :param x2: Longitude of point #3
        :param y0: latitude of point #1
        :param y1: latitude of point #2
        :param y2: latitude of point #3

    .. py:method:: Update(time, x0, x1, x2, y0, y1, y2, y3)

        Update the position

        :param time: Time step (number of seconds elapsed since 1970)
        :param x0: Longitude of point #1
        :param x1: Longitude of point #2
        :param x2: Longitude of point #3
        :param y0: latitude of point #1
        :param y1: latitude of point #2
        :param y2: latitude of point #3

    .. py:method:: get_x0()

        Get the longitude of the point #1

        :return: The longitude in degrees
        :rtype: float

    .. py:method:: get_x1()

        Get the longitude of the point #2

        :return: The longitude in degrees
        :rtype: float

    .. py:method:: get_x2()

        Get the longitude of the point #3

        :return: The longitude in degrees
        :rtype: float

    .. py:method:: get_y0()

        Get the latitude of the point #1

        :return: The latitude in degrees
        :rtype: float

    .. py:method:: get_y1()

        Get the latitude of the point #2

        :return: The latitude in degrees
        :rtype: float

    .. py:method:: get_y2()

        Get the latitude of the point #3

        :return: The latitude in degrees
        :rtype: float

    .. py:method:: get_time()

        Get the time at the end of the integration

        :return: The time expressed in number of seconds elapsed since 1970
        :rtype: float

    .. py:method:: get_completed()

        Test if the integration is over

        :return: True if the integration is over
        :rtype: bool

    .. py:method:: set_completed()

        Indicate that the integration is complete.

    .. py:method:: IsMissing()

        Test if the integration is defined.

        :return: True if the integration is defined.
        :rtype: bool

    .. py:staticmethod:: MISSING()

        Return the location of a missing dot.

        :return: The missing dot
        :rtype: `py:class:`lagrangian.Triplet`

Mode
====

.. py:class:: lagrangian.Mode

    Mode of integration

    .. py:data:: kFSLE = lagrangian.Mode.kFSLE

        Finite Size Lyapunov Exponent

    .. py:data:: kFTLE = lagrangian.Mode.kFTLE

        Finite Time Lyapunov Exponent

FiniteLyapunovExponents
=======================

Finite Size Lyapunov Exponent (FSLE) is a scalar local notion that represents
the rate of separation of initially neighbouring particles over a finite-time
window :math:`[t_{0^\prime}, t_0 + T]`, where :math:`T` is the time two
particules need to be advected in order to be separated from a given distance
:math:`d`.

Let :math:`x(t) = x(t; x{0^\prime}, t_0)` be the position of a lagrangian
particle at time :math:`t`, started at :math:`x_0` at :math:`t=t_0` and
advected by the time-dependent fluid flow :math:`u(x, t)`.

The Forward Finite-Time Lyapunov Exponent at a point :math:`x_0` and for the
advection time :math:`T` is defined as the growth factor of the norm of the
perturbation :math:`dx_0` started around :math:`x_0` and advected by the flow
after the finite advection time :math:`T`.

Maximal stretching occurs when :math:`dx_0` is aligned with the eigenvector
associated with the maximum eigenvalue :math:`\delta_{max}` of the Cauchy-Green
strain tensor
:math:`\Delta = [ \nabla\Phi_0^T (x_0) ]^* [ \nabla\Phi_0^T (x_0) ]`

where :math:`\Phi_0^T: x_0 \mapsto x(t, x_0, t_0)` is the flow map of the
advection equation: it links the location :math:`x_0` of a lagragian particule
at :math:`t=t_0` to its position :math:`x(t,x_0,t_0)` at time :math:`t`.
(* denotes the transposition operator).

FTLE is defined as

:math:`\delta = \frac{log(\lambda_{max}(\Delta))}{2*T}`

Finite-Size Lyapunov Exponent is similary defined: :math:`T` is choosen so that
neighbouring particules separate from a given distance d.

.. seealso::

    * G. Haller, Lagrangian coherent structures and the rate of strain in
      two-dimensional turbulence Phys. Fluids A 13 (2001) 3365-3385
      (http://georgehaller.com/reprints/approx.pdf).
      Remark: In this paper, FTLE is referred to as the Direct Lyapunov
      Exponent (DLE)
    * http://mmae.iit.edu/shadden/LCS-tutorial/FTLE-derivation.html


.. py:class:: lagrangian.FiniteLyapunovExponents(start_time, end_time, delta_t, mode, min_separation, delta, field)

    Base: :py:class:`lagrangian.Integration` 

    Handles the computation of Lyapunov Exponent

    :param start_time: Start time of the integration (number of seconds elapsed 1970)
    :type start_time: :py:class:`lagrangian.JulianDay`
    :param end_time: End date of the integration (number of seconds elapsed 1970)
    :type end_time: :py:class:`lagrangian.JulianDay`
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


    .. py:method:: Compute(it, p)

        Calculate the integration

        :param it: Iterator
        :type it: :py:class:`lagrangian.Iterator`
        :param p: Position of the particle
        :type p: :py:class:`lagrangian.Triplet`
        :return: A tuple that contains a boolean to indicate if the
            integration is defined and the new position of the particle.
        :rtype: tuple(bool, :py:class:`lagrangian.Triplet`)

    .. py:method:: Exponents(p)

        Compute the eigenvalue and the orientation of the eigenvectors
        of the Cauchy-Green strain tensor

        This function implements the computation of the lyapunov exponents
        based on maximal and minimal eigenvalues and orientation of
        eigenvectors of :math:`\Delta` given the elements of
        :math:`\nabla\Phi_0^T (x_0)` matrix.

        :param p: Position of the particle
        :type p: :py:class:`lagrangian.Triplet`
        :return: True if the exponents are defined
        :rtype: bool

    .. py:method:: get_lambda1()

        Return :math:`\lambda_1`

        :return: :math:`\lambda_1`
        :rtype: float

    .. py:method:: get_lambda1()

        Return :math:`\lambda_2`

        :return: :math:`\lambda_2`
        :rtype: float

    .. py:method:: get_theta1()

        Return :math:`\theta_1`

        :return: :math:`\theta_1`
        :rtype: float

    .. py:method:: get_theta2()

        Return :math:`\theta_2`

        :return: :math:`\theta_2`
        :rtype: float
