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

#include "reader_wrapper.h"
#include "reader_factory.hpp"

namespace wrapper
{

// ___________________________________________________________________________//

Reader::Reader() :
    lagrangian::Reader(), bp::wrapper<lagrangian::Reader>()
{
}

lagrangian::JulianDay Reader::GetJulianDay(std::string const &name) const
{
    bp::override function = this->get_override("GetJulianDay");
    return function(name);
}

double Reader::Interpolate(double& longitude,
        double const latitude,
        lagrangian::Coordinates& coordinates) const
{
    bp::override function = this->get_override("Interpolate");
    return function(longitude, latitude, boost::ref(coordinates));
}

lagrangian::Axis& Reader::axis_x()
{
    throw std::logic_error("This method could not be overriden in Python");
}

lagrangian::Axis& Reader::axis_y()
{
    throw std::logic_error("This method could not be overriden in Python");
}

void Reader::Load(::std::string const & name, ::std::string const & unit)
{
    bp::override function = this->get_override("Load");
    function(name, unit);
}

void Reader::Open(::std::string const & filename)
{
    bp::override function = this->get_override("Open");
    function(filename);
}

// ___________________________________________________________________________//

Netcdf::Netcdf(lagrangian::reader::Netcdf const & arg) :
    lagrangian::reader::Netcdf(arg), bp::wrapper<lagrangian::reader::Netcdf>()
{
}

Netcdf::Netcdf() :
    lagrangian::reader::Netcdf(), bp::wrapper<lagrangian::reader::Netcdf>()
{
}

lagrangian::JulianDay Netcdf::GetJulianDay(std::string const & name) const
{
    if ( bp::override function = this->get_override( "GetJulianDay" ) )
        return function(name);
    else
        return this->lagrangian::reader::Netcdf::GetJulianDay(name);
}

lagrangian::JulianDay Netcdf::WrapperGetJulianDay(std::string const & name) const
{
    return lagrangian::reader::Netcdf::GetJulianDay(name);
}

bp::tuple Netcdf::WrapperInterpolate(double longitude,
        double const latitude,
        Coordinates coordinates) const
{
    double result = this->lagrangian::reader::Netcdf::Interpolate(longitude,
                latitude, coordinates);
    return bp::make_tuple(result, longitude);
}

void Netcdf::Load(std::string const & varname, std::string const & unit)
{
    if ( bp::override function = this->get_override( "Load" ) )
        function(varname, unit);
    else
        this->lagrangian::reader::Netcdf::Load(varname, unit);
}

void Netcdf::WrapperLoad(std::string const & varname, std::string const & unit)
{
    lagrangian::reader::Netcdf::Load(varname, unit);
}

void Netcdf::Open(std::string const & filename)
{
    if ( bp::override function = this->get_override( "Open" ) )
        function(filename);
    else
        this->lagrangian::reader::Netcdf::Open(filename);
}

void Netcdf::WrapperOpen(std::string const & filename)
{
    lagrangian::reader::Netcdf::Open(filename);
}

// ___________________________________________________________________________//

void ReaderPythonModule()
{
    //
    // lagangian::Reader
    //
    bp::class_<Coordinates>(
            "Coordinates",
            bp::init<int, int, int, int>((
                bp::arg("ix0"),
                bp::arg("ix1"),
                bp::arg("iy0"),
                bp::arg("iy1"))))
        .def(
            "UNDEF",
            (Coordinates (*)())
                (&Coordinates::UNDEF))
        .def(
            "Undef",
            (bool (Coordinates::*)())
                (&Coordinates::Undef))
        .def(
            "ix0",
            (int (Coordinates::*)() const)
                (&Coordinates::ix0))
        .def(
            "ix1",
            (int (Coordinates::*)() const)
                (&Coordinates::ix1))
        .def(
            "iy0",
            (int (Coordinates::*)() const)
                (&Coordinates::iy0))
        .def(
            "iy1",
            (int (Coordinates::*)() const)
                (&Coordinates::iy1))
        .staticmethod( "UNDEF" );

    bp::class_<Reader, boost::noncopyable >("Reader")
        .def(
            "GetJulianDay",
            bp::pure_virtual((lagrangian::JulianDay (lagrangian::Reader::* )
                (std::string const &) const)
                    (&lagrangian::Reader::GetJulianDay)),
            (bp::arg("name")))
        .def(
            "Load",
            bp::pure_virtual((void (lagrangian::Reader::*)
                (std::string const &,
                 std::string const &))
                    (&lagrangian::Reader::Load)),
            (bp::arg("name"),
             bp::arg("unit")))
        .def(
            "Open",
            bp::pure_virtual((void (lagrangian::Reader::*)
                (std::string const &))
                    (&lagrangian::Reader::Open)),
            ( bp::arg("filename")));
    //
    // lagrangian::reader::Netcdf
    //
    bp::class_<Netcdf, bp::bases<lagrangian::Reader> >("NetcdfReader")
        .def(
            "GetJulianDay",
            (lagrangian::JulianDay (lagrangian::reader::Netcdf::* )
                (std::string const &) const)
                    (&lagrangian::reader::Netcdf::GetJulianDay),
            (lagrangian::JulianDay (Netcdf::*)
                (std::string const &) const)
                    (&Netcdf::WrapperGetJulianDay),
            (bp::arg("name")))
        .def(
            "Interpolate",
            (bp::tuple (Netcdf::*)
                (double,
                 double const,
                 Coordinates & ) const)
                     (&Netcdf::WrapperInterpolate),
            (bp::arg("longitude"),
             bp::arg("latitude"),
             bp::arg("coordinates")=Coordinates::UNDEF()))
        .def(
            "Load",
            (void (lagrangian::reader::Netcdf::*)
                (std::string const &,
                 std::string const &))
                     (&lagrangian::reader::Netcdf::Load),
            (void (Netcdf::*)
                (std::string const &,
                 std::string const &))
                     (&Netcdf::WrapperLoad),
            (bp::arg("varname"),
             bp::arg("unit")=""))
        .def(
            "Open",
            (void (lagrangian::reader::Netcdf::*)
                (std::string const &))
                    (&lagrangian::reader::Netcdf::Open),
            (void (Netcdf::*)(std::string const &))
                (&Netcdf::WrapperOpen),
            (bp::arg("filename")));
}

}
