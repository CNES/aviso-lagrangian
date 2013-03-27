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

#include "julian_day.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

void JulianDay::AdjustDomain()
{
    if (microseconds_ >= microseconds_per_second_)
    {
        ++seconds_;
        microseconds_ -= microseconds_per_second_;
    }
    if (seconds_ >= seconds_per_day_)
    {
        ++day_;
        seconds_ -= seconds_per_day_;
    }

    if (microseconds_ < 0)
    {
        --seconds_;
        microseconds_ += microseconds_per_second_;
    }
    if (seconds_ < 0)
    {
        --day_;
        seconds_ += seconds_per_day_;
    }
}

// ___________________________________________________________________________//

JulianDay& JulianDay::operator+=(const JulianDay& jd)
{
    day_ += jd.get_day();
    microseconds_ += jd.get_microseconds();
    seconds_ += jd.get_seconds();

    AdjustDomain();

    return *this;
}

// ___________________________________________________________________________//

JulianDay& JulianDay::operator-=(const JulianDay& jd)
{
    day_ -= jd.get_day();
    microseconds_ -= jd.get_microseconds();
    seconds_ -= jd.get_seconds();

    AdjustDomain();

    return *this;
}

// ___________________________________________________________________________//

boost::posix_time::ptime JulianDay::ToPtime() const
{
    int h = seconds_ / 3600;
    int m = (seconds_ - h * 3600) / 60;
    int s = seconds_ - h * 3600 - m * 60;

    boost::gregorian::date d = boost::gregorian::date(
            boost::gregorian::gregorian_calendar::from_julian_day_number(day_));

    boost::posix_time::time_duration t = boost::posix_time::time_duration(h,
            m,
            s,
            microseconds_);

    return boost::posix_time::ptime(d, t);
}

// ___________________________________________________________________________//

std::string JulianDay::ToString(const std::string& format) const
{
    std::string s;
    boost::posix_time::ptime pt = ToPtime();
    boost::posix_time::time_facet* f =
            new boost::posix_time::time_facet(format.c_str());
    std::stringstream ss;

    ss.imbue(std::locale(ss.getloc(), f));
    ss << pt;
    // do not call "delete f"; f is a smart pointer !
    return ss.str();
}

}
