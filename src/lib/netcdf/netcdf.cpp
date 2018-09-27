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

#include <boost/format.hpp>
#include <memory>
#include <stdexcept>

// ___________________________________________________________________________//

#include "lagrangian/netcdf.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

void Netcdf::Open(const std::string& filename) {
  if (filename.empty()) {
    throw std::system_error(ENOENT, std::system_category(), filename);
  }

  try {
    ncfile_ = std::make_shared<netCDF::NcFile>(filename, netCDF::NcFile::read);

    for (auto& item : ncfile_->getDims()) {
      dimensions_.emplace_back(netcdf::Dimension(item.second.getName(),
                                                 item.second.getSize(),
                                                 item.second.isUnlimited()));
    }

    for (auto& item : ncfile_->getVars()) {
      variables_.emplace_back(netcdf::Variable(item.second));
    }
  } catch (netCDF::exceptions::NcException) {
    throw std::runtime_error(
        boost::str(boost::format("Couldn't open `%s' for reading") % filename));
  }
}
}  // namespace lagrangian
