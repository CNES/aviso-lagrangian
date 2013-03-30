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
 * @brief Cell properties of the grid used for the interpolation.
 */
class CellProperties
{
private:
    double x0_, x1_, y0_, y1_;
    int ix0_, ix1_, iy0_, iy1_;

public:

    /**
     * @brief Default constructor
     */
    CellProperties() :
            x0_(std::numeric_limits<double>::max()),
            x1_(std::numeric_limits<double>::min()),
            y0_(), y1_(), ix0_(), ix1_(), iy0_(), iy1_()
    {
    }

    /**
     * @brief Test if the coordinate is in the cell
     *
     * @param x Longitude
     * @param y Latitude
     *
     * @return True if the coordinate is in the cell.
     */
    inline bool Contains(const double x, const double y) const
    {
        return x >= x0_ && x <= x1_ && y >= y0_ && y <= y1_;
    }

    static CellProperties& NONE()
    {
        static CellProperties result;
        return result;
    }

    /**
     * @brief Update the cell properties
     *
     * @param x0 First longitude of the cell in degrees
     * @param x1 Last longitude of the cell in degress
     * @param y0 First latitude of the cell in degrees
     * @param y1 Last latitude of the cell in degrees
     * @param ix0 Index of the first longitude in the grid
     * @param ix1 Index of the last longitude in the grid
     * @param iy0 Index of the first latitude in the grid
     * @param iy1 Index of the last latitude in the grid
     */
    inline void Update(const double x0,
            const double x1,
            const double y0,
            const double y1,
            const int ix0,
            const int ix1,
            const int iy0,
            const int iy1)
    {
        x0_ = x0;
        x1_ = x1;
        y0_ = y0;
        y1_ = y1;
        ix0_ = ix0;
        ix1_ = ix1;
        iy0_ = iy0;
        iy1_ = iy1;
    }

    /**
     * @brief Get the first longitude of the cell
     *
     * @return The first longitude
     */
    inline double x0() const
    {
        return x0_;
    }

    /**
     * @brief Get the last longitude of the cell
     *
     * @return The last longitude of the cell
     */
    inline double x1() const
    {
        return x1_;
    }

    /**
     * @brief Get the first latitude of the cell
     *
     * @return The first latitude
     */
    inline double y0() const
    {
        return y0_;
    }

    /**
     * @brief Get the last latitude of the cell
     *
     * @return The last longitude
     */
    inline double y1() const
    {
        return y1_;
    }

    /**
     * @brief Get the index of the first longitude in the grid
     *
     * @return The index of the first longitude
     */
    inline int ix0() const
    {
        return ix0_;
    }

    /**
     * @brief Get the index of the last longitude in the grid
     *
     * @return The index of the last longitude
     */
    inline int ix1() const
    {
        return ix1_;
    }

    /**
     * @brief Get the index of the first latitude in the grid
     *
     * @return The index of the first latitude
     */
    inline int iy0() const
    {
        return iy0_;
    }

    /**
     * @brief Get the index of the last latitude in the grid
     *
     * @return The index of the first latitude
     */
    inline double iy1() const
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
     * @param cell Cell properties of the grid used for the interpolation.
     *
     * @return Interpolated velocity or std::numeric_limits<double>::quiet_NaN()
     * if point is outside the grid.
     */
    virtual double Interpolate(const double longitude,
            const double latitude,
            CellProperties& cell=CellProperties::NONE()) const = 0;

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
};

}
