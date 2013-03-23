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

#include "axis.hpp"
#include "julian_day.hpp"

// ___________________________________________________________________________//


namespace lagrangian
{

/**
 * @brief Coordinates to use for spatial interpolation
 */
class Coordinates
{
private:
    int ix0_, ix1_, iy0_, iy1_;
public:

    /**
     * @brief Default constructor
     *
     * @param ix0
     * @param ix1
     * @param iy0
     * @param iy1
     */
    Coordinates(const int ix0, const int ix1, const int iy0, const int iy1)
            : ix0_(ix0), ix1_(ix1), iy0_(iy0), iy1_(iy1)
    {
    }

    /**
     * @brief Represent an undefined coordinates
     *
     * @return an undefined coordinates
     */
    static Coordinates& UNDEF()
    {
        static Coordinates coordinates(std::numeric_limits<int>::max(),
                0,
                0,
                0);
        return coordinates;
    }

    /**
     * @brief Checks if the current instance is defined
     *
     * @return true if the current coordinates are defined
     */
    inline bool Undef()
    {
        return ix0_ == std::numeric_limits<int>::max();
    }

    inline int ix0() const
    {
        return ix0_;
    }

    inline int ix1() const
    {
        return ix1_;
    }

    inline int iy0() const
    {
        return iy0_;
    }

    inline int iy1() const
    {
        return iy1_;
    }
};

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
     * @param longitude in degrees . In output, the longitude is normalized
     * relative to the definition of its axis.
     * @param latitude in degrees
     * @param coordinates Coordinates will be calculated if the parameter
     * coordinates is equal to Coordinates::UNDEF() otherwise the coordinates
     * defined by the parameter will be used to interpolate the value.
     *
     * @return Interpolated velocity or std::numeric_limits<double>::quiet_NaN()
     * if point is outside the grid.
     */
    virtual double Interpolate(double& longitude,
            const double latitude,
            Coordinates& coordinates=Coordinates::UNDEF()) const = 0;

    /**
     * @brief Returns the date of the grid expressed in JulianDay.
     *
     * @param name The variable name containing the date
     *
     * @return the date
     */
    virtual JulianDay GetJulianDay(const std::string& name) const = 0;

    /**
     * @brief Default method invoked when a reader is destroyed.
     */
    virtual ~Reader()
    {
    }

    /**
     * @brief Returns the longitude coordinate
     */
    virtual Axis& axis_x() = 0;

    /**
     * @brief Returns the latitude coordinate
     */
    virtual Axis& axis_y() = 0;
};

}
