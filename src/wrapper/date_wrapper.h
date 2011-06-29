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

#ifndef DATE_WRAPPER_H_
#define DATE_WRAPPER_H_

// ___________________________________________________________________________//

#include <boost/python.hpp>
#include <datetime.h>

// ___________________________________________________________________________//

#include "julian_day.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

struct PtimeToPythonDatetime
{
    static PyObject* convert(boost::posix_time::ptime const& pt);
};

// ___________________________________________________________________________//

struct PtimeFromPythonDatetime
{
    PtimeFromPythonDatetime();

    static void* convertible(PyObject * obj_ptr);

    static void construct(PyObject* obj_ptr,
            bp::converter::rvalue_from_python_stage1_data * data);
};

// ___________________________________________________________________________//

struct TimeDurationToPython
{
    static PyObject* convert(boost::posix_time::time_duration const& d);
};

// ___________________________________________________________________________//

struct TimeDurationFromPython
{
    TimeDurationFromPython();

    static void* convertible(PyObject * obj_ptr);

    static void construct(PyObject* obj_ptr,
            bp::converter::rvalue_from_python_stage1_data* data);
};

// ___________________________________________________________________________//

struct JulianDay: lagrangian::JulianDay, bp::wrapper<lagrangian::JulianDay>
{
    JulianDay(lagrangian::JulianDay const & arg);

    JulianDay(boost::posix_time::ptime const & t =
            boost::posix_time::microsec_clock::universal_time());

    JulianDay(std::string const & s);

    JulianDay(int const day, int const seconds, int const microseconds);

    JulianDay(double const jd);
};

// ___________________________________________________________________________//

struct ModifiedJulianDay: lagrangian::ModifiedJulianDay, bp::wrapper<
        lagrangian::ModifiedJulianDay>
{

    ModifiedJulianDay(lagrangian::ModifiedJulianDay const & arg);

    ModifiedJulianDay();

    ModifiedJulianDay(std::string const & s);

    ModifiedJulianDay(int const day,
            int const seconds = 0,
            int const microseconds = 0,
            int const gap = 0);

    ModifiedJulianDay(double const day, int const gap = 0);

    virtual int GetModifiedJulianDay() const;

    int DefaultGetModifiedJulianDay() const;
};

// ___________________________________________________________________________//

struct CCSDSJulianDay: lagrangian::CCSDSJulianDay, bp::wrapper<
        lagrangian::CCSDSJulianDay>
{
    CCSDSJulianDay(lagrangian::CCSDSJulianDay const & arg);

    CCSDSJulianDay();

    CCSDSJulianDay(std::string const & s);

    CCSDSJulianDay(double const jd);

    CCSDSJulianDay(int const day,
            int const seconds = 0,
            int const microseconds = 0u);

    virtual int GetModifiedJulianDay() const;

    int DefaultGetModifiedJulianDay() const;
};

// ___________________________________________________________________________//

struct CNESJulianDay: lagrangian::CNESJulianDay, bp::wrapper<
        lagrangian::CNESJulianDay>
{

    CNESJulianDay(lagrangian::CNESJulianDay const & arg);

    CNESJulianDay();

    CNESJulianDay(std::string const & s);

    CNESJulianDay(double const jd);

    CNESJulianDay(int const day,
            int const seconds = 0,
            int const microseconds = 0u);

    virtual int GetModifiedJulianDay() const;

    int DefaultGetModifiedJulianDay() const;
};

// ___________________________________________________________________________//

struct LOPJulianDay: lagrangian::LOPJulianDay, bp::wrapper<
        lagrangian::LOPJulianDay>
{

    LOPJulianDay(lagrangian::LOPJulianDay const & arg);

    LOPJulianDay();

    LOPJulianDay(std::string const & s);

    LOPJulianDay(double const jd);

    LOPJulianDay(int const day,
            int const seconds = 0,
            int const microseconds = 0u);

    virtual int GetModifiedJulianDay() const;

    int DefaultGetModifiedJulianDay() const;
};

// ___________________________________________________________________________//

struct NASAJulianDay: lagrangian::NASAJulianDay, bp::wrapper<
        lagrangian::NASAJulianDay>
{

    NASAJulianDay(lagrangian::NASAJulianDay const & arg);

    NASAJulianDay();

    NASAJulianDay(std::string const & s);

    NASAJulianDay(double const jd);

    NASAJulianDay(int const day,
            int const seconds = 0,
            int const microseconds = 0u);

    virtual int GetModifiedJulianDay() const;

    int DefaultGetModifiedJulianDay() const;
};

void DatePythonModule();

}
#endif /* DATE_WRAPPER_H_ */
