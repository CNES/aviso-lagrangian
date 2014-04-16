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

// ___________________________________________________________________________//

#include <stdexcept>
#include "netcdf.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

void Netcdf::Open(const std::string& filename)
{
    ncfile_ = boost::shared_ptr<netCDF::NcFile>(new netCDF::NcFile(filename,
            netCDF::NcFile::read));

    std::multimap<std::string, netCDF::NcDim> dims = ncfile_->getDims();

    for (std::multimap<std::string, netCDF::NcDim>::iterator it = dims.begin();
            it != dims.end(); ++it)
    {
        netcdf::Dimension dim(it->second.getName(),
                it->second.getSize(),
                it->second.isUnlimited());

        dimensions_.push_back(dim);
    }

    std::multimap<std::string, netCDF::NcVar> vars = ncfile_->getVars();

    for (std::multimap<std::string, netCDF::NcVar>::iterator it = vars.begin();
            it != vars.end(); ++it)
    {
        variables_.push_back(netcdf::Variable(it->second));
    }
}

}
