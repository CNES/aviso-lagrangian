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
    