Date types
==========

Classes for manipulating date.

JulianDay
---------

.. py:class:: lagrangian.JulianDay

    Handle a Julian day as a triplet: day, seconds and microseconds.        

    .. py:method:: __init__(date=datetime.datetime.now())

        Constructs a Julian day from a datetime.datetime object

        :param date: Instance of a datetime.datetime object. If t isn't set,
            the new instance will be contain the current date.
        :type date: datetime.datetime

    .. py:method:: __init__(s)

        Constructs a Julian day from string.
        
        :param s: The date to parse
        :type s: str

    .. py:method:: __init__(day, seconds=0, microseconds=0)

        Constructs a Julian day from a triplet: day, seconds and microseconds
        
        :param day: Julian day
        :type day: int
        :param seconds: Seconds
        :type seconds: int
        :param microseconds: Microseconds
        :type microseconds: int

    .. py:method:: __init__(day)

        Creates a JulianDay from a fractional Julian day
        
        :param day: Fractional Julian day
        :type day: float

    .. py:attribute:: day

        The Julian day

    .. py:attribute:: seconds

        The number of seconds since the start of the day.

    .. py:attribute:: microseconds

        The number of microseconds.

    .. py:staticmethod:: FromUnixTime(time)

        Computes a Julian day from POSIX timestamp

        :param time: Number of seconds elapsed since 1970
        :type time: float
        :return: The Julian day
        :rtype: :py:class:`lagrangian.JulianDay`

    .. py:method:: ToPtime():

        Returns the datetime.datetime representation of the instance

        :return: The datetime.datetime representation of the instance
        :type: datetime.datetime

    .. py:method:: ToString(format)

        Converts a Julian day to a string according the given format.

        :param format: String format
        :type format: str
        :return: The string representation of the Julian day
        :rtype: str

        .. note::

            See http://www.boost.org/doc/html/date_time.html for more details
            about possibles format.

    .. py:method:: ToUnixTime()

        :return: The number of seconds elapsed since midnight Coordinated
            Universal Time (UTC) of January 1, 1970, not counting leap
            seconds.
        :rtype: float 

    .. py:method:: __float__()

        Converts the Julian day as a fractionnal Julian day

        :return: the fractionnal Julian day
        :rtype: float 

    .. py:method:: __str__()

        Return a string representing the julian date

        :return: the string representing the date
        :rtype: str

AbstractModifiedJulianDay
-------------------------

.. py:class:: lagrangian.AbstractModifiedJulianDay

    Base: :py:class:`lagrangian.JulianDay`

    .. note::

        This is a template class that can not be instantiated from Python.

    Handle a Julian day as a triplet, day, seconds and
    microseconds, for different reference epoch.

    .. py:staticmethod:: FromUnixTime(time)

        Computes a modified Julian day from POSIX timestamp

        :param time: Number of seconds elapsed since 1970
        :type time: float
        :return: The modified Julian day
        :rtype: :py:class:`lagrangian.AbstractModifiedJulianDay`

    .. py:staticmethod:: Gap()

        Returns the Julian day number that defines the modified julian

        :return: the Julian day of reference
        :rtype: :py:class:`lagrangian.JulianDay`

    .. py::method:: GetModifiedJulianDay()

        Returns the modified Julian day

        :return: The modified Julian day
        :type: int

ModifiedJulianDay
-----------------

.. py:class:: lagrangian.ModifiedJulianDay

    Base: :py:class:`lagrangian.AbstractModifiedJulianDay`

    Handle a modified Julian day as a triplet: day, seconds and
    microseconds

    Modified julian Day is defined as the julian Day minus 2400000.5.
    Thus MJD 0 is at midnight between the 16 and 17 November 1858 AD
    Gregorian.

CNESJulianDay
-------------

.. py:class:: lagrangian.CNESJulianDay

    Base: :py:class:`lagrangian.AbstractModifiedJulianDay`

    Handle a CNES Julian day as a triplet: day, seconds and microseconds.
 
    A CNES Julian day is defined as Modified Julian Day minus 33282.
    Thus CNES 0 is at midnight between the 31 December and 01 January 1950 AD
    Gregorian.

LOPJulianDay
------------

.. py:class:: lagrangian.LOPJulianDay

    Base: :py:class:`lagrangian.AbstractModifiedJulianDay`

    Handle a LOP Julian day as a triplet: day, seconds and microseconds.
 
    A LOP Julian day is defined as Modified Julian Day minus 48622.
    Thus LOP 0 is at midnight between the 31 December and 01 January 1992 AD
    Gregorian.

NASAJulianDay
-------------

.. py:class:: lagrangian.NASAJulianDay

    Base: :py:class:`lagrangian.AbstractModifiedJulianDay`

    Handle a NASA Julian day as a triplet: day, seconds and microseconds.

    A NASA Julian day is defined as Modified Julian Day minus 40000.
    Thus NASA 0 is at midnight between the 23 May and 24 May 1968 AD
    Gregorian, at which time the Apollo missions to the Moon were underway.

CCSDSJulianDay
--------------

.. py:class:: lagrangian.CCSDSJulianDay

    Base: :py:class:`lagrangian.AbstractModifiedJulianDay`

    Handle a CCSDS Julian day as a triplet: day, seconds and microseconds.
 
    A CCSDS Julian day is defined as Modified Julian Day minus 36204.
    Thus CCSDS 0 is at midnight between the 31 December and 01 January 1958 AD
    Gregorian.

Example usage
-------------

    >>> import lagrangian
    >>> import datetime
    >>> lagrangian.CNESJulianDay().ToPtime()
    datetime.datetime(2015, 1, 1, 0, 0, 0, 0)
    >>> jd = lagrangian.CNESJulianDay(datetime.datetime(2000, 1, 1))
    >>> str(jd)
    '18262 0 0'
    >>> jd.ToString("%Y-%m-%dT%H:%M:%s")
    '2000-01-01T00:00:00.000000'
    >>> n = datetime.datetime(2000, 1, 1, 23, 59, 59, 999999)
    >>> lagrangian.NASAJulianDay.FromUnixTime(n.timestamp())
    datetime.datetime(2000, 1, 1, 22, 59, 59, 999999)
    >>> jd.ToUnixTime()
    946684800.0
