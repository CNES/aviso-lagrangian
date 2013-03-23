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

#include <stdexcept>
#include <string>

// ___________________________________________________________________________//

namespace lagrangian
{

class Field
{
public:
    enum UnitType
    {
        kMetric, kAngular
    };
private:
    UnitType unit_type_;
public:

    Field(const Field::UnitType unit_type): unit_type_(unit_type)
    {
        switch (unit_type_)
        {
        case kMetric:
            break;
        case kAngular:
            break;
        default:
            throw std::invalid_argument("invalid Field::UnitType value");
        }
    }

    virtual ~Field() {}

    /**
     * @brief Loads the data in the interval [t0, t1]
     *
     * @param t0 Begin date (number of seconds elapsed since 1/1/1970)
     * @param t1 End date (number of seconds elapsed since 1/1/1970)
     */
    virtual void Fetch(const double t0, const double t1) {}

    virtual bool Compute(const double t,
            const double x,
            const double y,
            double& u,
            double& v) const = 0;

    /**
     * @brief Unit type used by this field.
     *
     * @return unit type.
     */
    UnitType get_unit_type() const
    {
        return unit_type_;
    }
    /**
     * @brief Unit used by this field.
     *
     * @return unit.
     */
    const std::string GetUnit() const
    {
        switch (unit_type_)
        {
        case kMetric:
            return "m/s";
        case kAngular:
            return "deg/s";
        default:
            throw std::runtime_error("invalid Field::UnitType value");
        }
    }
};

}