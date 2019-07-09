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
    along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// ___________________________________________________________________________//

#include <stdexcept>
#include <string>

// ___________________________________________________________________________//

#include "lagrangian/reader.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Abstract class defining a field where it is possible to calculate a
 * speed
 */
class Field {
 public:
  /**
   * @brief Unit field
   */
  enum UnitType {
    kMetric,  //!< The field velocity is expressed in the metric
              //!<	system (eg. m/s)
    kAngular  //!< The field velocity is expressed in the angular
              //!< 	system (eg. deg/s)
  };

  /**
   * @brief Type of coordinates
   */
  enum CoordinatesType {
    kSphericalEquatorial,  //!< kSphericalEquatorial
    kCartesian             //!< kCartesian
  };

 private:
  UnitType unit_type_;
  CoordinatesType coordinates_type_;

 public:
  /**
   * @brief Default constructor
   *
   * @param unit_type Unit field
   * @param coordinate_type Type of the coordinate system.
   *
   * @throw std::invalid_argument if the type of unit or the coordinate system
   * is unknown
   */
  Field(const UnitType unit_type, const CoordinatesType coordinates_type)
      : unit_type_(unit_type), coordinates_type_(coordinates_type) {
    switch (unit_type_) {
      case kMetric:
        break;
      case kAngular:
        break;
      default:
        throw std::invalid_argument("invalid Field::UnitType value");
    }
    switch (coordinates_type_) {
      case kSphericalEquatorial:
        break;
      case kCartesian:
        break;
      default:
        throw std::invalid_argument("invalid Field::CoordinatesType value");
    }
  }

  /**
   * @brief Default method invoked when a Field is destroyed.
   */
  virtual ~Field() = default;

  /**
   * @brief Loads the data in the interval [t0, t1]
   *
   * @param t0 Begin date (number of seconds elapsed since 1/1/1970)
   * @param t1 End date (number of seconds elapsed since 1/1/1970)
   */
  virtual void Fetch(const double t0, const double t1) {}

  /**
   * Calculate the value of the speed to the spatio temporal position
   * requested.
   *
   * @param t Time in number of seconds elapsed since 1970
   * @param x Longitude in degree
   * @param y Latitude in degree
   * @param u Velocity
   * @param v Velocity
   * @param cell Cell properties of the grid used for the interpolation
   *
   * @return true if the value of the speed is set otherwise false.
   */
  virtual bool Compute(
      const double /*t*/, const double /*x*/, const double /*y*/, double& u,
      double& v, CellProperties& /*cell*/ = CellProperties::NONE()) const {
    u = std::numeric_limits<double>::quiet_NaN();
    v = std::numeric_limits<double>::quiet_NaN();
    return false;
  }

  /**
   * @brief Unit type used by this field.
   *
   * @return unit type.
   */
  UnitType get_unit_type() const noexcept { return unit_type_; }

  /**
   * @brief Coordinates type used by this field.
   *
   * @return coordinates type.
   */
  CoordinatesType get_coordinates_type() const noexcept {
    return coordinates_type_;
  }

  /**
   * @brief Unit used by this field.
   *
   * @return unit.
   */
  const std::string GetUnit() const {
    switch (unit_type_) {
      case kMetric:
        return "m/s";
      case kAngular:
        return "degrees/s";
      default:
        throw std::runtime_error("invalid Field::UnitType value");
    }
  }
};

}  // namespace lagrangian
