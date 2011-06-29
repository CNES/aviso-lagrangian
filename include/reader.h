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

#ifndef READER_H_
#define READER_H_
// ___________________________________________________________________________//

#include <string>

// ___________________________________________________________________________//

#include "julian_day.hpp"

// ___________________________________________________________________________//


namespace lagrangian
{

/**
 * @brief Abstract class that defines a velocity reader fields.
 */
class Reader
{

public:

    /**
     * @brief Opens a file in read-only.
     *
     * @param filename Path to the grid
     *
     */
    virtual void Open(const std::string& filename) = 0;

    /**
     * @brief Load into memory grid data
     *
     * @param varname name of the grid who contains data
     * @param unit Unit of data loaded into memory.
     */
    virtual void Load(const std::string& name, const std::string& unit) = 0;

    /**
     * @brief Computes the velocity of the grid point requested
     *
     * @param longitude in degrees
     * @param latitude in degrees
     *
     * @return Interpolated velocity or std::numeric_limits<double>::quiet_NaN()
     * if point is outside the grid.
     */
    virtual double
    Interpolate(const double longitude, const double latitude) const = 0;

    /**
     * @brief Returns the date of the grid expressed in JulianDay.
     *
     * @param name The variable name containing the date
     *
     * @return the date
     */
    virtual JulianDay GetJulianDay(const std::string& name) const = 0;

    /**
     * @brief Default method invoked when a NetcdfReader is destroyed.
     */
    virtual ~Reader()
    {
    }
};

}

#endif /* READER_H_ */
