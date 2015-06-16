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
        lagrangian::Field* field) :
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
        lagrangian::Field* field) :
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

bool FiniteLyapunovExponents::WrapperCompute(
        const lagrangian::Iterator& it,
        lagrangian::Position* const position) const
{
    lagrangian::CellProperties cell;

    return lagrangian::FiniteLyapunovExponents::Compute(it, position, cell);
}

// ___________________________________________________________________________//

Position::Position() :
    lagrangian::Position( ), bp::wrapper< lagrangian::Position >()
{
}

void Position::StrainTensor(double& a00,
        double& a01,
        double& a10,
        double& a11) const
{
    bp::override function = this->get_override("StrainTensor");
    function(a00, a01, a10, a11);
}

// ___________________________________________________________________________//

Triplet::Triplet(lagrangian::Triplet const & arg ) :
    lagrangian::Triplet( arg ), bp::wrapper< lagrangian::Triplet >()
{
}

Triplet::Triplet() :
    lagrangian::Triplet(), bp::wrapper<lagrangian::Triplet>()
{
}

Triplet::Triplet(double const x, double const y, double const delta) :
    lagrangian::Triplet(x, y, delta), bp::wrapper< lagrangian::Triplet >()
{
}

void Triplet::StrainTensor(double& a00,
        double& a01,
        double& a10,
        double& a11) const
{
    if ( bp::override function = this->get_override( "StrainTensor" ) )
        function(a00, a01, a10, a11);
    else
        this->lagrangian::Triplet::StrainTensor(a00, a01, a10, a11);
}

void Triplet::default_StrainTensor(double& a00,
    double& a01,
    double& a10,
    double& a11 ) const
{
    lagrangian::Triplet::StrainTensor(a00, a01, a10, a11);
}

// ___________________________________________________________________________//

Quintuplet::Quintuplet(lagrangian::Quintuplet const & arg ) :
    lagrangian::Quintuplet( arg ), bp::wrapper< lagrangian::Quintuplet >()
{
}

Quintuplet::Quintuplet() :
    lagrangian::Quintuplet(), bp::wrapper<lagrangian::Quintuplet>()
{
}

Quintuplet::Quintuplet(double const x, double const y, double const delta) :
    lagrangian::Quintuplet(x, y, delta),
        bp::wrapper< lagrangian::Quintuplet >()
{
}

void Quintuplet::StrainTensor(double& a00,
        double& a01,
        double& a10,
        double& a11) const
{
    if ( bp::override function = this->get_override( "StrainTensor" ) )
        function(a00, a01, a10, a11);
    else
        this->lagrangian::Quintuplet::StrainTensor(a00, a01, a10, a11);
}

void Quintuplet::default_StrainTensor(double& a00,
    double& a01,
    double& a10,
    double& a11 ) const
{
    lagrangian::Quintuplet::StrainTensor(a00, a01, a10, a11);
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
                     lagrangian::Field*>((
                             bp::arg("start_time"),
                             bp::arg("end_time"),
                             bp::arg("delta_t"),
                             bp::arg("field"))))
        .def(
            "GetIterator",
            (lagrangian::Iterator (lagrangian::Integration::*)() const)
                (&lagrangian::Integration::GetIterator))
        .def(
            "Fetch",
            (void (lagrangian::Integration::*)
                (double const) const)
                    (&lagrangian::Integration::Fetch),
            (bp::arg("t")));

    //
    // lagrangian::Path
    //
    bp::class_<Path, bp::bases<lagrangian::Integration> >(
            "Path",
            bp::init<lagrangian::JulianDay const &,
                     lagrangian::JulianDay const &,
                     boost::posix_time::time_duration const &,
                     lagrangian::Field*>((
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
    // lagrangian::Position
    //
    bp::class_< Position, boost::noncopyable >(
            "Position",
            bp::init< >())
        .def( 
            "Compute",
            (bool (lagrangian::Position::*)
                (lagrangian::RungeKutta const &,
                 lagrangian::Iterator const & it,
                 lagrangian::CellProperties&))
                    (&lagrangian::Position::Compute),
            (bp::arg("rk"),
             bp::arg("it"),
             bp::arg("cell")))
        .def("IsMissing",
            (bool (lagrangian::Position::*)())
                (&lagrangian::Position::IsMissing))
        .def("MaxDistance",
            (double (lagrangian::Position::* )() const)
                (&lagrangian::Position::MaxDistance))    
        .def("Missing",
            (void (lagrangian::Position::*)())
                (&lagrangian::Position::Missing))    
        .def("StrainTensor",
            bp::pure_virtual(
                (void (lagrangian::Position::*)
                    (double &,
                     double &,
                     double &,
                     double & ) const)
                        (&lagrangian::Position::StrainTensor)),
            (bp::arg("a00"),
             bp::arg("a01"),
             bp::arg("a10"),
             bp::arg("a11")))    
        .def( "get_completed",
            (bool (lagrangian::Position::* )() const)
                (&lagrangian::Position::get_completed))    
        .def("get_time",
            (double (lagrangian::Position::*)() const)
                (&lagrangian::Position::get_time))    
        .def("get_xi",
            (double (lagrangian::Position::*)
                (size_t const) const)
                    (&lagrangian::Position::get_xi),
            (bp::arg("idx")))    
        .def("get_yi",
            (double (lagrangian::Position::*)
                (size_t const) const)
                    (&lagrangian::Position::get_yi),
            (bp::arg("idx")))    
        .def("set_completed",
            (void (lagrangian::Position::* )())
                (&lagrangian::Position::set_completed));
    //
    // lagrangian::Triplet
    //
    bp::class_< Triplet, bp::bases< lagrangian::Position > >(
            "Triplet",
            bp::init< >())    
        .def(bp::init<double, double, double>
            ((bp::arg("x"),
              bp::arg("y"),
              bp::arg("delta"))))    
        .def("StrainTensor",
            (void (lagrangian::Triplet::*)
                (double &,
                 double &,
                 double &,
                 double &) const)
                    (&lagrangian::Triplet::StrainTensor),
            (void (Triplet::*)
                (double &,
                 double &,
                 double &,
                 double & ) const)
                    (&Triplet::default_StrainTensor),
                (bp::arg("a00"),
                 bp::arg("a01"),
                 bp::arg("a10"),
                 bp::arg("a11")));
    //
    // lagrangian::Quintuplet
    //
    bp::class_< Quintuplet, bp::bases< lagrangian::Position > >(
            "Quintuplet",
            bp::init< >())    
        .def(bp::init<double, double, double>
            ((bp::arg("x"),
              bp::arg("y"),
              bp::arg("delta"))))    
        .def("StrainTensor",
            (void (lagrangian::Quintuplet::*)
                (double &,
                 double &,
                 double &,
                 double &) const)
                    (&lagrangian::Quintuplet::StrainTensor),
            (void (Quintuplet::*)
                (double &,
                 double &,
                 double &,
                 double & ) const)
                    (&Quintuplet::default_StrainTensor),
                (bp::arg("a00"),
                 bp::arg("a01"),
                 bp::arg("a10"),
                 bp::arg("a11")));    
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
                     lagrangian::Field*>((
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
                (lagrangian::Iterator const &,
                 lagrangian::Position* const) const)
                    (&FiniteLyapunovExponents::WrapperCompute),
                                 (bp::arg("it"), bp::arg("position")))
        .def(
            "Exponents",
            (void (lagrangian::FiniteLyapunovExponents::*)
                (const lagrangian::Position* const))
                     (&lagrangian::FiniteLyapunovExponents::Exponents),
            (bp::arg("position")))
        .def(
            "Separation",
            (bool (lagrangian::FiniteLyapunovExponents::*)
                (const lagrangian::Position* const) const)
                    (&lagrangian::FiniteLyapunovExponents::Separation),
            ( bp::arg("p")))
        // .def(
        //     "SetInitialPoint",
        //     (lagrangian::Position* (lagrangian::FiniteLyapunovExponents::*)
        //         (double const,
        //          double const,
        //          const lagrangian::FiniteLyapunovExponents::Stencil) const)
        //             (&lagrangian::FiniteLyapunovExponents::SetInitialPoint),
        //     (bp::arg("x"),
        //      bp::arg("y"),
        //      bp::arg("stencil")))
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
