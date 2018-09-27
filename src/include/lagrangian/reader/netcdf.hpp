/*
    This file is part of lagrangian library.

    lagrangian is free software: you can redistribute it and/or modify
    it under the terms of GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lagrangian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of GNU Lesser General Public License
    along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// ___________________________________________________________________________//

#include "lagrangian/axis.hpp"
#include "lagrangian/datetime.hpp"
#include "lagrangian/netcdf.hpp"
#include "lagrangian/reader.hpp"

// ___________________________________________________________________________//

namespace lagrangian {
namespace reader {

/**
 * @brief Grid NetCDF CF reader.
 *
 * The grid must contain at least one variable and two vectors defining the
 * axes of the longitudes and latitudes of the variable. For example :
 *
 * @code
 * dimensions:
 *   y = 915 ;
 *   x = 1080 ;
 * variables:
 *    double y(y) ;
 *        y:long_name = "Latitudes" ;
 *        y:units = "degrees_north" ;
 *    double x(x) ;
 *        x:long_name = "Longitudes" ;
 *        x:units = "degrees_east" ;
 *    float u(x, y) ;
 *        u:_FillValue = 999f ;
 *        u:long_name = "U" ;
 *        u:units = "cm/s" ;
 *        u:date = "2012-01-01 00:00:00.000000 UTC" ;
 *     float v(y, x) ;
 *        v:_FillValue = 999f ;
 *        v:long_name = "U" ;
 *        v:units = "cm/s" ;
 *        V:date = "2012-01-01 00:00:00.000000 UTC" ;
 * @endcode
 *
 * @note: The variable to be read must set an attribute named "date" that
 * define the date of data contained in the variable.
 */
class Netcdf : public Reader {
 private:
  using GetIndex = size_t (Netcdf::*)(const double ix, const double iy) const;

  Axis axis_x_;
  Axis axis_y_;

  lagrangian::Netcdf netcdf_;

  std::vector<double> data_;

  GetIndex pGetIndex_{nullptr};

  // Search for a variable in the NetCDF file
  netcdf::Variable FindVariable(const std::string& name) const {
    netcdf::Variable variable = netcdf_.FindVariable(name);

    if (variable == netcdf::Variable::MISSING) {
      throw std::logic_error(name + ": no such variable");
    }

    return variable;
  }

  // Get the index of the cell of a grid [Y, X]
  inline size_t GetIndexXY(const double ix, const double iy) const {
    return ix * axis_y_.GetNumElements() + iy;
  }

  // Get the index of the cell of a grid [X, Y]
  inline size_t GetIndexYX(const double ix, const double iy) const {
    return iy * axis_x_.GetNumElements() + ix;
  }

  // Get the value of the cell [ix, iy] of the grid
  inline double GetValue(const int ix, const int iy,
                         const double fill_value = 0) const {
    double result = data_[(this->*pGetIndex_)(ix, iy)];
    return std::isnan(result) ? fill_value : result;
  }

 public:
  /**
   * @brief Constructor
   */
  Netcdf() : pGetIndex_(nullptr) {}

  /**
   * Move constructor
   *
   * @param rhs right value
   */
  Netcdf(Netcdf&& rhs) = default;

  /**
   * Move assignment operator
   *
   * @param rhs right value
   */
  Netcdf& operator=(Netcdf&& rhs) = default;

  /**
   * @brief Opens a NetCDF grid in read-only.
   *
   * @param filename Path to the NetCDF grid
   *
   * @throw std::logic_error If the function can not find the definition of
   * longitudes or latitudes in the file.
   */
  void Open(const std::string& filename) override;

  /**
   * @brief Load into memory grid data
   *
   * @param varname name of the NetCDF grid who contains grid data
   * @param unit Unit of data loaded into memory. If the parameter is
   * undefined or contains an empty string, the object will not do unit
   * conversion(i.e. the unit of the data loaded into memory is the unit
   * defined in the NetCDF file)
   */
  void Load(const std::string& varname, const std::string& unit = "") override;

  /**
   * @brief Computes the value of the grid point requested by bilinear
   * interpolation
   *
   * @param longitude Longitude in degrees
   * @param latitude Latitude in degrees
   * @param fill_value Value to be taken into account for fill values
   * @param cell Cell properties of the grid used for the interpolation.
   *
   * @return Interpolated value or std::numeric_limits<double>::quiet_NaN() if
   * point is outside the grid.
   */
  double Interpolate(
      double longitude, double latitude, double fill_value = 0,
      CellProperties& cell = CellProperties::NONE()) const override;

  /**
   * @brief Returns the date of the grid.
   *
   * @param name The variable name containing the date
   *
   * @return the date
   */
  DateTime GetDateTime(const std::string& name) const override;
};

}  // namespace reader
}  // namespace lagrangian
