// This file is part of lagrangian library.
//
// lagrangian is free software: you can redistribute it and/or modify
// it under the terms of GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// lagrangian is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of GNU Lesser General Public License
// along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
#pragma once

// ___________________________________________________________________________//

#include <string>
#include <vector>

// ___________________________________________________________________________//

#include "lagrangian/netcdf/scale_missing.hpp"

// ___________________________________________________________________________//

namespace lagrangian::netcdf {

/**
 * @brief A Variable is a logical container for data. It has a set of
 * Dimensions that define its array shape, and optionally a set of
 * Attributes.
 */
class Variable : public Group {
 public:
  /**
   * @brief Get the name of this variable.
   *
   * @return variable name
   */
  [[nodiscard]] inline auto get_name() const -> std::string { return name_; }

  /**
   * @brief Get the shape: length of Variable in each dimension.
   * @return array whose length is the rank of this variable.
   */
  [[nodiscard]] inline auto get_shape() const -> std::vector<size_t> {
    return shape_;
  }

  /**
   * @brief Get the size of the ith dimension
   *
   * @param index which dimension
   *
   * @return size of the ith dimension
   */
  [[nodiscard]] inline auto get_shape(const int index) const -> size_t {
    return shape_[index];
  }

  /**
   * @brief Get the total number of elements in the Variable.
   *
   * @return total number of elements in the Variable.
   */
  [[nodiscard]] inline auto GetSize() const -> long {
    long result = 1;

    for (auto& item : shape_) {
      result *= item;
    }

    return result;
  }

  /**
   * @brief Get the number of dimensions of the Variable.
   *
   * @return the rank
   */
  [[nodiscard]] inline auto GetRank() const -> size_t { return shape_.size(); }

  /**
   * @brief Whether this is a scalar Variable (rank == 0).
   *
   * @return true if Variable has rank 0
   */
  [[nodiscard]] inline auto IsScalar() const -> bool { return GetRank() == 0; }

  /**
   * @brief Get the description of the Variable.
   * Default is to use "long_name" attribute value. If not exist, look
   * for "description", "title", or "standard_name" attribute value (in
   * that order).
   * @param description found
   *
   * @return true if a description exists, otherwise false
   */
  auto GetDescription(std::string& description) const -> bool;

  /**
   * @brief Get the unit string for the variable
   * Looks for the "units" attribute value
   *
   * @param units unit found
   *
   * @return true if units attribute exists otherwise false
   */
  auto GetUnitsString(std::string& units) const -> bool;

  /**
   * @brief Create a variable.
   *
   * @param var an instance of NcVar object
   */
  explicit Variable(const netCDF::NcVar& var);

  /**
   * @brief Calculate if this is a classic coordinate variable: has same name
   * as its first dimension
   */
  [[nodiscard]] inline auto IsCoordinateVariable() const -> bool {
    if (GetRank() == 1) {
      Dimension first_dimension = GetDimension(0);
      if (name_ == first_dimension.get_name()) {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief compare two variables instances
   *
   * @param a A variable to compare
   * @param b An other variable to compare
   *
   * @return if variables are equals
   */
  inline friend auto operator==(Variable const& a, Variable const& b) -> bool {
    return a.name_ == b.name_ && a.shape_ == b.shape_;
  }

  /**
   * @brief compare two variables instances
   *
   * @param a A variable to compare
   * @param b An other variable to compare
   *
   * @return if variables are different
   */
  inline friend auto operator!=(Variable const& a, Variable const& b) -> bool {
    return a.name_ != b.name_ || a.shape_ != b.shape_;
  }

  /**
   * @brief Read all the data for this variable.
   *
   * @param data read
   */
  void Read(std::vector<double>& data) const;

  /**
   * @brief Read all the data for this variable and convert data to the
   * requested unit.
   *
   * @param data read and converted
   * @param unit used to convert data
   */
  void Read(std::vector<double>& data, const std::string& to) const;

  /**
   * @brief Represents a missing variable.
   */
  static Variable const MISSING;

 private:
  std::string name_;
  std::vector<size_t> shape_;

  netCDF::NcVar ncvar_;

  ScaleMissing scale_missing_;

  /**
   * @brief Default constructor
   */
  Variable() : name_("") {}
};

}  // namespace lagrangian::netcdf
