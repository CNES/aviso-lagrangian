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

#include <boost/algorithm/string.hpp>

// ___________________________________________________________________________//

#include "lagrangian/axis.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

// _____________________________________________________________________________

static const axis::LatitudeUnit g_axis_latitude_unit;
static const axis::LongitudeUnit g_axis_longitude_unit;

// ___________________________________________________________________________//

void Axis::NormalizeLongitude()
{
    is_ascending_ = points_.size() < 2 ? true : GetCoordinateValue(0)
            < GetCoordinateValue(1);

    if (type_ == kLongitude)
    {
        bool monotonic = true;

        for (unsigned int ix = 1; ix < points_.size(); ++ix)
        {
            monotonic = is_ascending_
                    ? points_[ix - 1] < points_[ix]
                    : points_[ix - 1] > points_[ix];

            if (!monotonic)
                break;
        }

        if (!monotonic)
        {
            bool cross = false;

            for (unsigned int ix = 1; ix < points_.size(); ++ix)
            {
                if (cross)
                    points_[ix] += is_ascending_ ? 360 : -360;
                else
                    cross = is_ascending_
                            ? points_[ix - 1] > points_[ix]
                            : points_[ix - 1] < points_[ix];
            }
        }
    }
}

// ___________________________________________________________________________//

void Axis::MakeEdges()
{
    if (is_regular_)
        return;

    const int n = points_.size();

    edges_.resize(n + 1);

    for (int ix = 1; ix < n; ++ix)
        edges_[ix] = (points_[ix - 1] + points_[ix]) / 2;

    edges_[0] = 2 * points_[0] - edges_[1];
    edges_[n] = 2 * points_[n - 1] - edges_[n - 1];
}

// ___________________________________________________________________________//

int Axis::FindIndexIrregular(const double coordinate, bool bounded) const
{
    int low = 0;
    int mid = 0;
    int high = points_.size();

    if (coordinate < edges_[low])
        return bounded ? 0 : -1;
    if (coordinate > edges_[high])
        return bounded ? high - 1 : -1;

    if (is_ascending_)
    {
        while (high > low + 1)
        {
            mid = (low + high) >> 1;
            register double value = edges_[mid];

            if (value == coordinate)
                return mid;

            value < coordinate ? low = mid : high = mid;
        }

        return low;
    }
    else
    {
        while (high > low + 1)
        {
            mid = (low + high) >> 1;
            register double value = edges_[mid];

            if (value == coordinate)
                return mid;

            value < coordinate ? high = mid : low = mid;
        }

        return high - 1;
    }
}

// ___________________________________________________________________________//

Axis::Axis(const netcdf::Variable& variable) :
    type_(kUnknown), points_(), edges_()
{
    if (variable.IsCoordinateVariable() == false)
        throw std::invalid_argument(variable.get_name() + ": not an axis");

    // Store unit axis
    if (!variable.GetUnitsString(unit_))
        unit_ = "";

    // Determines axis from standard_name
    netcdf::Attribute attribute =
            variable.FindAttributeIgnoreCase(netcdf::CF::STANDARD_NAME);
    if (attribute != netcdf::Attribute::MISSING)
    {
        if (boost::iequals(attribute.get_string(), "latitude"))
        {
            type_ = kLatitude;
        }
        else if (boost::iequals(attribute.get_string(), "longitude"))
        {
            type_ = kLongitude;
        }
    }

    // Determines axis from units
    if (type_ == kUnknown && unit_ != "")
    {
        if (g_axis_latitude_unit(unit_))
            type_ = kLatitude;
        else if (g_axis_longitude_unit(unit_))
            type_ = kLongitude;
    }

    // Determines axis from generic spatial coordinates
    if (type_ == kUnknown)
    {
        attribute = variable.FindAttributeIgnoreCase(netcdf::CF::AXIS);
        if (attribute != netcdf::Attribute::MISSING)
        {
            if (boost::iequals(attribute.get_string(), "Y"))
            {
                type_ = kY;
            }
            else if (boost::iequals(attribute.get_string(), "X"))
            {
                type_ = kX;
            }
        }
    }

    variable.Read(points_);

    ComputeProperties();
}

// ___________________________________________________________________________//

//
// True if a & b are the same coordinate
//
static inline bool IsSame(const double a, const double b)
{
    return std::fabs(a - b) <= 1e-4;
}

// ___________________________________________________________________________//

void Axis::CalcIsRegular()
{
    int n = points_.size();

    start_ = GetCoordinateValue(0);

    // The axis is defined by a single value.
    if (n < 2)
    {
        is_regular_ = true;
        increment_ = 1;
    }
    else
    {
        increment_ = (GetCoordinateValue(n - 1) - start_) / (n - 1);
        is_regular_ = true;

        for (int ix = 1; ix < n; ++ix)
        {
            if (!IsSame(GetCoordinateValue(ix) - GetCoordinateValue(ix - 1),
                    increment_))
            {
                is_regular_ = false;
                break;
            }
        }
    }

    is_circle_ = is_regular_ && type_ == kLongitude
            ? IsSame(increment_ * GetNumElements(), 360)
            : false;
}

}
