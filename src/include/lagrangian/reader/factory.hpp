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

#include "lagrangian/reader.hpp"
#include "lagrangian/reader/netcdf.hpp"

// ___________________________________________________________________________//

namespace lagrangian::reader {

/**
 * @brief %Reader factory
 */
class Factory {
 public:
  /**
   * @brief Type of fields reader known
   */
  enum Type {
    kNetCDF  //!< kNetCDF
  };

  /**
   * @brief Get an instance of a given reader
   * @param type %Reader type
   * @return An instance of a reader
   */
  static auto NewReader(const Type type) -> Reader* {
    switch (type) {
      case kNetCDF:
        return new NetCDF();
    }
    throw std::invalid_argument(
        "invalid lagrangian::reader::Factory::Type value");
  }
};

}  // namespace lagrangian::reader
