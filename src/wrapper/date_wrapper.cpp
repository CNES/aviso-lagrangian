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

// ___________________________________________________________________________//

#include "date_wrapper.h"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

static long GetMicroseconds(boost::posix_time::time_duration const& d)
{
    static long ticks = boost::posix_time::time_duration::ticks_per_second();
    long fractional = d.fractional_seconds();

    return ticks > 1000000 ? fractional / (ticks / 1000000) : fractional
            * (1000000 / ticks);
}

// ___________________________________________________________________________//

/* Convert ptime to/from python */
PyObject* PtimeToPythonDatetime::convert(boost::posix_time::ptime const& pt)
{
    boost::gregorian::date date = pt.date();
    boost::posix_time::time_duration time = pt.time_of_day();
    return PyDateTime_FromDateAndTime(static_cast<int> (date.year()),
            static_cast<int> (date.month()),
            static_cast<int> (date.day()),
            time.hours(),
            time.minutes(),
            time.seconds(),
            GetMicroseconds(time));
}

// ___________________________________________________________________________//

PtimeFromPythonDatetime::PtimeFromPythonDatetime()
{
    bp::converter::registry::push_back(&convertible,
            &construct,
            bp::type_id<boost::posix_time::ptime>());
}

void* PtimeFromPythonDatetime::convertible(PyObject * obj_ptr)
{
    return PyDateTime_Check(obj_ptr) ? obj_ptr : NULL;
}

void PtimeFromPythonDatetime::construct(PyObject* obj_ptr,
        bp::converter::rvalue_from_python_stage1_data * data)
{
    PyDateTime_DateTime const* pydate =
            reinterpret_cast<PyDateTime_DateTime*> (obj_ptr);

    // Create date object
    boost::gregorian::date date(PyDateTime_GET_YEAR(pydate),
            PyDateTime_GET_MONTH(pydate),
            PyDateTime_GET_DAY(pydate));

    // Create time duration object
    boost::posix_time::time_duration duration(PyDateTime_DATE_GET_HOUR(pydate),
            PyDateTime_DATE_GET_MINUTE(pydate),
            PyDateTime_DATE_GET_SECOND(pydate),
            0);

   // Set the usecs value
   duration += boost::posix_time::microseconds(
           PyDateTime_DATE_GET_MICROSECOND(pydate));

    // Create posix time object
    void* storage = ((bp::converter::rvalue_from_python_storage<
            boost::posix_time::ptime>*) data)->storage.bytes;
    new (storage) boost::posix_time::ptime(date, duration);
    data->convertible = storage;
}

// ___________________________________________________________________________//

PyObject* TimeDurationToPython::convert(
        boost::posix_time::time_duration const& d)
{
    int days = d.hours() / 24;

    if (days < 0)
        days--;

    return PyDelta_FromDSU(days,
            d.total_seconds() - days * (24 * 3600),
            GetMicroseconds(d));
}

// ___________________________________________________________________________//

TimeDurationFromPython::TimeDurationFromPython()
{
    bp::converter::registry::push_back(&convertible,
            &construct,
            bp::type_id<boost::posix_time::time_duration>());
}

void* TimeDurationFromPython::convertible(PyObject * obj_ptr)
{
    return PyDelta_Check(obj_ptr) ? obj_ptr : NULL;
}

void TimeDurationFromPython::construct(PyObject* obj_ptr,
        bp::converter::rvalue_from_python_stage1_data * data)
{
    PyDateTime_Delta const* pydelta =
            reinterpret_cast<PyDateTime_Delta*> (obj_ptr);

    int days = pydelta->days;
    bool is_negative = days < 0;
    if (is_negative)
        days--;

    // Create time duration object
    boost::posix_time::time_duration duration = (boost::posix_time::hours(24)
            * days + boost::posix_time::seconds(pydelta->seconds)
            + boost::posix_time::microseconds(pydelta->microseconds));

    if (is_negative)
        duration = duration.invert_sign();

    void* storage = ((bp::converter::rvalue_from_python_storage<
            boost::posix_time::time_duration>*) data)->storage.bytes;
    new (storage) boost::posix_time::time_duration(duration);
    data->convertible = storage;
}

// ___________________________________________________________________________//

JulianDay::JulianDay(lagrangian::JulianDay const & arg)
    : lagrangian::JulianDay(arg), bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(boost::posix_time::ptime const & t)
    : lagrangian::JulianDay(boost::ref(t)), bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(std::string const & s)
    : lagrangian::JulianDay(s), bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(int const day, int const seconds, int const microseconds)
    : lagrangian::JulianDay(day, seconds, microseconds),
      bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(double const jd)
    : lagrangian::JulianDay(static_cast<long double>(jd)),
      bp::wrapper<lagrangian::JulianDay>()
{
}

// ___________________________________________________________________________//

ModifiedJulianDay::ModifiedJulianDay(lagrangian::ModifiedJulianDay const & arg)
    : lagrangian::ModifiedJulianDay(arg),
      bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay(boost::posix_time::ptime const & t)
    : lagrangian::ModifiedJulianDay(boost::ref(t)),
      bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay(std::string const & s)
    : lagrangian::ModifiedJulianDay(s),
      bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay(
    int const day,
    int const seconds,
    int const microseconds)
    : lagrangian::ModifiedJulianDay(day, seconds, microseconds),
      bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay(double const jd)
    : lagrangian::ModifiedJulianDay(jd),
      bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

// ___________________________________________________________________________//

CNESJulianDay::CNESJulianDay(lagrangian::CNESJulianDay const & arg)
    : lagrangian::CNESJulianDay(arg),
      bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay(boost::posix_time::ptime const & t)
    : lagrangian::CNESJulianDay(boost::ref(t)),
      bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay(std::string const & s)
    : lagrangian::CNESJulianDay(s),
      bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay(
    int const day,
    int const seconds,
    int const microseconds)
    : lagrangian::CNESJulianDay(day, seconds, microseconds),
      bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay(double const jd)
    : lagrangian::CNESJulianDay(jd), bp::wrapper<lagrangian::CNESJulianDay>()
{
}

int CNESJulianDay::GetModifiedJulianDay() const
{
  return lagrangian::CNESJulianDay::GetModifiedJulianDay();
}

// ___________________________________________________________________________//

LOPJulianDay::LOPJulianDay(lagrangian::LOPJulianDay const & arg)
    : lagrangian::LOPJulianDay(arg),
      bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay(boost::posix_time::ptime const & t)
    : lagrangian::LOPJulianDay(boost::ref(t)),
      bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay(std::string const & s)
    : lagrangian::LOPJulianDay(s),
      bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay(
    int const day,
    int const seconds,
    int const microseconds)
    : lagrangian::LOPJulianDay(day, seconds, microseconds),
      bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay(double const jd)
    : lagrangian::LOPJulianDay(jd), bp::wrapper<lagrangian::LOPJulianDay>()
{
}

int LOPJulianDay::GetModifiedJulianDay() const
{
  return lagrangian::LOPJulianDay::GetModifiedJulianDay();
}

// ___________________________________________________________________________//

NASAJulianDay::NASAJulianDay(lagrangian::NASAJulianDay const & arg)
    : lagrangian::NASAJulianDay(arg),
      bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay(boost::posix_time::ptime const & t)
    : lagrangian::NASAJulianDay(boost::ref(t)),
      bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay(std::string const & s)
    : lagrangian::NASAJulianDay(s),
      bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay(
    int const day,
    int const seconds,
    int const microseconds)
    : lagrangian::NASAJulianDay(day, seconds, microseconds),
      bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay(double const jd)
    : lagrangian::NASAJulianDay(jd), bp::wrapper<lagrangian::NASAJulianDay>()
{
}

int NASAJulianDay::GetModifiedJulianDay() const
{
  return lagrangian::NASAJulianDay::GetModifiedJulianDay();
}

// ___________________________________________________________________________//

CCSDSJulianDay::CCSDSJulianDay(lagrangian::CCSDSJulianDay const & arg)
    : lagrangian::CCSDSJulianDay(arg),
      bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay(boost::posix_time::ptime const & t)
    : lagrangian::CCSDSJulianDay(boost::ref(t)),
      bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay(std::string const & s)
    : lagrangian::CCSDSJulianDay(s),
      bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay(
    int const day,
    int const seconds,
    int const microseconds)
    : lagrangian::CCSDSJulianDay(day, seconds, microseconds),
      bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay(double const jd)
    : lagrangian::CCSDSJulianDay(jd), bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

int CCSDSJulianDay::GetModifiedJulianDay() const
{
  return lagrangian::CCSDSJulianDay::GetModifiedJulianDay();
}

// ___________________________________________________________________________//

void DatePythonModule()
{
  PyDateTime_IMPORT;

  //
  // lagrangian::JulianDay
  //
  {
    bp::class_<JulianDay> JulianDayExposer = bp::class_<JulianDay>(
            "JulianDay",
            bp::init<bp::optional<boost::posix_time::ptime const &> >((
                bp::arg("datetime")=
                    boost::posix_time::microsec_clock::universal_time())));

    // Constructors
    JulianDayExposer.def(
            bp::init<std::string const &>((
                    bp::arg("string"))));
    JulianDayExposer.def(
            bp::init<int,int,int >((
                    bp::arg("day"),
                    bp::arg("seconds")=0,
                    bp::arg("microseconds")=0)));
    JulianDayExposer.def(bp::init<double>((
            bp::arg("day"))));

    { //lagrangian::JulianDay::FromUnixTime
        JulianDayExposer.def(
            "FromUnixTime",
            (long double (*)( long double const ))
                ( &lagrangian::JulianDay::FromUnixTime ),
            (bp::arg("time")));
    }
    { //lagrangian::JulianDay::ToPtime
        JulianDayExposer.def(
            "ToPtime",
            (boost::posix_time::ptime (lagrangian::JulianDay::*)() const)
                (&lagrangian::JulianDay::ToPtime));
    }
    { //lagrangian::JulianDay::ToString
        JulianDayExposer.def(
            "ToString",
            (std::string (lagrangian::JulianDay::*)
                (std::string const &) const)
                    (&lagrangian::JulianDay::ToString),
            (bp::arg("format")));
    }
    { //lagrangian::JulianDay::ToUnixTime

        JulianDayExposer.def(
            "ToUnixTime",
            (long double (lagrangian::JulianDay::*)() const)
                (&lagrangian::JulianDay::ToUnixTime));
    }
    { //lagrangian::JulianDay::get_day
        JulianDayExposer.def(
            "get_day",
            (int (lagrangian::JulianDay::*)() const)
                (&lagrangian::JulianDay::get_day));
    }
    { //lagrangian::JulianDay::get_seconds
        JulianDayExposer.def(
            "get_seconds",
            (int (lagrangian::JulianDay::*)() const)
                (&lagrangian::JulianDay::get_seconds));
    }
    { //lagrangian::JulianDay::get_microseconds
        JulianDayExposer.def(
            "get_microseconds",
            (int (lagrangian::JulianDay::*)() const)
                (&lagrangian::JulianDay::get_microseconds));
    }

    JulianDayExposer.staticmethod("FromUnixTime");

    // Operators
    JulianDayExposer.def("__float__",
            &lagrangian::JulianDay::operator double );

    JulianDayExposer.def(bp::self != bp::self);
    JulianDayExposer.def(bp::self += bp::self);
    JulianDayExposer.def(bp::self -= bp::self);
    JulianDayExposer.def(bp::self < bp::self);
    JulianDayExposer.def(bp::self <= bp::self);

    { //lagrangian::JulianDay::operator=
        JulianDayExposer.def(
            "assign",
            (lagrangian::JulianDay& (lagrangian::JulianDay::*)
                (lagrangian::JulianDay const &))
                    ( &lagrangian::JulianDay::operator= ),
            (bp::arg("other")),
            bp::return_self<>());
    }
    JulianDayExposer.def( bp::self == bp::self );
    JulianDayExposer.def( bp::self > bp::self );
    JulianDayExposer.def( bp::self >= bp::self );
    JulianDayExposer.def( bp::self + bp::self );
    JulianDayExposer.def( bp::self - bp::self );
    JulianDayExposer.def( bp::self_ns::str( bp::self ) );
  }
  bp::implicitly_convertible< lagrangian::JulianDay, double >();

  //
  // lagrangian::ModifiedJulianDay
  //
  {
    bp::class_<ModifiedJulianDay,
            bp::bases<lagrangian::JulianDay> >
                ModifiedJulianDayExposer = bp::class_<ModifiedJulianDay,
                    bp::bases<lagrangian::JulianDay> >(
            "ModifiedJulianDay",
            bp::init<bp::optional<boost::posix_time::ptime const &> >((
                bp::arg("datetime")=
                    boost::posix_time::microsec_clock::universal_time())));
    ModifiedJulianDayExposer.def(bp::init<std::string const &>((
            bp::arg("string"))));
    ModifiedJulianDayExposer.def(bp::init<int,
                                 bp::optional<int, int> >((
            bp::arg("day"),
            bp::arg("seconds") = 0,
            bp::arg("microseconds") = 0)));
    ModifiedJulianDayExposer.def(bp::init<double>((
            bp::arg("day"))));
    { //lagrangian::ModifiedJulianDay::FromUnixTime
        ModifiedJulianDayExposer.def(
            "FromUnixTime",
            (long double (*)( long double const ))
                ( &lagrangian::ModifiedJulianDay::FromUnixTime ),
            (bp::arg("time")));
    }
    { //lagrangian::ModifiedJulianDay::GetModifiedJulianDay
        ModifiedJulianDayExposer.def(
            "GetModifiedJulianDay",
            (int (lagrangian::ModifiedJulianDay::*)() const)
                (&lagrangian::ModifiedJulianDay::GetModifiedJulianDay));
    }
    { //lagrangian::ModifiedJulianDay::Gap
        ModifiedJulianDayExposer.def(
            "Gap",
            (lagrangian::JulianDay (*)( void ))
                ( &lagrangian::ModifiedJulianDay::Gap ));
    }
    ModifiedJulianDayExposer.staticmethod("FromUnixTime");
    ModifiedJulianDayExposer.staticmethod("Gap");
    ModifiedJulianDayExposer.def("__float__",
            &lagrangian::ModifiedJulianDay::operator double);
    ModifiedJulianDayExposer.def(bp::self_ns::str(bp::self));
  }

  //
  // lagrangian::CNESJulianDay
  //
  {
    bp::class_<CNESJulianDay,
            bp::bases<lagrangian::JulianDay> >
                CNESJulianDayExposer = bp::class_<CNESJulianDay,
                    bp::bases<lagrangian::JulianDay> >(
            "CNESJulianDay",
            bp::init<bp::optional<boost::posix_time::ptime const &> >((
                bp::arg("datetime")=
                    boost::posix_time::microsec_clock::universal_time())));
    CNESJulianDayExposer.def(bp::init<std::string const &>((
            bp::arg("string"))));
    CNESJulianDayExposer.def(bp::init<int,
                                 bp::optional<int, int> >((
            bp::arg("day"),
            bp::arg("seconds") = 0,
            bp::arg("microseconds") = 0)));
    CNESJulianDayExposer.def(bp::init<double>((
            bp::arg("day"))));
    { //lagrangian::CNESJulianDay::FromUnixTime
        CNESJulianDayExposer.def(
            "FromUnixTime",
            (long double (*)( long double const ))
                ( &lagrangian::CNESJulianDay::FromUnixTime ),
            (bp::arg("time")));
    }
    { //lagrangian::CNESJulianDay::GetCNESJulianDay
        CNESJulianDayExposer.def(
            "GetModifiedJulianDay",
            (int (lagrangian::CNESJulianDay::*)() const)
                (&lagrangian::CNESJulianDay::GetModifiedJulianDay));
    }
    { //lagrangian::CNESJulianDay::Gap
        CNESJulianDayExposer.def(
            "Gap",
            (lagrangian::JulianDay (*)( void ))
                ( &lagrangian::CNESJulianDay::Gap ));
    }
    CNESJulianDayExposer.staticmethod("FromUnixTime");
    CNESJulianDayExposer.staticmethod("Gap");
    CNESJulianDayExposer.def("__float__",
            &lagrangian::CNESJulianDay::operator double);
    CNESJulianDayExposer.def(bp::self_ns::str(bp::self));
  }

  //
  // lagrangian::LOPJulianDay
  //
  {
    bp::class_<LOPJulianDay,
            bp::bases<lagrangian::JulianDay> >
                LOPJulianDayExposer = bp::class_<LOPJulianDay,
                    bp::bases<lagrangian::JulianDay> >(
            "LOPJulianDay",
            bp::init<bp::optional<boost::posix_time::ptime const &> >((
                bp::arg("datetime")=
                    boost::posix_time::microsec_clock::universal_time())));
    LOPJulianDayExposer.def(bp::init<std::string const &>((
            bp::arg("string"))));
    LOPJulianDayExposer.def(bp::init<int,
                                 bp::optional<int, int> >((
            bp::arg("day"),
            bp::arg("seconds") = 0,
            bp::arg("microseconds") = 0)));
    LOPJulianDayExposer.def(bp::init<double>((
            bp::arg("day"))));
    { //lagrangian::LOPJulianDay::FromUnixTime
        LOPJulianDayExposer.def(
            "FromUnixTime",
            (long double (*)( long double const ))
                ( &lagrangian::LOPJulianDay::FromUnixTime ),
            (bp::arg("time")));
    }
    { //lagrangian::LOPJulianDay::GetLOPJulianDay
        LOPJulianDayExposer.def(
            "GetModifiedJulianDay",
            (int (lagrangian::LOPJulianDay::*)() const)
                (&lagrangian::LOPJulianDay::GetModifiedJulianDay));
    }
    { //lagrangian::LOPJulianDay::Gap
        LOPJulianDayExposer.def(
            "Gap",
            (lagrangian::JulianDay (*)( void ))
                ( &lagrangian::LOPJulianDay::Gap ));
    }
    LOPJulianDayExposer.staticmethod("FromUnixTime");
    LOPJulianDayExposer.staticmethod("Gap");
    LOPJulianDayExposer.def("__float__",
            &lagrangian::LOPJulianDay::operator double);
    LOPJulianDayExposer.def(bp::self_ns::str(bp::self));
  }

  //
  // lagrangian::NASAJulianDay
  //
  {
    bp::class_<NASAJulianDay,
            bp::bases<lagrangian::JulianDay> >
                NASAJulianDayExposer = bp::class_<NASAJulianDay,
                    bp::bases<lagrangian::JulianDay> >(
            "NASAJulianDay",
            bp::init<bp::optional<boost::posix_time::ptime const &> >((
                bp::arg("datetime")=
                    boost::posix_time::microsec_clock::universal_time())));
    NASAJulianDayExposer.def(bp::init<std::string const &>((
            bp::arg("string"))));
    NASAJulianDayExposer.def(bp::init<int,
                                 bp::optional<int, int> >((
            bp::arg("day"),
            bp::arg("seconds") = 0,
            bp::arg("microseconds") = 0)));
    NASAJulianDayExposer.def(bp::init<double>((
            bp::arg("day"))));
    { //lagrangian::NASAJulianDay::FromUnixTime
        NASAJulianDayExposer.def(
            "FromUnixTime",
            (long double (*)( long double const ))
                ( &lagrangian::NASAJulianDay::FromUnixTime ),
            (bp::arg("time")));
    }
    { //lagrangian::NASAJulianDay::GetNASAJulianDay
        NASAJulianDayExposer.def(
            "GetModifiedJulianDay",
            (int (lagrangian::NASAJulianDay::*)() const)
                (&lagrangian::NASAJulianDay::GetModifiedJulianDay));
    }
    { //lagrangian::NASAJulianDay::Gap
        NASAJulianDayExposer.def(
            "Gap",
            (lagrangian::JulianDay (*)( void ))
                ( &lagrangian::NASAJulianDay::Gap ));
    }
    NASAJulianDayExposer.staticmethod("FromUnixTime");
    NASAJulianDayExposer.staticmethod("Gap");
    NASAJulianDayExposer.def("__float__",
            &lagrangian::NASAJulianDay::operator double);
    NASAJulianDayExposer.def(bp::self_ns::str(bp::self));
  }


  //
  // lagrangian::CCSDSJulianDay
  //
  {
    bp::class_<CCSDSJulianDay,
            bp::bases<lagrangian::JulianDay> >
                CCSDSJulianDayExposer = bp::class_<CCSDSJulianDay,
                    bp::bases<lagrangian::JulianDay> >(
            "CCSDSJulianDay",
            bp::init<bp::optional<boost::posix_time::ptime const &> >((
                bp::arg("datetime")=
                    boost::posix_time::microsec_clock::universal_time())));
    CCSDSJulianDayExposer.def(bp::init<std::string const &>((
            bp::arg("string"))));
    CCSDSJulianDayExposer.def(bp::init<int,
                                 bp::optional<int, int> >((
            bp::arg("day"),
            bp::arg("seconds") = 0,
            bp::arg("microseconds") = 0)));
    CCSDSJulianDayExposer.def(bp::init<double>((
            bp::arg("day"))));
    { //lagrangian::CCSDSJulianDay::FromUnixTime
        CCSDSJulianDayExposer.def(
            "FromUnixTime",
            (long double (*)( long double const ))
                ( &lagrangian::CCSDSJulianDay::FromUnixTime ),
            (bp::arg("time")));
    }
    { //lagrangian::CCSDSJulianDay::GetCCSDSJulianDay
        CCSDSJulianDayExposer.def(
            "GetModifiedJulianDay",
            (int (lagrangian::CCSDSJulianDay::*)() const)
                (&lagrangian::CCSDSJulianDay::GetModifiedJulianDay));
    }
    { //lagrangian::CCSDSJulianDay::Gap
        CCSDSJulianDayExposer.def(
            "Gap",
            (lagrangian::JulianDay (*)( void ))
                ( &lagrangian::CCSDSJulianDay::Gap ));
    }
    CCSDSJulianDayExposer.staticmethod("FromUnixTime");
    CCSDSJulianDayExposer.staticmethod("Gap");
    CCSDSJulianDayExposer.def("__float__",
          &lagrangian::CCSDSJulianDay::operator double);
    CCSDSJulianDayExposer.def(bp::self_ns::str(bp::self));
  }
}

}
