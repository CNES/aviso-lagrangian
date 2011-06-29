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
    ncfile_ = boost::shared_ptr<NcFile>(new NcFile(filename.c_str(),
            NcFile::ReadOnly));

    if (ncfile_->is_valid() == 0)
        throw std::runtime_error(filename + " is not a valid netCDF file");

    for (int ix = 0; ix < ncfile_->num_dims(); ++ix)
    {
        NcDim* ncdim = ncfile_->get_dim(ix);
        netcdf::Dimension dim(ncdim->name(),
                ncdim->size(),
                ncdim->is_unlimited() == 1);

        dimensions_.push_back(dim);
    }

    for (int ix = 0; ix < ncfile_->num_vars(); ++ix)
    {
        NcVar* ncvar(ncfile_->get_var(ix));
        netcdf::Variable variable(ncvar);

        variables_.push_back(variable);
    }
}

// ___________________________________________________________________________//

void Netcdf::Close()
{
    if (ncfile_->is_valid())
        ncfile_->close();
}

}
