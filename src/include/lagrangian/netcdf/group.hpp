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

#include <list>
#include <vector>

// ___________________________________________________________________________//

#include "lagrangian/netcdf/attribute.hpp"
#include "lagrangian/netcdf/dimension.hpp"

// ___________________________________________________________________________//

namespace lagrangian::netcdf {

/**
 * @brief A group is a logical collection of dimensions and attributes
 */
class Group {
 public:
  /**
   * @brief Constructor
   */
  Group() = default;

  /**
   * @brief Default method invoked when a Group is destroyed.
   */
  virtual ~Group() = default;

  /**
   * @brief Get the list of dimensions used in this group.
   *
   * @return Dimension list
   */
  [[nodiscard]] inline auto get_dimensions() const -> std::vector<Dimension> {
    return dimensions_;
  }

  /**
   * @brief Get the ith dimension.
   *
   * @param index of the dimension.
   * @return requested Dimension
   */
  [[nodiscard]] auto GetDimension(const int index) const -> Dimension const& {
    return dimensions_[index];
  }

  /**
   * @brief Find the index of the named Dimension.
   *
   * @param name of the dimension
   *
   * @return the index of the named Dimension, or -1 if not found.
   */
  [[nodiscard]] inline auto FindDimensionIndex(const std::string& name) const
      -> int {
    for (unsigned int ix = 0; ix < dimensions_.size(); ++ix) {
      if (dimensions_[ix].get_name() == name) {
        return ix;
      }
    }
    return -1;
  }

  /**
   * @brief Returns the set of attributes for this variable.
   *
   * @return Attribute list
   */
  [[nodiscard]] inline auto get_attributes() const -> std::list<Attribute> {
    return attributes_;
  }

  /**
   * @brief Find an Attribute by name.
   *
   * @param name of the attribute
   *
   * @return the Attribute or Attribute::MISSING if not found
   */
  [[nodiscard]] auto FindAttribute(const std::string& name) const
      -> Attribute const&;

  /**
   * @brief Find an Attribute by name, ignoring the case
   *
   * @param name of the attribute
   *
   * @return the attribute or Attribute::MISSING if not found
   */
  [[nodiscard]] auto FindAttributeIgnoreCase(const std::string& name) const
      -> Attribute const&;

 protected:
  /// @brief Dimensions known
  std::vector<Dimension> dimensions_;

  /// @brief Attributes in this group
  std::list<Attribute> attributes_;
};

}  // namespace lagrangian::netcdf
