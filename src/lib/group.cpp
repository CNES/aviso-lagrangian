/*
 * group.cpp
 *
 *  Created on: 22 avr. 2011
 *      Author: fbriol
 */

#include <boost/algorithm/string.hpp>

// ___________________________________________________________________________//

#include "group.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

Attribute const& Group::FindAttribute(const std::string& name) const
{
    std::list<Attribute>::const_iterator it;

    for (it = attributes_.begin(); it != attributes_.end(); ++it)
    {
        if ((*it).get_name() == name)
            return *it;
    }
    return Attribute::MISSING;
}

// ___________________________________________________________________________//

Attribute const& Group::FindAttributeIgnoreCase(const std::string& name) const
{
    std::list<Attribute>::const_iterator it;

    for (it = attributes_.begin(); it != attributes_.end(); ++it)
    {
        if (boost::iequals((*it).get_name(), name))
            return *it;
    }
    return Attribute::MISSING;
}

} // namespace netcdf
} // namespace lagrangian
