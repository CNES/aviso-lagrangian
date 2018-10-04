# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See GNU Lesser General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License along
# with lagrangian. If not, see <http://www.gnu.org/licenses/>.
cimport boost.date_time.gregorian


cdef extern from "boost/date_time/posix_time/posix_time.hpp" nogil:
    cdef cppclass time_duration "boost::posix_time::time_duration":
        time_duration()
        time_duration(int, int, int, int) except+
        time_duration operator+(time_duration)
        time_duration operator*(long)
        time_duration invert_sign()
        long hours()
        long total_seconds()
        long ticks_per_second()
        long fractional_seconds()

    cdef cppclass ptime "boost::posix_time::ptime":
        ptime()
        ptime(boost.date_time.gregorian.gregorian_date, time_duration)

    cdef time_duration microseconds "boost::posix_time::microseconds"(long)
    cdef time_duration seconds "boost::posix_time::seconds"(long)
    cdef time_duration hours "boost::posix_time::hours"(long)
