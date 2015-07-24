cdef extern from "boost/date_time/gregorian/gregorian.hpp" nogil:
    cdef cppclass gregorian_date "boost::gregorian::date":
        gregorian_date()
        gregorian_date(int, int, int) except+
