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

#include "integration_wrapper.h"

namespace wrapper
{

Path::Path(lagrangian::Path const & arg) :
    lagrangian::Path(arg), bp::wrapper<lagrangian::Path>()
{
}

Path::Path(lagrangian::JulianDay const & start_time,
        lagrangian::JulianDay const & end_time,
        boost::posix_time::time_duration const & delta_t,
        lagrangian::Field const * field) :
    lagrangian::Path(start_time, end_time, delta_t, field),
            bp::wrapper<lagrangian::Path>()
{
}

bp::tuple Path::WrapperCompute(const lagrangian::Iterator& it,
        const double x0,
        const double y0) const
{
    double x1, y1;

    bool result = lagrangian::Path::Compute(it, x0, y0, x1, y1);

    return bp::make_tuple(result, x1, y1);
}

// ___________________________________________________________________________//

void IntegrationPythonModule()
{
    //
    // lagrangian::Iterator
    //
    bp::class_<lagrangian::Iterator >(
            "Iterator",
            bp::init<double, double, double>((
                    bp::arg("begin"),
                    bp::arg("end"),
                    bp::arg("inc") )) )
        .def(
            "GoAfter",
            (bool (lagrangian::Iterator::*)() const)
                (&lagrangian::Iterator::GoAfter))
        .def(
            "__call__",
            (double (lagrangian::Iterator::*)() const)
                (&lagrangian::Iterator::operator()))
        .def(
            "Next",
            (void (lagrangian::Iterator::*)() const)
                (&lagrangian::Iterator::operator++));
    //
    // lagrangian::Integration
    //
    bp::class_<lagrangian::Integration >(
            "Integration",
            bp::init<lagrangian::JulianDay const &,
                     lagrangian::JulianDay const &,
                     boost::posix_time::time_duration const &,
                     double, lagrangian::Field const *>((
                             bp::arg("start_time"),
                             bp::arg("end_time"),
                             bp::arg("delta_t"),
                             bp::arg("min_separation"),
                             bp::arg("field"))))
        .def(
            "GetIterator",
            (lagrangian::Iterator (lagrangian::Integration::*)() const)
                (&lagrangian::Integration::GetIterator));
    //
    // lagrangian::Path
    //
    bp::class_<Path, bp::bases<lagrangian::Integration> >(
            "Path",
            bp::init<lagrangian::JulianDay const &,
                     lagrangian::JulianDay const &,
                     boost::posix_time::time_duration const &,
                     lagrangian::Field const *>((
                             bp::arg("start_time"),
                             bp::arg("end_time"),
                             bp::arg("delta_t"),
                             bp::arg("field"))))
        .def(
            "Compute",
            (bp::tuple (lagrangian::Path::*)
                (lagrangian::Iterator const &,
                 double const,
                 double const) const)
                    (&Path::WrapperCompute),
            (bp::arg("it"),
             bp::arg("x0"),
             bp::arg("x1")));
}

} // namespace wrapper
