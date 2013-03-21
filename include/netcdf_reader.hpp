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

#ifndef NETCDF_READER_HPP_
#define NETCDF_READER_HPP_

#include "axis.hpp"
#include "reader.h"
#include "julian_day.hpp"
#include "netcdf.hpp"

namespace lagrangian
{
namespace reader
{

class Netcdf: public Reader
{
private:
    typedef size_t
    (Netcdf::*GetIndex)(const double ix, const double iy) const;

    Axis axis_x_;
    Axis axis_y_;

    lagrangian::Netcdf netcdf_;

    std::vector<double> data_;

    GetIndex pGetIndex_;

    netcdf::Variable FindVariable(const std::string& name) const
    {
        netcdf::Variable variable = netcdf_.FindVariable(name);

        if (variable == netcdf::Variable::MISSING)
            throw std::logic_error(name + ": no such variable");

        return variable;
    }

    inline size_t GetIndexXY(const double ix, const double iy) const
    {
        return ix * axis_y_.GetNumElements() + iy;
    }

    inline size_t GetIndexYX(const double ix, const double iy) const
    {
        return iy * axis_x_.GetNumElements() + ix;
    }

    inline double GetValue(const int ix, const int iy) const
    {
        double result = data_[(this->*pGetIndex_)(ix, iy)];
        return boost::math::isnan<double>(result) ? 0 : result;
    }

public:

    /**
     * @brief Constructor
     */
    Netcdf(): pGetIndex_(NULL)
    {
    }

    /**
     * @brief Opens a NetCDF grid in read-only.
     *
     * @param filename Path to the NetCDF grid
     *
     * @thow std::logic_error If the function can not find the definition of
     * longitudes or latitudes in the file.
     */
    void Open(const std::string& filename);

    /**
     * @brief Load into memory grid data
     *
     * @param varname name of the NetCDF grid who contains grid data
     * @param unit Unit of data loaded into memory. If the parameter is
     * undefined or contains an empty string, the object will not do unit
     * conversion(i.e. the unit of the data loaded into memory is the unit
     * defined in the NetCDF file)
     */
    void Load(const std::string& varname, const std::string& unit = "");

    /**
     * @brief Computes the value of the grid point requested by bilinear
     * interpolation
     *
     * @param x coordinate
     * @param y coordinate
     * @param coordinates Coordinates will be calculated if the parameter
     * coordinates is equal to Coordinates::UNDEF() otherwise the coordinates
     * defined by the parameter Coordinates::UNDEF() will be used to
     * interpolate the value.
     *
     * @return Interpolated value or std::numeric_limits<double>::quiet_NaN() if
     * point is outside the grid.
     */
    double Interpolate(double& longitude,
            const double latitude,
            Coordinates& coordinates=Coordinates::UNDEF()) const;

    /**
     * @brief Returns the date of the grid expressed in JulianDay.
     *
     * @param name The variable name containing the date
     *
     * @return the date
     */
    JulianDay GetJulianDay(const std::string& name) const;
    /**
     * @brief Returns the longitude coordinate
     */
    inline Axis& axis_x()
    {
        return axis_x_;
    }

    /**
     * @brief Returns the latitude coordinate
     */
    inline Axis& axis_y()
    {
        return axis_y_;
    }
};

} // namespace::reader
} // namespace::lagrangian

#endif /* NETCDF_READER_HPP_ */
