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

#include <string>

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

/**
 * @brief Constants used in CF conventions.
 */

class CF
{
public:
    /**
     * @brief Represents the number to be added to the data after it is read.
     */
    static const std::string ADD_OFFSET;
    /**
     * @brief Identifies generic spatial coordinates.
     */
    static const std::string AXIS;
    /**
     * @brief Specifies the fill value used to pre-fill disk space allocated to
     * the variable.
     */
    static const std::string FILL_VALUE;
    /**
     * @brief Represents the factor to be applied to the data after it is read.
     */
    static const std::string SCALE_FACTOR;
    /**
     * @brief The name used to identify the physical quantity.
     */
    static const std::string STANDARD_NAME;
    /**
     * @brief Specifies the unit used for the variable's data.
     */
    static const std::string UNITS;
    /**
     * @brief Values indicating missing data
     */
    static const std::string MISSING_VALUE;
    /**
     * @brief Specified the maximum valid value for this variable.
     */
    static const std::string VALID_MAX;
    /**
     * @brief Specified the minimum valid value for this variable.
     */
    static const std::string VALID_MIN;
    /**
     * @brief A vector of two numbers specifying the minimum and maximum valid
     * values for this variable.
     */
    static const std::string VALID_RANGE;
};

} // namespace netcdf
} // namespace lagrangian
