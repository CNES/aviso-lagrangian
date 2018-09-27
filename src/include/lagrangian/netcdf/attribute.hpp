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

#include <netcdf>
#include <stdexcept>
#include <vector>

// ___________________________________________________________________________//

namespace lagrangian {
namespace netcdf {

/**
 * @brief An Attribute has a name and values, used for associating
 * arbitrary metadata with a Variable or a Group.
 *
 * Attributes are immutable.
 *
 */
class Attribute {
 private:
  std::string name_;
  std::string svalue_;
  std::vector<double> value_;
  Attribute() : name_(""), svalue_("") {}

 public:
  /**
   * @brief Constructor
   *
   * @param ncatt An instance of NcAtt
   */
  explicit Attribute(const netCDF::NcAtt& ncatt) : name_(ncatt.getName()) {
    // Attribute is a string ?
    if (ncatt.getType() == netCDF::ncChar ||
        ncatt.getType() == netCDF::ncString) {
      ncatt.getValues(svalue_);
    // Get Attributes values as double
    } else {
      value_.resize(ncatt.getAttLength());
      ncatt.getValues(&value_[0]);
    }
  }

  /**
   * @brief Get the name of this Attribute.
   *
   * @return name
   */
  inline std::string const& get_name() const { return name_; }

  /**
   * @brief True if value is a string.
   *
   * @return if its a string
   */
  inline bool IsString() const { return value_.empty(); }

  /**
   * @brief Retrieve String value; only call if IsString() is true.
   *
   * @return String if this is a String valued attribute
   *
   * @throw std::logic_error if value isn't of type string
   *
   * @see Attribute#IsString
   */
  inline std::string const& get_string() const {
    if (!IsString()) {
      throw std::logic_error("Attribute is numeric");
    }
    return svalue_;
  }

  /**
   * Get an attribute value;
   *
   * @param index which index; only call if IsString() is false.
   *
   * @throw std::logic_error if value is of type string
   *
   * @return ith value.
   */
  inline double get_value(const int index = 0) const {
    if (IsString()) {
      throw std::logic_error("Attribute isn't numeric");
    }
    return value_[index];
  }

  /**
   * @brief Get the length of the array of values
   *
   * @return number of elements for this attribute.
   */
  inline int GetLength() const {
    return IsString() ? svalue_.size() : value_.size();
  }

  /**
   * @brief compare two attributes instances
   *
   * @param a An attribute to compare
   * @param b An other attribute to compare
   *
   * @return if attributes are equals
   */
  inline friend bool operator==(Attribute const& a, Attribute const& b) {
    return a.name_ == b.name_ && a.svalue_ == b.svalue_ && a.value_ == b.value_;
  }

  /**
   * @brief compare two attributes instances
   *
   * @param a An attribute to compare
   * @param b An other attribute to compare
   *
   * @return if attributes are different
   */
  inline friend bool operator!=(Attribute const& a, Attribute const& b) {
    return a.name_ != b.name_ || a.svalue_ != b.svalue_ || a.value_ != b.value_;
  }

  /**
   * @brief Represents a missing attribute
   */
  static Attribute const MISSING;
};

}  // namespace netcdf
}  // namespace lagrangian
