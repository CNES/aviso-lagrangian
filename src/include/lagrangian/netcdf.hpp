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
#include <memory>
#include <netcdf>

// ___________________________________________________________________________//

#include "lagrangian/netcdf/group.hpp"
#include "lagrangian/netcdf/variable.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Manages NetCDF files
 */
class NetCDF : public netcdf::Group {
 public:
  /**
   * @brief Default constructor
   */
  NetCDF() = default;

  /**
   * @brief Default method invoked when a NetcdfReader is destroyed.
   */
  ~NetCDF() override = default;

  /**
   * @brief Creates a new instance and opens an existing NetCDF file (read-
   * only)
   */
  explicit NetCDF(const std::string &filename) { Open(filename); }

  /**
   * @brief Open an existing NetCDF file (read-only)
   *
   * @throw std::runtime_error if filename is not a valid NetCDF file
   */
  void Open(const std::string &filename);

  /**
   * @brief Get all of the variables in the files.
   * @return List of variable
   */
  [[nodiscard]] inline auto get_variables() const
      -> std::list<netcdf::Variable> {
    return variables_;
  }

  /**
   * @brief Find the Variable with the specified (short) name in this file.
   *
   * @param name short name of Variable
   *
   * @return the Variable, or Variable::MISSING if not found
   */
  [[nodiscard]] auto FindVariable(const std::string &name) const
      -> netcdf::Variable const & {
    for (auto &item : variables_) {
      if (item.get_name() == name) {
        return item;
      }
    }
    return netcdf::Variable::MISSING;
  }

 private:
  std::shared_ptr<netCDF::NcFile> ncfile_;
  std::list<netcdf::Variable> variables_;
};

}  // namespace lagrangian
