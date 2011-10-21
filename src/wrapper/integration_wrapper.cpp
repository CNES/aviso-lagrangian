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

FiniteLyapunovExponents::FiniteLyapunovExponents(
        lagrangian::FiniteLyapunovExponents const & arg) :
    lagrangian::FiniteLyapunovExponents(arg),
            bp::wrapper<lagrangian::FiniteLyapunovExponents>()

{

}

FiniteLyapunovExponents::FiniteLyapunovExponents(
        lagrangian::JulianDay const & start_time,
        lagrangian::JulianDay const & end_time,
        boost::posix_time::time_duration const & delta_t,
        lagrangian::FiniteLyapunovExponents::Mode mode,
        double const min_separation,
        double const delta,
        lagrangian::Field const * field) :
    lagrangian::FiniteLyapunovExponents(start_time,
                end_time,
                delta_t,
                mode,
                min_separation,
                delta,
                field),
            bp::wrapper<lagrangian::FiniteLyapunovExponents>()
{
}

bp::tuple FiniteLyapunovExponents::WrapperCompute(
        const lagrangian::Iterator& it) const
{
    lagrangian::Triplet p;

    bool result = lagrangian::FiniteLyapunovExponents::Compute(it, p);

    return bp::make_tuple(result, p);
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
                     lagrangian::Field const *>((
                             bp::arg("start_time"),
                             bp::arg("end_time"),
                             bp::arg("delta_t"),
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
    //
    // lagrangian::FiniteLyapunovExponents
    //
    bp::enum_< lagrangian::FiniteLyapunovExponents::Mode>("Mode").
            value("kFSLE", lagrangian::FiniteLyapunovExponents::kFSLE).
            value("kFTLE", lagrangian::FiniteLyapunovExponents::kFTLE).
            export_values();

    bp::class_<FiniteLyapunovExponents, bp::bases<lagrangian::Integration> >(
            "FiniteLyapunovExponents",
            bp::init<lagrangian::JulianDay const &,
                     lagrangian::JulianDay const &,
                     boost::posix_time::time_duration const &,
                     lagrangian::FiniteLyapunovExponents::Mode,
                     double const,
                     double const,
                     lagrangian::Field const *>((
                             bp::arg("start_time"),
                             bp::arg("end_time"),
                             bp::arg("delta_t"),
                             bp::arg("mode"),
                             bp::arg("min_separation"),
                             bp::arg("delta"),
                             bp::arg("field"))))
        .def(
            "Compute",
            (bp::tuple (lagrangian::FiniteLyapunovExponents::*)
                (lagrangian::Iterator const &) const)
                    (&FiniteLyapunovExponents::WrapperCompute),
                                 (bp::arg("it")))
        .def(
            "Exponents",
            (void (lagrangian::FiniteLyapunovExponents::*)
                (lagrangian::Iterator const &,
                 lagrangian::Triplet const &))
                     (&lagrangian::FiniteLyapunovExponents::Exponents),
            (bp::arg("it"),
             bp::arg("p")))
        .def(
            "Separation",
            (bool (lagrangian::FiniteLyapunovExponents::*)
                (lagrangian::Triplet const &) const)
                    (&lagrangian::FiniteLyapunovExponents::Separation),
            ( bp::arg("p")))
        .def(
            "SetInitialPoint",
            (lagrangian::Triplet (lagrangian::FiniteLyapunovExponents::*)
                (double const, double const) const)
                    (&lagrangian::FiniteLyapunovExponents::SetInitialPoint),
            (bp::arg("x"),
             bp::arg("y")))
        .def(
             "get_lambda1",
             (double (lagrangian::FiniteLyapunovExponents::*)() const)
                     (&lagrangian::FiniteLyapunovExponents::get_lambda1))
        .def(
            "get_lambda2",
            (double (lagrangian::FiniteLyapunovExponents::*)() const)
                    (&lagrangian::FiniteLyapunovExponents::get_lambda2))
        .def(
             "get_theta1",
             (double (lagrangian::FiniteLyapunovExponents::*)() const)
                     (&lagrangian::FiniteLyapunovExponents::get_theta1))
        .def(
            "get_theta2",
            (double (lagrangian::FiniteLyapunovExponents::*)() const)
                    (&lagrangian::FiniteLyapunovExponents::get_theta2));
}

} // namespace wrapper
