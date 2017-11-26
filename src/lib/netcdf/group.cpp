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

#include "lagrangian/netcdf/group.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

Attribute const& Group::FindAttribute(const std::string& name) const
{
    for (auto& item: attributes_)
    {
        if (item.get_name() == name)
            return item;
    }
    return Attribute::MISSING;
}

// ___________________________________________________________________________//

Attribute const& Group::FindAttributeIgnoreCase(const std::string& name) const
{
    for (auto& item: attributes_)
    {
        if (boost::iequals(item.get_name(), name))
            return item;
    }
    return Attribute::MISSING;
}

} // namespace netcdf
} // namespace lagrangian
