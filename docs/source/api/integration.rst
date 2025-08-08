Integration and Computation
===========================

This module provides the core classes for performing Lagrangian trajectory
integrations and computing finite-time Lyapunov exponents.

.. currentmodule:: lagrangian.core

Base Integration
----------------

.. class:: Integration

    The base class for all integration methods. Handles the time evolution of
    particles in a velocity field.

    .. automethod:: __init__

    **Key Methods**

    * :py:meth:`compute()`: Executes the integration process to compute
      particle trajectories.
    * :py:meth:`fetch()`: Prepares necessary data or performs tasks required
      before advancing to the next time step.
    * :py:meth:`iterator()`: Generates an iterator that defines the time steps
      for the integration period.

    **Examples**

    Basic integration setup::

        import lagrangian

        # Create a field and integration object
        field = lagrangian.field.TimeSerie("config.ini")
        integration = lagrangian.Integration(
            start_time=datetime(2010, 1, 1),
            end_time=datetime(2010, 1, 10),
            delta_t=timedelta(hours=1),
            field=field
        )

        # Compute trajectory
        result = integration.compute(it=integration.iterator(), x0=10.0, y0=45.0)

    ----

    .. automethod:: compute

    ----

    .. automethod:: fetch

    ----

    .. automethod:: iterator


Path Integration
----------------

.. class:: Path

    Bases: :py:class:`lagrangian.Integration`

    Specialized integration for computing particle trajectories (paths) over time.

    .. automethod:: __init__

Integration Modes
-----------------

.. class:: IntegrationMode

    Enumeration of available integration modes:

    * ``FSLE``: Finite-Size Lyapunov Exponents
    * ``FTLE``: Finite-Time Lyapunov Exponents

Finite Lyapunov Exponents
--------------------------

.. class:: FiniteLyapunovExponentsIntegration

    Specialized integration class for computing finite-time Lyapunov exponents.

    Finite Size Lyapunov Exponent (FSLE) is a scalar local notion that
    represents the rate of separation of initially neighbouring particles over a
    finite-time window :math:`[t_0, t_0 + T]`, where :math:`T` is the time two
    particles need to be advected in order to be separated from a given distance
    :math:`d`.

    Let :math:`x(t) = x(t; x_0, t_0)` be the position of a lagrangian particle
    at time :math:`t`, started at :math:`x_0` at :math:`t = t_0` and advected by
    the time-dependent fluid flow :math:`u(x, t)`.

    The Forward Finite-Time Lyapunov Exponent at a point :math:`x_0` and for the
    advection time :math:`T` is defined as the growth factor of the norm of the
    perturbation :math:`dx_0` started around :math:`x_0` and advected by the
    flow after the finite advection time :math:`T`.

    Maximal stretching occurs when :math:`dx_0` is aligned with the eigenvector
    associated with the maximum eigenvalue :math:`\delta_{\text{max}}` of the
    Cauchy-Green strain tensor :math:`\Delta`:

    .. math::
        \Delta = \left[ \nabla \Phi_0^T (x_0) \right]^*
            \left[ \nabla \Phi_0^T (x_0) \right]

    where :math:`\Phi_0^T : x_0 \rightarrow x(t, x_0, t_0)` is the flow map of
    the advection equation: it links the location :math:`x_0` of a lagrangian
    particle at :math:`t = t_0` to its position :math:`x(t, x_0, t_0)` at time
    :math:`t`. (* denotes the transposition operator).

    FTLE is defined as:

    :math:`\sigma = \frac{1}{2T} \log \left( \lambda_{\text{max}}(\Delta) \right)`

    Finite-Size Lyapunov Exponent is similarly defined: :math:`T` is chosen so
    that neighbouring particles separate from a given distance :math:`d`.

    ComputeExponents function implements the computation of the Lyapunov
    exponents based on maximal and minimal eigenvalues and orientation of
    eigenvectors of :math:`\Delta` given the elements of :math:`\nabla \Phi_0^T`
    matrix.

    For more details see:

    #. G. Haller, Lagrangian coherent structures and the rate of strain in
       two-dimensional turbulence Phys. Fluids A 13 (2001) 3365-3385
       (http://georgehaller.com/reprints/approx.pdf)
       Remark: In this paper, FTLE is referred to as the Direct Lyapunov
       Exponent (DLE)
    #. http://mmae.iit.edu/shadden/LCS-tutorial/FTLE-derivation.html

    .. automethod:: __init__

    **Examples**

    Computing FSLE::

        fsle_integration = lagrangian.FiniteLyapunovExponentsIntegration(
            start_time=datetime(2010, 1, 1),
            end_time=datetime(2010, 1, 10),
            delta_t=timedelta(hours=1),
            mode=lagrangian.IntegrationMode.FSLE,
            min_separation=0.1,  # in degrees
            delta=0.01,          # initial separation
            field=field
        )

.. class:: FiniteLyapunovExponents

    Container for finite-time Lyapunov exponent results.

    **Attributes**

    * :py:attr:`lambda1`: First Lyapunov exponent
    * :py:attr:`lambda2`: Second Lyapunov exponent
    * :py:attr:`theta1`: Angle of first eigenvector
    * :py:attr:`theta2`: Angle of second eigenvector
    * :py:attr:`delta_t`: Time taken to reach final separation
    * :py:attr:`final_separation`: Final separation distance

Numerical Integration
---------------------

.. class:: RungeKutta

    Fourth-order Runge-Kutta integrator for solving ordinary differential
    equations.

    .. automethod:: __init__

    **Key Methods**

    * :py:meth:`compute()`: Perform the Runge-Kutta integration step

    ----

    .. automethod:: compute


.. class:: Iterator

    Iterator for time stepping through integration periods.

Coordinate Systems
------------------

.. class:: CoordinatesType

    Enumeration of coordinate system types:

    * ``CARTESIAN``: Cartesian coordinates (x, y)
    * ``SPHERICAL_EQUATORIAL``: Spherical coordinates (longitude, latitude)

.. class:: UnitType

    Enumeration of unit types:

    * ``METRIC``: Metric units (meters)
    * ``ANGULAR``: Angular units (degrees)
