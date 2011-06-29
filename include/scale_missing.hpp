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

#ifndef SCALE_MISSING_H_
#define SCALE_MISSING_H_

// ___________________________________________________________________________//
#include <boost/math/special_functions.hpp>
#include <cmath>
#include <limits>

// ___________________________________________________________________________//

#include "group.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

/**
 * @brief A variable decorator that handles missing data, and scale/offset
 * packed data.
 *
 * Specifically, this handles:
 *  - packed data using @c scale_factor and @c add_offset
 *  - invalid/missing data using @c valid_min, @c valid_max, @c valid_range,
 *   @c missing_value or @c _FillValue
 *
 * if those "standard attributes" are present.
 *
 */

class ScaleMissing
{
private:
    bool has_scale_offset_;
    bool has_valid_range_;
    bool has_valid_min_;
    bool has_valid_max_;
    bool has_fill_value_;
    bool has_missing_value_;
    double valid_min_;
    double valid_max_;
    double scale_;
    double offset_;
    double fill_value_;
    double missing_value_;

public:
    /**
     * @brief Default constructor
     */
    ScaleMissing();

    /**
     * @brief Create a new instance from an existing Group.
     *
     * @param group an existing Group
     */
    ScaleMissing(const Group& group);

    /**
     * @brief The variable defined a valid data range
     *
     * @return true if variable has valid_range, valid_min or valid_max
     * attributes
     */
    bool HasInvalidData() const
    {
        return has_valid_range_ || has_valid_min_ || has_valid_max_;
    }

    /**
     * @brief The Variable defined missing data values
     *
     * @return true if Variable has missing data values
     */
    bool HasMissing() const
    {
        return HasInvalidData() || has_fill_value_;
    }

    /**
     * @brief Minimum value in the valid range
     *
     * @return the minimum value in the valid range
     */
    double get_valid_min() const
    {
        return valid_min_;
    }

    /**
     * @brief Maximum value in the valid range
     *
     * @return the maximum value in the valid range
     */
    double get_valid_max() const
    {
        return valid_max_;
    }

    /**
     * @brief Value it's outside the valid range
     *
     * @param value Value to test
     * @return true if value it's outside the valid range
     */
    bool IsInvalidData(const double value) const
    {
        if (HasInvalidData())
            return (value < valid_min_) || (value > valid_max_);
        else if (has_valid_min_)
            return value < valid_min_;
        else if (has_valid_max_)
            return value > valid_max_;
        return false;
    }

    /**
     * @brief Determines if the variable has the attribute _FillValue
     *
     * @return true if Variable has _FillValue attribute
     */
    bool has_fill_value() const
    {
        return has_fill_value_;
    }

    /**
     * @brief Determines if value equals the _FillValue
     *
     * @param value Value to test
     * @return true if value equals the _FillValue
     */
    bool IsFillValue(const double value) const
    {
        return has_fill_value_ && (value == fill_value_);
    }

    /**
     * @brief Determines if value equals the missing_value
     *
     * @param value Value to test
     * @return true if value equals the missing_value
     */
    bool IsMissingValue(const double value) const
    {
        return has_missing_value_ && (value == missing_value_);
    }

    /**
     * @brief Determines if value is valid
     *
     * @param value value to test
     *
     * @return true if value is missing
     */
    bool IsMissing(const double value) const
    {

        if (boost::math::isnan(value))
            return true;
        if (IsMissingValue(value))
            return true;
        if (IsFillValue(value))
            return true;
        return false;
    }

    /**
     * @brief Convert Data with scale and offset. Missing data are set to nan
     *
     * @param array data to convert
     */
    void ConvertScaleOffset(std::vector<double>& array) const
    {
        if (!has_scale_offset_)
            return;

        std::vector<double>::iterator it = array.begin();
        for (; it != array.end(); ++it)
            *it = !IsMissing(*it)
                    ? (*it) * scale_ + offset_
                    : std::numeric_limits<double>::quiet_NaN();
    }

    /**
     * @brief Convert all values in the given array that are considered as
     * "missing" to nan
     *
     * @param array data to convert
     *
     */
    void SetMissingToNan(std::vector<double>& array) const
    {
        if (!HasMissing())
            return;

        std::vector<double>::iterator it = array.begin();
        for (; it != array.end(); ++it)
            if (IsMissing(*it))
                *it = std::numeric_limits<double>::quiet_NaN();
    }
};

} // namespace netcdf
} // namespace lagrangian

#endif /* SCALE_MISSING_H_ */
