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

#ifndef JULIAN_DAY_HPP_
#define JULIAN_DAY_HPP_

// ___________________________________________________________________________//

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/math/special_functions.hpp>

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * @brief Handle a Julian day as a triplet: day, seconds and microseconds.
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

    /*
     * @brief Comparison of Julian days
     *
     * @param j Julian day compared with the current instance.
     *
     * @return The difference of the Julian day of the instance minus
     * the Julian day passed as parameter.
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

public:
    /**
     * @brief Construct a Julian day from a boost::posix_time::ptime object
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
     * @brief Creating a Julian day from string.
     *
     * @param s String to parse.
     */
    JulianDay(const std::string& s)
    {
        *this = JulianDay(boost::posix_time::time_from_string(s));
    }

    /**
     * @brief Creating a Julian day from a triplet: day, seconds and microseconds.
     *
     * @param day Julian day
     * @param seconds Seconds
     * @param microseconds Microseconds
     *
     * @throw std::runtime_error if the number of seconds is greater than 86400
     * or the number of microseconds is greater than 1000000
     */
    JulianDay(const int day, const int seconds, const int microseconds) :
        day_(day), seconds_(seconds), microseconds_(microseconds)
    {
        if (seconds_ > seconds_per_day_)
            std::invalid_argument("seconds must be less than 86400");

        if (microseconds_ > microseconds_per_second_)
            std::invalid_argument("microseconds must be less than 1000000");
    }

    /**
     * @brief Creating a Julian day from a fractional Julian day
     *
     * @param jd Julian day
     */
    JulianDay(const double jd)
    {
        double fractional, integer;
        fractional = modf(jd, &integer) * 86400;
        day_ = static_cast<int> (integer);
        microseconds_ = modf(fractional, &integer) * 1e6;
        seconds_ = static_cast<int> (integer);
    }

    /**
     * @brief Computes a Julian day from Unix Time
     *
     * @return The julian day
     */
    static double JulianDayFromUnixTime(const double time)
    {
        return time / 86400.0 + unix_time;
    }

    /**
     * @brief Returns the julian day
     *
     * @return The Julian day
     */
    int get_day() const
    {
        return day_;
    }

    /**
     * @brief Returns the number of seconds since the start of the day.
     *
     * @return The number of second
     */
    int get_seconds() const
    {
        return seconds_;
    }

    /**
     * @brief Returns the number of seconds since the start of the day.
     *
     * @return The number of second
     */
    int get_microseconds() const
    {
        return microseconds_;
    }

    JulianDay& operator+=(const JulianDay& j);
    JulianDay& operator-=(const JulianDay& j);

    bool operator <(const JulianDay& jd) const
    {
        return Compare(jd) < 0;
    }

    bool operator >(const JulianDay& jd) const
    {
        return Compare(jd) > 0;
    }

    bool operator <=(const JulianDay& jd) const
    {
        return Compare(jd) <= 0;
    }

    bool operator >=(const JulianDay& jd) const
    {
        return Compare(jd) >= 0;
    }

    bool operator ==(const JulianDay& jd) const
    {
        return Compare(jd) == 0;
    }

    bool operator !=(const JulianDay& jd) const
    {
        return Compare(jd) != 0;
    }

    virtual operator double() const
    {
        return day_ + (seconds_ + microseconds_ * 1e-6) / 86400.0;
    }

    /**
     * @brief Converts a Julian day to Unix Time
     *
     * @return the number of seconds elapsed since midnight Coordinated
     * Universal Time (UTC) of January 1, 1970, not counting leap seconds.
     */
    double ToUnixTime() const
    {
        return (day_ * 86400.0 + seconds_ + microseconds_ * 1e-6) - unix_time
                * 86400.0;
    }

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
     * @return a new intance of a boost::posix_time::ptime object
     */
    boost::posix_time::ptime ToPtime() const;

    /**
     * @brief Converts a Julian day to a string according the given format.
     *
     * @param format String format
     *
     * @return The string representation of the Julian day
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

inline JulianDay operator+(const JulianDay& a, const JulianDay& b)
{
    JulianDay r = a;
    return r += b;
}

inline JulianDay operator-(const JulianDay& a, const JulianDay& b)
{
    JulianDay r = a;
    return r -= b;
}

inline std::ostream& operator<<(std::ostream& os, const JulianDay& j)
{
    return os << j.get_day() << " " << j.get_seconds() << " "
            << j.get_microseconds();
}

// ___________________________________________________________________________//

/**
 * @brief Handle a modified Julian day as a triplet: day, seconds and
 * microseconds.
 *
 * Modified Julian Day is defined as the Julian Day minus 2400000.5.
 * Thus MJD 0 is at midnight between the 16 and 17 November 1858 AD Gregorian.
 */
class ModifiedJulianDay: public JulianDay
{
private:
    // boost use a prerounded gap between julian day and modified julian day
    static const int gap_ = 2400001;

protected:

    /**
     * @brief Convert a modified Julian day to a Julian day
     *
     * @param mjd Modified Julian day to process
     * @param gap Modified Julian day minus "gap" to compute a modified julian
     * day derived.
     *
     * @return The Julian day
     */
    template<class T>
    static T ToJulianDay(const T mjd, const int gap = 0)
    {
        return mjd + gap + gap_;
    }

    /**
     * @brief Convert a Julian day to a modified Julian day
     *
     * @param jd Julian day to process
     * @param gap Modified Julian day minus "gap" to compute a modified julian
     * day derived.
     *
     * @return The modified Julian day
     */
    static int ToModifiedJulianDay(const int jd, const int gap = 0)
    {
        return jd - gap - gap_;
    }

public:

    ModifiedJulianDay() :
        JulianDay()
    {
    }

    ModifiedJulianDay(const std::string& s) :
        JulianDay(s)
    {
    }

    ModifiedJulianDay(const int day,
            const int seconds = 0,
            const int microseconds = 0,
            const int gap = 0) :
        JulianDay(ToJulianDay<int> (day, gap), seconds, microseconds)
    {
    }

    ModifiedJulianDay(const double day, const int gap = 0) :
        JulianDay(ToJulianDay<double> (day, gap))
    {
    }

    /**
     * @brief Returns the modified Julian day
     *
     * @return The modified julian day
     */
    virtual int GetModifiedJulianDay() const
    {
        return ToModifiedJulianDay(get_day());
    }

    operator double() const
    {
        return GetModifiedJulianDay() + (get_seconds() + get_microseconds()
                * 1e-6) / 86400.0;
    }
    using JulianDay::operator=;
};
// ___________________________________________________________________________//

inline std::ostream& operator<<(std::ostream& os, const ModifiedJulianDay& j)
{
    return os << j.GetModifiedJulianDay() << " " << j.get_seconds() << " "
            << j.get_microseconds();
}

// ___________________________________________________________________________//

/**
 * @brief Handle a CNES Julian day as a triplet: day, seconds and microseconds.
 *
 * A CNES Julian day is defined as Modified Julian Day minus 33282.
 * Thus CNES 0 is at midnight between the 31 December and 01 January 1950 AD
 * Gregorian.
 */
class CNESJulianDay: public ModifiedJulianDay
{
private:
    static const int gap_ = 33282;
public:

    CNESJulianDay() :
        ModifiedJulianDay()
    {
    }

    CNESJulianDay(const std::string& s) :
        ModifiedJulianDay(s)
    {
    }

    CNESJulianDay(const double jd) :
        ModifiedJulianDay(jd, gap_)
    {
    }

    CNESJulianDay(const int day,
            const int seconds = 0,
            const int microseconds = 0U) :
        ModifiedJulianDay(day, seconds, microseconds, gap_)
    {
    }

    /**
     * @brief Returns the CNES Julian day
     *
     * @return The CNES Julian day
     */
    int GetModifiedJulianDay() const
    {
        return ToModifiedJulianDay(get_day(), gap_);
    }

    using JulianDay::operator=;
};

// ___________________________________________________________________________//

/**
 * @brief Handle a LOP Julian day as a triplet: day, seconds and microseconds.
 *
 * A LOP Julian day is defined as Modified Julian Day minus 48622.
 * Thus LOP 0 is at midnight between the 31 December and 01 January 1992 AD
 * Gregorian.
 */
class LOPJulianDay: public ModifiedJulianDay
{
private:
    static const int gap_ = 48622;
public:
    LOPJulianDay() :
        ModifiedJulianDay()
    {
    }

    LOPJulianDay(const std::string& s) :
        ModifiedJulianDay(s)
    {
    }

    LOPJulianDay(const double jd) :
        ModifiedJulianDay(jd, gap_)
    {
    }

    LOPJulianDay(const int day,
            const int seconds = 0,
            const int microseconds = 0U) :
        ModifiedJulianDay(day, seconds, microseconds, gap_)
    {
    }

    /**
     * @brief Returns the LOP Julian day
     *
     * @return the LOP Julian day
     */
    int GetModifiedJulianDay() const
    {
        return ToModifiedJulianDay(get_day(), gap_);
    }

    using JulianDay::operator=;
};

// ___________________________________________________________________________//

/**
 * @brief Handle a NASA Julian day as a triplet: day, seconds and microseconds.
 *
 * A NASA Julian day is defined as Modified Julian Day minus 40000.
 * Thus NASA 0 is at midnight between the 23 May and 24 May 1968 AD Gregorian,
 * at which time the Apollo missions to the Moon were underway.
 */
class NASAJulianDay: public ModifiedJulianDay
{
private:
    static const int gap_ = 40000;
public:

    NASAJulianDay() :
        ModifiedJulianDay()
    {
    }

    NASAJulianDay(const std::string& s) :
        ModifiedJulianDay(s)
    {
    }

    NASAJulianDay(const double jd) :
        ModifiedJulianDay(jd, gap_)
    {
    }

    NASAJulianDay(const int day,
            const int seconds = 0,
            const int microseconds = 0U) :
        ModifiedJulianDay(day, seconds, microseconds, gap_)
    {
    }

    /**
     * @brief Returns the NASA Julian day
     *
     * @return the NASA Julian day
     */
    int GetModifiedJulianDay() const
    {
        return ToModifiedJulianDay(get_day(), gap_);
    }

    using JulianDay::operator=;
};

// ___________________________________________________________________________//

/**
 * @brief Handle a CCSDS Julian day as a triplet: day, seconds and microseconds.
 *
 * A CCSDS Julian day is defined as Modified Julian Day minus 36204.
 * Thus CCSDS 0 is at midnight between the 31 December and 01 January 1958 AD
 * Gregorian.
 */
class CCSDSJulianDay: public ModifiedJulianDay
{
private:
    static const int gap_ = 36204;
public:

    CCSDSJulianDay() :
        ModifiedJulianDay()
    {
    }

    CCSDSJulianDay(const std::string& s) :
        ModifiedJulianDay(s)
    {
    }

    CCSDSJulianDay(const double jd) :
        ModifiedJulianDay(jd, gap_)
    {
    }

    CCSDSJulianDay(const int day,
            const int seconds = 0,
            const int microseconds = 0U) :
        ModifiedJulianDay(day, seconds, microseconds, gap_)
    {
    }

    /**
     * @brief Returns the CCSDS Julian day

     * @return the CCSDS Julian day
     */
    int GetModifiedJulianDay() const
    {
        return ToModifiedJulianDay(get_day(), gap_);
    }

    using JulianDay::operator=;
};

}

#endif /* JULIAN_DAY_HPP_ */
