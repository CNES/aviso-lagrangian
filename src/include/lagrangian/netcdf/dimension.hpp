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
#include <utility>

// ___________________________________________________________________________//

namespace lagrangian::netcdf {

/**
 * @brief A %Dimension is used to define the array shape of a Variable.
 */

class Dimension {
 public:
  /**
   * @brief Returns the name of this dimension
   *
   * @return name of dimension
   */
  [[nodiscard]] inline auto get_name() const -> std::string const& { return name_; }

  /**
   * @brief Get the length of the dimension.
   *
   * @return length of the dimension
   */
  [[nodiscard]] inline auto get_length() const -> int { return length_; }

  /**
   * @brief If unlimited, then the length can increase; otherwise it is
   * immutable.
   *
   * @return if its an "unlimited" dimension
   */
  [[nodiscard]] inline auto is_unlimited() const -> bool { return unlimited_; }

  /**
   * @brief Constructor
   *
   * @param name of the dimension, must be unique.
   * @param length of the dimension.
   * @param unlimited whether the length can grow.
   */
  Dimension(std::string name, const int length, const bool unlimited = false)
      : name_(std::move(name)), length_(length), unlimited_(unlimited) {}

  /**
   * @brief compare two dimensions instances
   *
   * @param other Another instance of %Dimension
   * @return if dimensions are equals
   */
  inline auto operator==(Dimension& other) -> bool {
    return name_ == other.get_name() && length_ == other.get_length() &&
           unlimited_ == other.is_unlimited();
  }

 private:
  std::string name_;
  int length_;
  bool unlimited_;
};

}  // namespace lagrangian::netcdf
