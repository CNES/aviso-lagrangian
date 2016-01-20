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

#pragma once

// ___________________________________________________________________________//

#include <boost/smart_ptr.hpp>
#include <list>
#include <netcdf>

// ___________________________________________________________________________//

#include "lagrangian/netcdf/group.hpp"
#include "lagrangian/netcdf/variable.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * @brief Manages NetCDF files
 */
class Netcdf: public netcdf::Group
{
private:
    boost::shared_ptr<netCDF::NcFile> ncfile_;
    std::list<netcdf::Variable> variables_;

public:

    /**
     * @brief Default constructor
     */
    Netcdf()
    {
    }

    /**
     * @brief Default method invoked when a NetcdfReader is destroyed.
     */
    virtual ~Netcdf()
    {
    }

    /**
     * @brief Creates a new instance and opens an existing NetCDF file (read-
     * only)
     */
    Netcdf(const std::string& filename)
    {
        Open(filename);
    }

    /**
     * @brief Open an existing NetCDF file (read-only)
     *
     * @throw std::runtime_error if filename is not a valid NetCDF file
     */
    void Open(const std::string& filename);

    /**
     * @brief Get all of the variables in the files.
     * @return List of variable
     */
    inline std::list<netcdf::Variable> get_variables() const
    {
        return variables_;
    }

    /**
     * @brief Find the Variable with the specified (short) name in this file.
     *
     * @param name short name of Variable
     *
     * @return the Variable, or Variable::MISSING if not found
     */
    netcdf::Variable const& FindVariable(const std::string& name) const
    {
        std::list<netcdf::Variable>::const_iterator it;

        for (it = variables_.begin(); it != variables_.end(); ++it)
        {
            if ((*it).get_name() == name)
                return *it;
        }
        return netcdf::Variable::MISSING;
    }
};

} // namespace lagrangian
