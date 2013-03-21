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

#ifndef READER_WRAPPER_H_
#define READER_WRAPPER_H_

// ___________________________________________________________________________//

#include <boost/python.hpp>

// ___________________________________________________________________________//

#include "reader.h"
#include "axis.hpp"
#include "netcdf_reader.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

// ___________________________________________________________________________//

struct Coordinates: public lagrangian::Coordinates
{
    Coordinates(const int ix0, const int ix1, const int iy0, const int iy1):
        lagrangian::Coordinates(ix0, ix1, iy0, iy1)
    {
    }

    Coordinates(lagrangian::Coordinates& arg):
        lagrangian::Coordinates(arg)
    {
    }

    static Coordinates UNDEF()
    {
        static Coordinates result(lagrangian::Coordinates::UNDEF());
        return result;
    }
};

// ___________________________________________________________________________//

struct Reader: lagrangian::Reader, bp::wrapper<lagrangian::Reader>
{

    Reader();

    virtual lagrangian::JulianDay GetJulianDay(std::string const &name) const;

    virtual double Interpolate(double& longitude,
            double const latitude,
            lagrangian::Coordinates& coordinates=lagrangian::Coordinates::UNDEF()) const;

   
    virtual lagrangian::Axis& axis_x();

    virtual lagrangian::Axis& axis_y();

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

    bp::tuple WrapperInterpolate(double longitude,
            double const latitude,
            Coordinates coordinates=Coordinates::UNDEF()) const;

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

#endif /* READER_WRAPPER_HPP_ */
