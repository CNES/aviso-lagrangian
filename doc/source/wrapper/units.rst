Units of physical quantities.
=============================

AxisUnit
--------

.. py:class:: lagrangian.AxisUnit()

    Units known for a given type of axis.

    .. py:method:: __call__(self, unit)

        Checks if the unit can define this type of axis.

        :param unit: Unit to check
        :type unit: str
        :return: if the `unit` can define this type of axis
        :rtype: bool

AxisLatitudeUnit
----------------

.. py:class:: lagrangian.AxisLatitudeUnit()

    Base: :py:class:`lagrangian.AxisUnit`

    Units setting out latitudes

AxisLongitudeUnit
-----------------

.. py:class:: lagrangian.AxisLongitudeUnit()

    Base: :py:class:`lagrangian.AxisUnit`

    Units setting out longitudes

Units
-----

.. py:class:: lagrangian.Units()

    Provides support for units of physical quantities.

    .. py:staticmethod:: AreConvertible(unit1, unit2)

        Checks if numeric values in unit `unit1` are convertible to numeric
            values in unit `unit2`

        :param unit1: A unit.
        :type unit1: str
        :param unit2: Another unit.
        :type unit2: str
        :return: Numeric values can be converted between the units.
        :rtype: bool


    .. py:staticmethod:: GetConverter(from, to)

        Computes a converter of numeric values in unit `from` to numeric
            values in unit `to`.

        :param from: The unit from which to convert values.
        :type from: str
        :param to: The unit to which to convert values.
        :type to: str
        :return: A tuple that contains the offset and scale factor to convert
            from the unit `from` to the unit `to`.
        :rtype: tuple


    .. py:staticmethod:: IsTime(unit)

        Checks if numeric value in unit `unit` are convertible to time
        unit.

        :param unit: A unit.
        :type unit: str
        :return: If unit represents a time
        :rtype: bool

Example usage
-------------

    >>> import lagrangian
    >>> lagrangian.AxisLongitudeUnit()("degrees_east")
    True
    >>> lagrangian.Units.IsTime("m")
    False
    >>> lagrangian.Units.IsTime("seconds since 2000-01-01T00:00:00")
    True
    >>> lagrangian.Units.AreConvertible("m", "degrees")
    False
    >>> lagrangian.Units.AreConvertible("seconds", "minutes")
    True
    >>> lagrangian.Units.GetConverter("m", "km")
    (0.0, 0.001)
    >>> lagrangian.Units.GetConverter("m", "degrees")
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
    RuntimeError: the units 'm' and 'degrees' belong to the same unit-system but conversion between them is meaningless
