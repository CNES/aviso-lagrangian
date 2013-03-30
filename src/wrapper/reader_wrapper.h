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

#pragma once

// ___________________________________________________________________________//

#include <boost/python.hpp>

// ___________________________________________________________________________//

#include "reader.hpp"
#include "axis.hpp"
#include "netcdf_reader.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

// ___________________________________________________________________________//

struct CellProperties: public lagrangian::CellProperties
{
    CellProperties():
        lagrangian::CellProperties()
    {
    }

    static CellProperties NONE()
    {
        static CellProperties result;
        return result;
    }

};

// ___________________________________________________________________________//

struct Reader: lagrangian::Reader, bp::wrapper<lagrangian::Reader>
{

    Reader();

    virtual lagrangian::JulianDay GetJulianDay(std::string const &name) const;

    virtual double Interpolate(const double longitude,
            double const latitude,
            lagrangian::CellProperties& cell) const;

    virtual void Load(::std::string const & name, ::std::string const & unit);

    virtual void Open(::std::string const & filename);
};

// ___________________________________________________________________________//

struct Netcdf: lagrangian::reader::Netcdf, bp::wrapper<
        lagrangian::reader::Netcdf>
{
    Netcdf(lagrangian::reader::Netcdf const & arg);

    Netcdf();

    virtual lagrangian::JulianDay GetJulianDay(std::string const & name) const;

    lagrangian::JulianDay WrapperGetJulianDay(std::string const & name) const;

    bp::tuple WrapperInterpolate(const double longitude,
            double const latitude,
            lagrangian::CellProperties& cell) const;

    virtual void Load(std::string const & varname,
            std::string const & unit = "");

    void WrapperLoad(std::string const & varname,
            std::string const & unit = "");

    virtual void Open(std::string const & filename);

    void WrapperOpen(std::string const & filename);
};

// ___________________________________________________________________________//

void ReaderPythonModule();

}
