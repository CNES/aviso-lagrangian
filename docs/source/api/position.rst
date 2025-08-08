Position and Trajectory
=======================

Classes for representing particle positions and trajectories during integration.

.. currentmodule:: lagrangian.core

Position Tracking
------------------

.. class:: Position

    Base class for tracking particle positions during integration.

    .. method:: __init__()
        :no-index:

        Default constructor

        .. note::

            This constructor may lead to an error in diagnostic because start
            time is set to zero: if stencil has not been advected then delta_t
            is equal to minus advection starting time (``-start_time``) which is
            not correct.

    .. method:: __init__(self: lagrangian.core.Position, start_time: lagrangian::DateTime, spherical_equatorial: bool = True) -> None

        Constructor with start_time setting

        :param start_time: Advection starting time particles
        :param spherical_equatorial: True if the coordinates system is Lon/lat otherwise false


    **Key Methods**

    * :py:meth:`compute()`: Advance position using Runge-Kutta integration
    * :py:meth:`strain_tensor()`: Calculate the strain tensor at current position
    * :py:meth:`max_distance()`: Get maximum distance from initial position
    * :py:meth:`xi()`, :py:meth:`yi()`: Get coordinate arrays

    **Properties**

    * :py:attr:`is_completed`: Whether integration is complete
    * :py:attr:`time`: Current time of integration

    ----

    .. automethod:: compute

    ----

    .. automethod:: strain_tensor

    ----

    .. automethod:: max_distance

    ----

    .. automethod:: xi

    ----

    .. automethod:: yi

    ----

    .. autoproperty:: is_completed

    ----

    .. autoproperty:: time


Stencil Configurations
----------------------

.. class:: Stencil

    Enumeration of stencil types for finite difference calculations:

    * ``TRIPLET``: 3-point stencil
    * ``QUINTUPLET``: 5-point stencil

Triplet Position
----------------

.. class:: Triplet

    Bases: :py:class:`lagrangian.Position`

    3-point stencil for computing Lyapunov exponents using three particles.

    The triplet method uses three particles arranged in a specific pattern to
    compute the finite-time Lyapunov exponents. This is computationally
    efficient but may be less accurate than the quintuplet method.

    **Examples**

    Creating a triplet configuration::

        import lagrangian
        from datetime import datetime

        triplet = lagrangian.Triplet(
            x=10.0,      # Initial longitude
            y=45.0,      # Initial latitude
            delta=0.01,  # Initial separation
            start_time=datetime(2010, 1, 1),
            spherical_equatorial=True
        )

Quintuplet Position
-------------------

.. class:: Quintuplet

    5-point stencil for computing Lyapunov exponents using five particles.

    The quintuplet method uses five particles (one central particle plus four
    surrounding particles) to compute finite-time Lyapunov exponents. This
    provides higher accuracy than the triplet method at the cost of additional
    computation.

    **Examples**

    Creating a quintuplet configuration::

        import lagrangian
        from datetime import datetime

        quintuplet = lagrangian.Quintuplet(
            x=10.0,      # Initial longitude
            y=45.0,      # Initial latitude
            delta=0.01,  # Initial separation
            start_time=datetime(2010, 1, 1),
            spherical_equatorial=True
        )
