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

#include <boost/date_time/posix_time/posix_time.hpp>

namespace lagrangian
{

/**
 * A datetime object is a single object containing all the information from a
 * date object and a time object. Like a date object, datetime assumes the
 * current Gregorian calendar extended in both directions; like a time object,
 * datetime assumes there are exactly 3600*24 seconds in every day.
 */
class DateTime
{
private:
    boost::posix_time::ptime datetime_;

public:

    /**
     * @brief Construct a datetime from a boost::posix_time::ptime object
     *
     * @param datetime Instance of a ptime object. If t isn't set, the new
     * instance will be contain the current date.
     */
    DateTime(const boost::posix_time::ptime& datetime =
            boost::posix_time::microsec_clock::universal_time()) :
            datetime_(datetime)
    {
    }

    /**
     * @brief Creating a datetime from string.
     *
     * @param s String to parse.
     */
    DateTime(const std::string& s) :
            datetime_(boost::posix_time::time_from_string(s))
    {
    }

    /**
     * @brief Creating a datetime from a time map.
     *
     * @param time_map time map to parse.
     */
    DateTime(const struct tm& time_map) :
            datetime_(boost::posix_time::ptime_from_tm(time_map))
    {
    }

    /**
     * @brief Computes a datetime from Unix Time
     *
     * @param epoch Number of seconds elapsed since 1970
     *
     * @return The datetime
     */
    inline static DateTime FromUnixTime(const long double epoch)
    {
        long double integer;
        long double fractional = std::modf(epoch, &integer);

        boost::posix_time::ptime datetime(
                boost::posix_time::from_time_t(integer));
        datetime += boost::posix_time::microseconds(fractional);

        return DateTime(datetime);
    }

    /**
     * @brief Get the year part of the date.
     *
     * @return Year
     */
    inline int Year() const
    {
        return datetime_.date().year();
    }

    /**
     * @brief Get the month part of the date.
     *
     * @return Month between 1 and 12 inclusive.
     */
    inline int Month() const
    {
        return datetime_.date().month();
    }

    /**
     * @brief Get the day part of the date.
     *
     * @return Between 1 and the number of days in the given month of the given year.
     */
    inline int Day() const
    {
        return datetime_.date().day();
    }

    /**
     * @brief Get the number of normalized hours
     *
     * @return Hour between 0 and 23 inclusive.
     */
    inline int Hour() const
    {
        return datetime_.time_of_day().hours();
    }

    /**
     * @brief Get the number of normalized minutes
     *
     * @return Minute between 0 and 59 inclusive.
     */
    inline int Minute() const
    {
        return datetime_.time_of_day().minutes();
    }

    /**
     * @brief Get the normalized number of second
     *
     * @return Minute between 0 and 59 inclusive.
     */
    inline int Second() const
    {
        return datetime_.time_of_day().seconds();
    }

    /**
     * @brief Get the normalized number of microsecond
     *
     * @return Microsecond between 0 and 999999 inclusive
     */
    inline int Microsecond() const
    {
        static int ticks = boost::posix_time::time_duration::ticks_per_second();
        boost::posix_time::time_duration time_duration =
                datetime_.time_of_day();
        int fractional = time_duration.fractional_seconds();

        return ticks > 1000000 ?
                fractional / (ticks * 1e-6) : fractional * (1000000 / ticks);
    }

    /**
     * @brief Converts a datetime to Unix Time
     *
     * @return the number of seconds elapsed since midnight Coordinated
     * Universal Time (UTC) of January 1, 1970, not counting leap seconds.
     */
    inline long double ToUnixTime() const
    {
        static boost::posix_time::ptime epoch(
                boost::gregorian::date(1970, 1, 1));
        return (datetime_ - epoch).total_seconds() + Microsecond() * 1e-6;
    }

    /**
     * @brief Tests if this datetime is earlier than the other datetime.
     *
     * @param datetime DateTime to compare
     *
     * @return True if this datetime is earlier than the other datetime,
     * otherwise false
     */
    inline bool operator <(const DateTime& datetime) const
    {
        return datetime_ < datetime.datetime_;
    }

    /**
     * @brief Tests if this datetime is later than the other datetime.
     *
     * @param datetime DateTime to compare
     *
     * @return True if this datetime is later than the other datetime,
     * otherwise false
     */
    inline bool operator >(const DateTime& datetime) const
    {
        return datetime_ > datetime.datetime_;
    }

    /**
     * @brief Tests if this datetime is earlier than or equal to the other
     *     datetime.
     *
     * @param datetime DateTime to compare
     *
     * @return True if this datetime is earlier than or equal to the other
     *     datetime, otherwise false
     */
    inline bool operator <=(const DateTime& datetime) const
    {
        return datetime_ <= datetime.datetime_;
    }

    /**
     * @brief Tests if this datetime is later than or equal to the other
     *     datetime.
     *
     * @param datetime DateTime to compare
     *
     * @return True if this datetime is later than or equal to the other
     *     datetime, otherwise false
     */
    inline bool operator >=(const DateTime& datetime) const
    {
        return datetime_ >= datetime.datetime_;
    }

    /**
     * @brief Tests if this datetime is equal to the other datetime.
     *
     * @param datetime DateTime to compare
     *
     * @return True if this datetime is equal to the other datetime, otherwise
     *     false
     */
    inline bool operator ==(const DateTime& datetime) const
    {
        return datetime_ == datetime.datetime_;
    }

    /**
     * @brief Tests if this datetime is different from the other datetime.
     *
     * @param datetime DateTime to compare
     *
     * @return True if this datetime is different from the other datetime,
     *     otherwise false
     */
    inline bool operator !=(const DateTime& datetime) const
    {
        return datetime_ != datetime.datetime_;
    }

    /**
     * @brief Converts a datetime to a string according the given format.
     *
     * @param format String format
     *
     * @return The string representation of the datetime
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

}
