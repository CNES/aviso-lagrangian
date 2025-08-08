Units and Coordinates
=====================

Utilities for working with different unit systems and coordinate frames.

.. currentmodule:: lagrangian.core.axis

Axis Units
----------

Classes for validating and working with coordinate axis units.

.. class:: LatitudeUnit

    Utility for working with latitude units.

    .. automethod:: __init__

    **Methods**

    * :py:meth:`__call__(unit) <__call__>`: Check if unit string is a valid
        latitude unit

    **Examples**

    Validating latitude units::

        import lagrangian.axis as axis

        lat_unit = axis.LatitudeUnit()

        # Check various unit strings
        print(lat_unit("degrees_north"))  # True
        print(lat_unit("degree_north"))   # True
        print(lat_unit("deg_N"))          # True
        print(lat_unit("meters"))         # False

    ----

    .. automethod:: __call__

.. class:: LongitudeUnit

    Utility for working with longitude units.

    .. automethod:: __init__

    **Methods**

    * :py:meth:`__call__(unit) <__call__>`: Check if unit string is a valid
        longitude unit

    **Examples**

    Validating longitude units::

        import lagrangian.axis as axis

        lon_unit = axis.LongitudeUnit()

        # Check various unit strings
        print(lon_unit("degrees_east"))   # True
        print(lon_unit("degree_east"))    # True
        print(lon_unit("deg_E"))          # True
        print(lon_unit("radians"))        # False

    ----

    .. automethod:: __call__

.. currentmodule:: lagrangian.core.units

Unit Conversion
---------------

.. class:: Units

    Utility for unit conversion operations.

    **Methods**

    * :py:meth:`get_converter(from_unit, to_unit) <get_converter>`: Get
        conversion factor and offset

    **Examples**

    Converting between units::

        import lagrangian.units as units

        # Get conversion from m/s to cm/s
        factor, offset = units.Units.get_converter("m/s", "cm/s")

        # Convert velocity: cm_per_s = m_per_s * factor + offset
        velocity_ms = 1.5  # m/s
        velocity_cms = velocity_ms * factor + offset  # 150.0 cm/s

Common Unit Types
-----------------

The library supports several common unit types and coordinate systems:

**Velocity Units**

* ``m/s``: Meters per second
* ``cm/s``: Centimeters per second
* ``km/h``: Kilometers per hour
* ``deg/day``: Degrees per day (for angular velocities)

**Coordinate Units**

* ``degrees_north``, ``degree_north``, ``deg_N``: Latitude
* ``degrees_east``, ``degree_east``, ``deg_E``: Longitude
* ``meters``, ``m``: Metric distance
* ``kilometers``, ``km``: Metric distance

**Time Units**

* ``seconds``, ``s``: Time in seconds
* ``hours``, ``h``: Time in hours
* ``days``, ``d``: Time in days

Working with Different Coordinate Systems
------------------------------------------

The library supports both Cartesian and spherical coordinate systems:

**Cartesian Coordinates**

* Used for local, small-scale computations
* Units typically in meters
* x, y represent local coordinates

**Spherical Equatorial Coordinates**

* Used for global ocean/atmosphere applications
* Longitude/latitude in degrees
* Accounts for Earth's spherical geometry

**Examples**

Setting up fields with different coordinate systems::

    import lagrangian

    # Cartesian field (local coordinates)
    cartesian_field = lagrangian.Field(
        unit_type=lagrangian.UnitType.METRIC,
        coordinate_type=lagrangian.CoordinatesType.CARTESIAN
    )

    # Spherical field (global coordinates)
    spherical_field = lagrangian.Field(
        unit_type=lagrangian.UnitType.ANGULAR,
        coordinate_type=lagrangian.CoordinatesType.SPHERICAL_EQUATORIAL
    )
