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

#include <stdexcept>

// ___________________________________________________________________________//

#include "lagrangian/netcdf.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

void Netcdf::Open(const std::string& filename)
{
    ncfile_ = std::shared_ptr<netCDF::NcFile>(new netCDF::NcFile(filename,
            netCDF::NcFile::read));

    for (auto& item: ncfile_->getDims())
    {
        netcdf::Dimension dim(item.second.getName(),
                item.second.getSize(),
                item.second.isUnlimited());

        dimensions_.push_back(dim);
    }

    for (auto& item: ncfile_->getVars())
        variables_.push_back(netcdf::Variable(item.second));
}

}
