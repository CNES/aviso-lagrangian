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

JulianDay::JulianDay(lagrangian::JulianDay const & arg) :
    lagrangian::JulianDay(arg), bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(boost::posix_time::ptime const & t) :
    lagrangian::JulianDay(boost::ref(t)), bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(std::string const & s) :
    lagrangian::JulianDay(s), bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(int const day, int const seconds, int const microseconds) :
    lagrangian::JulianDay(day, seconds, microseconds),
            bp::wrapper<lagrangian::JulianDay>()
{
}

JulianDay::JulianDay(double const jd) :
    lagrangian::JulianDay(jd), bp::wrapper<lagrangian::JulianDay>()
{
}

// ___________________________________________________________________________//

ModifiedJulianDay::ModifiedJulianDay(lagrangian::ModifiedJulianDay const & arg) :
    lagrangian::ModifiedJulianDay(arg),
            bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay() :
    lagrangian::ModifiedJulianDay(),
            bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay(std::string const & s) :
    lagrangian::ModifiedJulianDay(s),
            bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay(int const day,
        int const seconds,
        int const microseconds,
        int const gap) :
    lagrangian::ModifiedJulianDay(day, seconds, microseconds, gap),
            bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

ModifiedJulianDay::ModifiedJulianDay(double const day, int const gap) :
    lagrangian::ModifiedJulianDay(day, gap),
            bp::wrapper<lagrangian::ModifiedJulianDay>()
{
}

int ModifiedJulianDay::GetModifiedJulianDay() const
{
    if ( bp::override function = this->get_override("GetModifiedJulianDay"))
        return function();
    else
        return this->lagrangian::ModifiedJulianDay::GetModifiedJulianDay();
}

int ModifiedJulianDay::DefaultGetModifiedJulianDay() const
{
    return lagrangian::ModifiedJulianDay::GetModifiedJulianDay();
}

// ___________________________________________________________________________//

CCSDSJulianDay::CCSDSJulianDay(lagrangian::CCSDSJulianDay const & arg) :
    lagrangian::CCSDSJulianDay(arg), bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay() :
    lagrangian::CCSDSJulianDay(), bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay(std::string const & s) :
    lagrangian::CCSDSJulianDay(s), bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay(double const jd) :
    lagrangian::CCSDSJulianDay(jd), bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

CCSDSJulianDay::CCSDSJulianDay(int const day,
        int const seconds,
        int const microseconds) :
    lagrangian::CCSDSJulianDay(day, seconds, microseconds),
            bp::wrapper<lagrangian::CCSDSJulianDay>()
{
}

int CCSDSJulianDay::GetModifiedJulianDay() const
{
    if ( bp::override function = this->get_override("GetModifiedJulianDay"))
        return function();
    else
        return this->lagrangian::CCSDSJulianDay::GetModifiedJulianDay();
}

int CCSDSJulianDay::DefaultGetModifiedJulianDay() const
{
    return lagrangian::CCSDSJulianDay::GetModifiedJulianDay();
}

// ___________________________________________________________________________//

 CNESJulianDay::CNESJulianDay(lagrangian::CNESJulianDay const & arg) :
    lagrangian::CNESJulianDay(arg), bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay() :
    lagrangian::CNESJulianDay(), bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay(std::string const & s) :
    lagrangian::CNESJulianDay(s), bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay(double const jd) :
    lagrangian::CNESJulianDay(jd), bp::wrapper<lagrangian::CNESJulianDay>()
{
}

CNESJulianDay::CNESJulianDay(int const day,
        int const seconds,
        int const microseconds) :
    lagrangian::CNESJulianDay(day, seconds, microseconds),
            bp::wrapper<lagrangian::CNESJulianDay>()
{
}

int CNESJulianDay::GetModifiedJulianDay() const
{
    if ( bp::override function = this->get_override("GetModifiedJulianDay"))
        return function();
    else
        return this->lagrangian::CNESJulianDay::GetModifiedJulianDay();
}

int CNESJulianDay::DefaultGetModifiedJulianDay() const
{
    return lagrangian::CNESJulianDay::GetModifiedJulianDay();
}


// ___________________________________________________________________________//

LOPJulianDay::LOPJulianDay(lagrangian::LOPJulianDay const & arg) :
    lagrangian::LOPJulianDay(arg), bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay() :
    lagrangian::LOPJulianDay(), bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay(std::string const & s) :
    lagrangian::LOPJulianDay(s), bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay(double const jd) :
    lagrangian::LOPJulianDay(jd), bp::wrapper<lagrangian::LOPJulianDay>()
{
}

LOPJulianDay::LOPJulianDay(int const day,
        int const seconds,
        int const microseconds) :
    lagrangian::LOPJulianDay(day, seconds, microseconds),
            bp::wrapper<lagrangian::LOPJulianDay>()
{
}

int LOPJulianDay::GetModifiedJulianDay() const
{
    if ( bp::override function = this->get_override("GetModifiedJulianDay"))
        return function();
    else
        return this->lagrangian::LOPJulianDay::GetModifiedJulianDay();
}

int LOPJulianDay::DefaultGetModifiedJulianDay() const
{
    return lagrangian::LOPJulianDay::GetModifiedJulianDay();
}


// ___________________________________________________________________________//

NASAJulianDay::NASAJulianDay(lagrangian::NASAJulianDay const & arg) :
    lagrangian::NASAJulianDay(arg), bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay() :
    lagrangian::NASAJulianDay(), bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay(::std::string const & s) :
    lagrangian::NASAJulianDay(s), bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay(double const jd) :
    lagrangian::NASAJulianDay(jd), bp::wrapper<lagrangian::NASAJulianDay>()
{
}

NASAJulianDay::NASAJulianDay(int const day,
        int const seconds,
        int const microseconds) :
    lagrangian::NASAJulianDay(day, seconds, microseconds),
            bp::wrapper<lagrangian::NASAJulianDay>()
{
}

int NASAJulianDay::GetModifiedJulianDay() const
{
    if ( bp::override function = this->get_override("GetModifiedJulianDay"))
        return function();
    else
        return this->lagrangian::NASAJulianDay::GetModifiedJulianDay();
}

int NASAJulianDay::DefaultGetModifiedJulianDay() const
{
    return lagrangian::NASAJulianDay::GetModifiedJulianDay();
}

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
                    bp::arg("t")=
                        boost::posix_time::microsec_clock::universal_time())));

        // Constructors
        JulianDayExposer.def(
                bp::init<std::string const &>((
                        bp::arg("s"))));
        JulianDayExposer.def(
                bp::init<int,int,int >((
                        bp::arg("day"),
                        bp::arg("seconds"),
                        bp::arg("microseconds"))));
        JulianDayExposer.def(bp::init<double>((
                bp::arg("jd"))));

        { //lagrangian::JulianDay::JulianFromUnixTime
            JulianDayExposer.def(
                "JulianDayFromUnixTime",
                (double (*)( double const ))
                    ( &::lagrangian::JulianDay::JulianDayFromUnixTime ),
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
                (double (lagrangian::JulianDay::*)() const)
                    (&lagrangian::JulianDay::ToUnixTime));

        }
        { //lagrangian::JulianDay::get_day
            JulianDayExposer.def(
                "get_day",
                (int (lagrangian::JulianDay::*)() const)
                    (&lagrangian::JulianDay::get_day));
        }
        { //lagrangian::JulianDay::get_microseconds
            JulianDayExposer.def(
                "get_microseconds",
                (int (lagrangian::JulianDay::*)() const)
                    (&lagrangian::JulianDay::get_microseconds));
        }
        { //lagrangian::JulianDay::get_seconds
            JulianDayExposer.def(
                "get_seconds",
                (int (lagrangian::JulianDay::*)() const)
                    (&lagrangian::JulianDay::get_seconds));

        }

        JulianDayExposer.staticmethod("JulianDayFromUnixTime");

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

    //
    // lagrangian::ModifiedJulianDay
    //
    {
        bp::class_<ModifiedJulianDay,
                bp::bases<lagrangian::JulianDay> >
                    ModifiedJulianDayExposer = bp::class_<ModifiedJulianDay,
                        bp::bases<lagrangian::JulianDay> >(
                "ModifiedJulianDay",
                bp::init< >());
        ModifiedJulianDayExposer.def(bp::init<std::string const &>((
                bp::arg("s"))));
        ModifiedJulianDayExposer.def(bp::init<int,
                                     bp::optional<int, int, int> >((
                bp::arg("day"),
                bp::arg("seconds") = 0,
                bp::arg("microseconds") = 0,
                bp::arg("gap") = 0)));
        ModifiedJulianDayExposer.def(bp::init<double, bp::optional<int> >((
                bp::arg("day"),
                bp::arg("gap") = 0)));
        { //lagrangian::ModifiedJulianDay::GetModifiedJulianDay
            ModifiedJulianDayExposer.def(
                "GetModifiedJulianDay",
                (int (lagrangian::ModifiedJulianDay::*)() const)
                    (&lagrangian::ModifiedJulianDay::GetModifiedJulianDay),
                (int (ModifiedJulianDay::*)() const)
                    (&ModifiedJulianDay::DefaultGetModifiedJulianDay));
        }
        ModifiedJulianDayExposer.def("__float__",
                &lagrangian::ModifiedJulianDay::operator double);
        ModifiedJulianDayExposer.def(bp::self_ns::str(bp::self));
    }
    //
    // lagrangian::CNESJulianDay
    //
    {
        bp::class_<CNESJulianDay, bp::bases<lagrangian::ModifiedJulianDay> >
                CNESJulianDayExposer = bp::class_<CNESJulianDay, bp::bases<
                        lagrangian::ModifiedJulianDay> >(
                "CNESJulianDay",
                bp::init< >());
        CNESJulianDayExposer.def(bp::init<std::string const &>((
                bp::arg("s"))));
        CNESJulianDayExposer.def(bp::init<int,
                                 bp::optional<int, int> >((
                bp::arg("day"),
                bp::arg("seconds") = 0,
                bp::arg("microseconds") = 0)));
        CNESJulianDayExposer.def(bp::init<double>((
                bp::arg("day"))));
        { //lagrangian::CNESJulianDay::GetJulianDay
            CNESJulianDayExposer.def(
                "GetModifiedJulianDay",
                (int (lagrangian::CNESJulianDay::*)() const)
                    (&lagrangian::CNESJulianDay::GetModifiedJulianDay),
                (int (CNESJulianDay::*)() const)
                    (&CNESJulianDay::DefaultGetModifiedJulianDay));
        }
        CNESJulianDayExposer.def("__float__",
                &lagrangian::CNESJulianDay::operator double);
        CNESJulianDayExposer.def(bp::self_ns::str(bp::self));
    }
    //
    // lagrangian::CCSDSJulianDay
    //
    {
        bp::class_<CCSDSJulianDay,
                bp::bases<lagrangian::ModifiedJulianDay> >
                    CCSDSJulianDayExposer = bp::class_<CCSDSJulianDay,
                        bp::bases<lagrangian::ModifiedJulianDay> >(
                "CCSDSJulianDay",
                bp::init< >());
        CCSDSJulianDayExposer.def(bp::init<std::string const &>((
                bp::arg("s"))));
        CCSDSJulianDayExposer.def(bp::init<int,
                                  bp::optional<int, int> >((
                bp::arg("day"),
                bp::arg("seconds") = 0,
                bp::arg("microseconds") = 0)));
        CCSDSJulianDayExposer.def(bp::init<double>((
                bp::arg("day"))));
        { //lagrangian::CCSDSJulianDay::GetJulianDay
            CCSDSJulianDayExposer.def(
                "GetModifiedJulianDay",
                (int (lagrangian::CCSDSJulianDay::*)() const)
                    (&lagrangian::CCSDSJulianDay::GetModifiedJulianDay),
                (int (CCSDSJulianDay::*)() const)
                    (&CCSDSJulianDay::DefaultGetModifiedJulianDay));
        }
        CCSDSJulianDayExposer.def("__float__",
                &lagrangian::CCSDSJulianDay::operator double);
        CCSDSJulianDayExposer.def(bp::self_ns::str(bp::self));
    }
    //
    // lagrangian::LOPJulianDay
    //
    {
        bp::class_<LOPJulianDay,
                bp::bases<lagrangian::ModifiedJulianDay> >
                    LOPJulianDayExposer = bp::class_<LOPJulianDay,
                        bp::bases<lagrangian::ModifiedJulianDay> >(
                "LOPJulianDay",
                bp::init< >());
        LOPJulianDayExposer.def(bp::init<std::string const &>((
                bp::arg("s"))));
        LOPJulianDayExposer.def(bp::init<int,
                                bp::optional<int, int> >((
                bp::arg("day"),
                bp::arg("seconds") = 0,
                bp::arg("microseconds") = 0)));
        LOPJulianDayExposer.def(bp::init<double>((
                bp::arg("day"))));
        { //lagrangian::LOPJulianDay::GetJulianDay
            LOPJulianDayExposer.def(
                "GetModifiedJulianDay",
                (int (lagrangian::LOPJulianDay::*)() const)
                    (&lagrangian::LOPJulianDay::GetModifiedJulianDay),
                (int (LOPJulianDay::*)() const)
                    (&LOPJulianDay::DefaultGetModifiedJulianDay));
        }
        LOPJulianDayExposer.def("__float__",
                &lagrangian::LOPJulianDay::operator double);
        LOPJulianDayExposer.def(bp::self_ns::str(bp::self));
    }
    //
    // lagrangian::NASAJulianDay
    //
    {
        bp::class_<NASAJulianDay,
                bp::bases<lagrangian::ModifiedJulianDay> >
                    NASAJulianDayExposer = bp::class_<NASAJulianDay,
                        bp::bases<lagrangian::ModifiedJulianDay> >(
                "NASAJulianDay",
                bp::init< >());
        NASAJulianDayExposer.def(bp::init<std::string const &>((
                bp::arg("s"))));
        NASAJulianDayExposer.def(bp::init<int,
                                 bp::optional<int, int> >((
                bp::arg("day"),
                bp::arg("seconds") = 0,
                bp::arg("microseconds") = 0)));
        NASAJulianDayExposer.def(bp::init<double>((
                bp::arg("day"))));
        { //lagrangian::NASAJulianDay::GetJulianDay
            NASAJulianDayExposer.def(
                "GetModifiedJulianDay",
                (int (lagrangian::NASAJulianDay::*)() const)
                    (&lagrangian::NASAJulianDay::GetModifiedJulianDay),
                (int (NASAJulianDay::*)() const)
                    (&NASAJulianDay::DefaultGetModifiedJulianDay));
        }
        NASAJulianDayExposer.def("__float__",
                &lagrangian::NASAJulianDay::operator double);
        NASAJulianDayExposer.def(bp::self_ns::str(bp::self));
    }
}


}
