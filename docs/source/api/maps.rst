Maps and Analysis
=================

Classes for computing and analyzing maps of Lagrangian diagnostics.

.. currentmodule:: lagrangian.core

Map Properties
--------------

.. class:: MapProperties

    Defines the spatial grid properties for computing Lagrangian maps.

    .. automethod:: __init__

    **Methods**

    * :py:meth:`x_axis()`: Get array of x-coordinates
    * :py:meth:`y_axis()`: Get array of y-coordinates

    **Properties**

    * :py:attr:`nx`, :py:attr:`ny`: Grid dimensions
    * :py:attr:`x_min`, :py:attr:`y_min`: Grid origin
    * :py:attr:`step`: Grid spacing

    **Examples**

    Creating a map grid::

        import lagrangian

        # Define 100x100 grid covering 10°×10° region
        map_props = lagrangian.MapProperties(
            nx=100, ny=100,
            x_min=0.0, y_min=40.0,
            step=0.1  # 0.1° spacing
        )

        # Get coordinate arrays
        x_coords = map_props.x_axis()
        y_coords = map_props.y_axis()

    ----

    .. automethod:: x_axis

    ----

    .. automethod:: y_axis

    ----

    .. autoproperty:: nx

    ----

    .. autoproperty:: ny

    ----

    .. autoproperty:: x_min

    ----

    .. autoproperty:: y_min

    ----

    .. autoproperty:: step


Lyapunov Exponent Maps
----------------------

.. class:: MapOfFiniteLyapunovExponents

    Computes maps of finite-time Lyapunov exponents over a spatial grid.

    .. automethod:: __init__

    **Key Methods**

    * :py:meth:`compute()`: Compute FTLE/FSLE for all grid points
    * :py:meth:`map_of_lambda1()`: Get map of first Lyapunov exponent
    * :py:meth:`map_of_lambda2()`: Get map of second Lyapunov exponent
    * :py:meth:`map_of_theta1()`: Get map of first eigenvector angles
    * :py:meth:`map_of_theta2()`: Get map of second eigenvector angles
    * :py:meth:`map_of_delta_t()`: Get map of integration times
    * :py:meth:`map_of_final_separation()`: Get map of final separations

    **Examples**

    Computing FSLE maps::

        import lagrangian
        import numpy as np
        from datetime import datetime, timedelta

        # Setup field and integration
        field = lagrangian.core.field.TimeSerie("config.ini")

        fsle_integration = lagrangian.FiniteLyapunovExponentsIntegration(
            start_time=datetime(2010, 1, 1),
            end_time=datetime(2010, 1, 10),
            delta_t=timedelta(hours=1),
            mode=lagrangian.IntegrationMode.FSLE,
            min_separation=0.1,
            delta=0.01,
            field=field
        )

        # Define spatial grid
        map_props = lagrangian.MapProperties(
            nx=50, ny=50,
            x_min=5.0, y_min=40.0,
            step=0.2
        )

        # Compute FSLE map
        fsle_map = lagrangian.MapOfFiniteLyapunovExponents(
            map_properties=map_props,
            fle=fsle_integration,
            stencil=lagrangian.Stencil.QUINTUPLET
        )

        # Run computation (parallel)
        fsle_map.compute(num_threads=4)

        # Extract results
        lambda1_map = fsle_map.map_of_lambda1(fill_value=np.nan)
        lambda2_map = fsle_map.map_of_lambda2(fill_value=np.nan)
        time_map = fsle_map.map_of_delta_t(fill_value=np.nan)

    ----

    .. automethod:: compute

    ----

    .. automethod:: map_of_lambda1

    ----

    .. automethod:: map_of_lambda2

    ----

    .. automethod:: map_of_theta1

    ----

    .. automethod:: map_of_theta2

    ----

    .. automethod:: map_of_delta_t

    ----

    .. automethod:: map_of_final_separation


Utilities and Helpers
=====================

Additional utility classes and functions.

Cell Properties
---------------

.. class:: CellProperties

    Properties of grid cells for interpolation and computation.

    **Methods**

    * :py:meth:`none()`: Create empty cell properties

    ----

    .. automethod:: none


Time Handling
-------------

.. class:: DateTime

    Date and time representation for integration.

    **Methods**

    * :py:meth:`to_datetime()`: Convert to Python datetime object

    ----

    .. method:: to_datetime() -> datetime.datetime

        Convert to Python datetime object.

        :return: Python datetime representation of this DateTime


.. class:: TimeDuration

    **Methods**

    * :py:meth:`to_timedelta()`: Convert to Python timedelta object

    ----

    .. method:: to_timedelta() -> datetime.timedelta

        Convert to Python timedelta object.

        :return: Python timedelta representation of this TimeDuration


Debug and Information
---------------------

.. autofunction:: debug

    Print debug message to console.

.. autofunction:: set_verbose

    Enable or disable verbose output.

    **Parameters**

    * ``value``: Boolean flag for verbose mode

.. autofunction:: version

    Get library version string.

    **Returns**

    Version string in format "major.minor.patch"

    **Examples**

    Checking version and enabling debug output::

        import lagrangian

        print(f"Lagrangian library version: {lagrangian.version()}")

        # Enable verbose output
        lagrangian.set_verbose(True)

        # Print debug message
        lagrangian.debug("Starting computation...")
