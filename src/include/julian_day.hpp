/*
    This file is part of lagrangian library.

    lagrangian is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lagrangian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// ___________________________________________________________________________//

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/regex.hpp>

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * @brief Handle a julian day as a triplet: day, seconds and microseconds.
 */
class JulianDay
{
private:
    /**
     * Number of seconds per day
     */
    static const int seconds_per_day_ = 86400;

    /**
     * Number of microseconds per second
     */
    static const int microseconds_per_second_ = 1000000;

    /**
     * Julian day at midnight of January 1, 1970
     */
    static const int unix_time = 2440588;

    int day_;
    int seconds_;
    int microseconds_;

    /**
     * @brief Comparison of Julian days
     *
     * @param j julian day compared with the current instance.
     *
     * @return The difference of the julian day of the instance minus
     * the julian day passed as parameter.
     */
    int Compare(const JulianDay& j) const
    {
        int n = this->get_day() - j.get_day();

        if (n == 0)
        {
            n = this->get_seconds() - j.get_seconds();

            if (n == 0)
            {
                n = this->get_microseconds() - j.get_microseconds();
            }
        }
        return n;
    }

    /**
     * @brief adjusts the seconds and microseconds in their domain after an
     * arithmetic operation.
     */
    void AdjustDomain();

public:
    /**
     * @brief Construct a julian day from a boost::posix_time::ptime object
     *
     * @param t Instance of a ptime object. If t isn't set, the new instance
     * will be contain the current date.
     */
    JulianDay(const boost::posix_time::ptime& t =
            boost::posix_time::microsec_clock::universal_time())
    {
        day_ = t.date().julian_day();
        seconds_ = t.time_of_day().total_seconds();
        microseconds_ = static_cast<int> (t.time_of_day().total_microseconds()
                - seconds_ * microseconds_per_second_);
    }

    /**
     * @brief Creating a julian day from string.
     *
     * @param s String to parse.
     */
    JulianDay(const std::string& s)
    {
        *this = JulianDay(boost::posix_time::time_from_string(s));
    }

    /**
     * @brief Creating a julian day from a triplet: day, seconds and microseconds.
     *
     * @param day julian day
     * @param seconds Seconds
     * @param microseconds Microseconds
     *
     * @throw std::invalid_argument if the number of seconds is greater than 86400
     * or the number of microseconds is greater than 1000000
     */
    JulianDay(
        const int day,
        const int seconds = 0,
        const int microseconds = 0) :
      day_(day), seconds_(seconds), microseconds_(microseconds)
    {
        if (seconds_ > seconds_per_day_)
            std::invalid_argument("seconds must be less than 86400");

        if (microseconds_ > microseconds_per_second_)
            std::invalid_argument("microseconds must be less than 1000000");
    }

    /**
     * @brief Creating a julian day from a fractional julian day
     *
     * @param jd julian day
     */
    JulianDay(const long double jd)
    {
        long double fractional, integer;
        long double integer_part = static_cast<int>(jd);

        fractional = boost::math::round<long double>(modfl(jd - integer_part,
              &integer) * 864e8L) * 1e-6L;

        day_ = static_cast<int> (integer_part);
        microseconds_ = static_cast<int> (modfl(fractional, &integer) * 1e6L);
        seconds_ = static_cast<int> (integer);
    }

    /**
     * @brief Default method invoked when a JulianDay is destroyed.
     */
    virtual ~JulianDay()
    {
    }

    /**
     * @brief Computes a julian day from Unix Time
     *
     * @param time Number of seconds elapsed since 1970
     *
     * @return The julian day
     */
    inline static JulianDay FromUnixTime(const long double time)
    {
        JulianDay result(time / 86400.0L);
        result.day_ += unix_time;
        return result;
    }

    /**
     * @brief Returns the julian day
     *
     * @return The julian day
     */
    inline int get_day() const
    {
        return day_;
    }

    /**
     * @brief Returns the number of seconds since the start of the day.
     *
     * @return The number of second
     */
    inline int get_seconds() const
    {
        return seconds_;
    }

    /**
     * @brief Returns the number of microseconds.
     *
     * @return The number of microseconds
     */
    inline int get_microseconds() const
    {
        return microseconds_;
    }

    /**
     * @brief Extends the date by adding an other julian day
     *
     * @param jd Julian day to add
     *
     * @return A reference to the extented julian day
     */
    JulianDay& operator+=(const JulianDay& jd);

    /**
     * @brief Reduces the date by subtracting an other julian day
     *
     * @param jd Julian day to add
     *
     * @return A reference to the reduced julian day
     */
    JulianDay& operator-=(const JulianDay& jd);

    /**
     * @brief Tests if this julian day is earlier than the other date.
     *
     * @param jd Julian day to compare
     *
     * @return True if this julian day is earlier than the other date,
     * otherwise false
     */
    inline bool operator <(const JulianDay& jd) const
    {
        return Compare(jd) < 0;
    }

    /**
     * @brief Tests if this julian day is later than the other date.
     *
     * @param jd Julian day to compare
     *
     * @return True if this julian day is later than the other date,
     * otherwise false
     */
    inline bool operator >(const JulianDay& jd) const
    {
        return Compare(jd) > 0;
    }

    /**
     * @brief Tests if this julian day is earlier than or equal to the other
     * date.
     *
     * @param jd Julian day to compare
     *
     * @return True if this julian day is earlier than or equal to the other
     * date, otherwise false
     */
    inline bool operator <=(const JulianDay& jd) const
    {
        return Compare(jd) <= 0;
    }

    /**
     * @brief Tests if this julian day is later than or equal to the other date.
     *
     * @param jd Julian day to compare
     *
     * @return True if this julian day is later than or equal to the other date,
     * otherwise false
     */
    inline bool operator >=(const JulianDay& jd) const
    {
        return Compare(jd) >= 0;
    }

    /**
     * @brief Tests if this julian day is equal to the other date.
     *
     * @param jd Julian day to compare
     *
     * @return True if this julian day is equal to the other date,
     * otherwise false
     */
    inline bool operator ==(const JulianDay& jd) const
    {
        return Compare(jd) == 0;
    }

    /**
     * @brief Tests if this julian day is different from the other date.
     *
     * @param jd Julian day to compare
     *
     * @return True if this julian day is different from the other date,
     * otherwise false
     */
    inline bool operator !=(const JulianDay& jd) const
    {
        return Compare(jd) != 0;
    }

    /**
     * @brief Converts the julian day as a fractionnal julian day
     *
     * @return the fractionnal julian day
     */
    virtual operator long double() const
    {
        return day_ + (seconds_ + microseconds_ * 1e-6L) / 86400.0L;
    }

    /**
     * @brief Converts a julian day to Unix Time
     *
     * @return the number of seconds elapsed since midnight Coordinated
     * Universal Time (UTC) of January 1, 1970, not counting leap seconds.
     */
    inline long double ToUnixTime() const
    {
        return (day_ - unix_time) * 86400.0L + seconds_ + microseconds_ * 1e-6L;
    }

    /**
     * @brief Makes a copy of an other julian day
     *
     * @param other An other julian day
     *
     * @return a reference to the copy
     */
    JulianDay& operator=(const JulianDay& other)
    {
        day_ = other.day_;
        seconds_ = other.seconds_;
        microseconds_ = other.microseconds_;

        return *this;
    }

    /**
     * @brief Returns a boost::posix_time::ptime object
     *
     * @return a new instance of a boost::posix_time::ptime object
     */
    boost::posix_time::ptime ToPtime() const;

    /**
     * @brief Converts a julian day to a string according the given format.
     *
     * @param format String format
     *
     * @return The string representation of the julian day
     *
     * @see
     * @htmlonly
     * See <a href="http://www.boost.org/doc/html/date_time.html">
     * http://www.boost.org/doc/html/date_time.html</a> for more details about
     * possibles date format.
     * @endhtmlonly
     */
    std::string ToString(const std::string& format) const;
};

// ___________________________________________________________________________//

/**
 * @brief Adds two julian day
 *
 * @param a A julian day
 * @param b An other julian day
 *
 * @return A new julian day that contains the sum of the two julian day
 */
inline JulianDay operator+(const JulianDay& a, const JulianDay& b)
{
    JulianDay r = a;
    return r += b;
}

/**
 * @brief Removes two julian day
 *
 * @param a A julian day
 * @param b An other julian day
 *
 * @return A new julian day that contains the subtraction of the two julian day
 */
inline JulianDay operator-(const JulianDay& a, const JulianDay& b)
{
    JulianDay r = a;
    return r -= b;
}

/**
 * @brief Writes the julian day to the stream
 *
 * @param os The stream used to write
 * @param jd The julian day to write
 *
 * @return a reference to the stream updated
 */
inline std::ostream& operator<<(std::ostream& os, const JulianDay& jd)
{
    return os << jd.get_day() << " " << jd.get_seconds() << " "
        << jd.get_microseconds();
}

/**
 * @brief Reads a julian from the stream
 *
 * @param is The stream used to read
 * @param jd The julian day read
 *
 * @return a reference to the stream updated
 */
inline std::istream& operator>>(std::istream& is, JulianDay& jd)
{
    static boost::regex triplet("(\\d+)\\s+(\\d+)\\s+(\\d+)");
    static boost::regex real("([-+]?\\d*\\.?\\d+)");
    boost::smatch match;

    std::string line;
    std::getline(is, line);

    // if the line contains three integers we construct a julian day from a
    // triplet: date, seconds, microseconds
    if (boost::regex_match(line, match, triplet))
    {
        jd = JulianDay(
            boost::lexical_cast<int>(match[1]),
            boost::lexical_cast<int>(match[2]),
            boost::lexical_cast<int>(match[3]));
    }
    // if the line contains a floating point number we construct a julian day
    // from a fractional julian day
    else if (boost::regex_match(line, match, real))
    {
        jd = JulianDay(boost::lexical_cast<long double>(match[1]));
    }
    // otherwise we try to build a julian day from a string
    else
    {
        jd = JulianDay(line);
    }
    return is;
}

// ___________________________________________________________________________//

/**
 * @brief Handle a Julian day as a triplet, day, seconds and
 * microseconds, for different reference epoch.
 */
template<int GAP>
class AbstractModifiedJulianDay: public JulianDay
{
protected:

    /**
     * @brief Convert a modified julian day to a julian day
     *
     * @param mjd Modified julian day to process
     *
     * @return The julian day
     */
    template<typename T>
    static inline T ToJulianDay(const T mjd)
    {
        return mjd + GAP;
    }

public:

    /**
     * @brief Construct a modified julian day from a
     * boost::posix_time::ptime object
     *
     * @param t Instance of a ptime object. If t isn't set, the new instance
     * will be contain the current date.
     */
    AbstractModifiedJulianDay(const boost::posix_time::ptime& t =
        boost::posix_time::microsec_clock::universal_time()) :
      JulianDay(t)
    {
    }

    /**
     * @brief Creating a modified julian day from string.
     *
     * @param s String to parse.
     */
    AbstractModifiedJulianDay(const std::string& s) :
      JulianDay(s)
    {
    }

    /**
     * @brief Creating a modified julian day from a triplet: day, seconds and
     * microseconds.
     *
     * @param day julian day
     * @param seconds Seconds
     * @param microseconds Microseconds
     *
     * @throw std::invalid_argument if the number of seconds is greater than 86400
     * or the number of microseconds is greater than 1000000
     */
    AbstractModifiedJulianDay(
        const int day,
        const int seconds = 0,
        const int microseconds = 0) :
      JulianDay(ToJulianDay<int> (day), seconds, microseconds)
    {
    }

    /**
     * @brief Creating a modified julian day from a fractional julian day
     *
     * @param day julian day
     */
    AbstractModifiedJulianDay(const long double day) :
      JulianDay(ToJulianDay<long double> (day))
    {
    }

    /**
     * @brief Default method invoked when a AbstractModifiedJulianDay
     * is destroyed.
     */
    virtual ~AbstractModifiedJulianDay()
    {
    }

    /**
     * @brief Computes a modified julian day from Unix Time
     *
     * @return The modified julian day
     */
    static AbstractModifiedJulianDay<GAP> FromUnixTime(const long double time)
    {
        JulianDay jd(JulianDay::FromUnixTime(time));
        return AbstractModifiedJulianDay<GAP>(
            jd.get_day() - GAP,
            jd.get_seconds(),
            jd.get_microseconds());
    }

    /**
     * @brief Returns the modified julian day
     *
     * @return The modified julian day
     */
    inline int GetModifiedJulianDay() const
    {
        return get_day() - GAP;
    }

    /**
     * @brief Returns the decimal julian day
     *
     * @return The decimal julian day
     */
    operator long double() const
    {
        return GetModifiedJulianDay() + (get_seconds() + get_microseconds()
            * 1e-6L) / 86400.0L;
    }

    /**
     * @brief Returns the julian day number that defines the modified julian
     * day.
     *
     * @return the julian day of reference
     */
    static JulianDay Gap()
    {
        return JulianDay(static_cast<long double> (GAP));
    }

    using JulianDay::operator=;
};

// ___________________________________________________________________________//

/**
 * @brief Writes the AbstractModifiedJulianDay to the stream
 *
 * @param os The stream used to write
 * @param mjd The modified julian day to write
 *
 * @return a reference to the stream updated
 */
template<int GAP>
inline std::ostream& operator<<(
    std::ostream& os,
    const AbstractModifiedJulianDay<GAP>& mjd)
{
    return os << mjd.GetModifiedJulianDay() << " " << mjd.get_seconds() << " "
        << mjd.get_microseconds();
}

/**
 * @brief Reads an AbstractModifiedJulianDay from the stream
 *
 * @param is The stream used to read
 * @param mjd The modified julian day read
 *
 * @return a reference to the stream updated
 */
template<int GAP>
inline std::istream& operator>>(
    std::istream& is,
    AbstractModifiedJulianDay<GAP>& mjd)
{
      JulianDay jd;
      is >> jd;
      mjd = AbstractModifiedJulianDay<GAP> (
          jd.get_day(),
          jd.get_seconds(),
          jd.get_microseconds());
      return is;
}

// ___________________________________________________________________________//

/**
 * @brief Handle a modified julian day as a triplet: day, seconds and
 * microseconds.
 *
 * Modified julian Day is defined as the julian Day minus 2400000.5.
 * Thus MJD 0 is at midnight between the 16 and 17 November 1858 AD Gregorian.
 * (boost use a rounded gap between julian day and modified julian day)
 */
typedef AbstractModifiedJulianDay<2400001> ModifiedJulianDay;

// ___________________________________________________________________________//

/**
 * @brief Handle a CNES Julian day as a triplet: day, seconds and microseconds.
 *
 * A CNES Julian day is defined as Modified Julian Day minus 33282.
 * Thus CNES 0 is at midnight between the 31 December and 01 January 1950 AD
 * Gregorian.
 */
typedef AbstractModifiedJulianDay<2400001 + 33282> CNESJulianDay;

// ___________________________________________________________________________//

/**
 * @brief Handle a LOP Julian day as a triplet: day, seconds and microseconds.
 *
 * A LOP Julian day is defined as Modified Julian Day minus 48622.
 * Thus LOP 0 is at midnight between the 31 December and 01 January 1992 AD
 * Gregorian.
 */
typedef AbstractModifiedJulianDay<2400001 + 48622> LOPJulianDay;

// ___________________________________________________________________________//

/**
 * @brief Handle a NASA Julian day as a triplet: day, seconds and microseconds.
 *
 * A NASA Julian day is defined as Modified Julian Day minus 40000.
 * Thus NASA 0 is at midnight between the 23 May and 24 May 1968 AD Gregorian,
 * at which time the Apollo missions to the Moon were underway.
 */
typedef AbstractModifiedJulianDay<2400001 + 40000> NASAJulianDay;

// ___________________________________________________________________________//

/**
 * @brief Handle a CCSDS Julian day as a triplet: day, seconds and microseconds.
 *
 * A CCSDS Julian day is defined as Modified Julian Day minus 36204.
 * Thus CCSDS 0 is at midnight between the 31 December and 01 January 1958 AD
 * Gregorian.
 */
typedef AbstractModifiedJulianDay<2400001 + 36204> CCSDSJulianDay;

}