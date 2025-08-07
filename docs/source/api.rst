API Reference
#############

The Lagrangian library provides tools for computing Lagrangian trajectories and
analyzing finite-time Lyapunov exponents from velocity fields. This reference
provides detailed documentation for all public classes and functions.

.. note::

    The API documentation below is generated from the core module, which
    contains the main classes and functions for Lagrangian integration and
    analysis. These classes are implemented in C++ for performance and exposed
    to Python. For convenience, you can import them directly from the root
    ``lagrangian`` module instead of ``lagrangian.core`` in your code examples.

.. automodule:: lagrangian.core
   :members:
   :exclude-members: CoordinatesType, IntegrationMode, Stencil, UnitType
   :show-inheritance:


.. class:: CoordinatesType

    Specifies the available coordinate systems for representing positions
    in a velocity field. This determines which movement function is used
    during Runge-Kutta integration.

    The CoordinatesType enum defines which coordinate system is used for
    interpreting positions and computing trajectories. This affects how
    distances and transformations are calculated during integration.

    .. autoattribute:: CoordinatesType.CARTESIAN

        If the field units are in degrees per second (deg/s), velocities
        are treated as direct changes in position: new coordinates are
        calculated by adding the velocity multiplied by time to the
        original coordinates (x1 = x0 + u*t, y1 = y0 + v*t).

    .. autoattribute:: CoordinatesType.SPHERICAL_EQUATORIAL

        When the field units are (m/s) and positions are given in degrees,
        velocities are applied in the tangent plane and then converted back
        to latitude and longitude. This method accounts for Earth's spherical
        geometry, ensuring accurate tracking of global ocean currents.


.. class:: IntegrationMode

    Enumeration of integration modes for finite-time Lyapunov exponent
    computations.

    The IntegrationMode enum specifies which type of finite-time Lyapunov
    exponent calculation to perform. This affects the stopping criteria
    and output interpretation.

    .. autoattribute:: IntegrationMode.FSLE

        Finite-Size Lyapunov Exponent mode. Integration continues until
        particles reach a specified separation distance. The result is
        the time taken to reach this separation.

    .. autoattribute:: IntegrationMode.FTLE

        Finite-Time Lyapunov Exponent mode. Integration runs for a fixed
        time period. The result is the separation achieved at the end
        of this period.


.. class:: Stencil

   Enumeration of particle stencil configurations for finite difference
   calculations.

   The Stencil enum defines how particles are arranged around a central point
   for computing finite-time Lyapunov exponents. Different stencils provide
   different trade-offs between accuracy and computational cost.

   .. autoattribute:: Stencil.TRIPLET

      3-particle stencil configuration. Uses one central particle and two
      additional particles to form a triangle. More efficient but less
      accurate than quintuplet.

   .. autoattribute:: Stencil.QUINTUPLET

      5-particle stencil configuration. Uses one central particle and four
      additional particles arranged in a cross pattern. More accurate but
      computationally more expensive than triplet.

.. class:: UnitType

   Enumeration of unit types for velocity fields and coordinates.

   The UnitType enum defines the physical units used in velocity field
   computations. This affects how distances and velocities are interpreted
   during integration.

   **Available Types:**

   .. autoattribute:: UnitType.METRIC

      Metric units (meters, m/s). Used for Cartesian coordinate systems
      where distances are measured in meters and velocities in meters per
      second.

   .. autoattribute:: UnitType.ANGULAR

      Angular units (degrees, degrees/day). Used for spherical coordinate
      systems where distances are measured in degrees and velocities in degrees
      per day.
