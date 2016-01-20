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

#include <cfloat>

// ___________________________________________________________________________//

#include "lagrangian/netcdf/scale_missing.hpp"
#include "lagrangian/netcdf/cf.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

ScaleMissing::ScaleMissing()
{
    scale_ = 1;
    offset_ = 0;
    has_scale_offset_ = false;

    valid_min_ = DBL_MIN;
    valid_max_ = DBL_MAX;
    has_valid_range_ = false;
    has_valid_min_ = false;
    has_valid_max_ = false;

    has_fill_value_ = false;
    fill_value_ = DBL_MAX;

    has_missing_value_ = false;
    missing_value_ = DBL_MAX;
}

// ___________________________________________________________________________//

ScaleMissing::ScaleMissing(const Group& group)
{
    valid_min_ = DBL_MIN;
    valid_max_ = DBL_MAX;

    Attribute attribute = group.FindAttribute(CF::SCALE_FACTOR);
    scale_ = attribute != Attribute::MISSING ? attribute.get_value() : 1;

    attribute = group.FindAttribute(CF::ADD_OFFSET);
    offset_ = attribute != Attribute::MISSING ? attribute.get_value() : 0;

    has_scale_offset_ = scale_ != 1 || offset_ != 0;

    attribute = group.FindAttribute(CF::VALID_RANGE);
    has_valid_range_ = attribute != Attribute::MISSING;
    if (has_valid_range_)
    {
        valid_min_ = attribute.get_value(0);
        valid_max_ = attribute.get_value(1);
    }

    attribute = group.FindAttribute(CF::VALID_MIN);
    has_valid_min_ = attribute != Attribute::MISSING;
    if (has_valid_min_)
    {
        valid_min_ = attribute.get_value();
    }

    attribute = group.FindAttribute(CF::VALID_MAX);
    has_valid_max_ = attribute != Attribute::MISSING;
    if (has_valid_max_)
    {
        valid_max_ = attribute.get_value();
    }

    attribute = group.FindAttribute(CF::FILL_VALUE);
    has_fill_value_ = attribute != Attribute::MISSING;
    if (has_fill_value_)
    {
        fill_value_ = attribute.get_value();
    }

    attribute = group.FindAttribute(CF::MISSING_VALUE);
    has_missing_value_ = attribute != Attribute::MISSING;
    if (has_missing_value_)
    {
        missing_value_ = attribute.get_value();
    }
} // namespace netcdf
} // namespace lagrangian
}
