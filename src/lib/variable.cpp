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

#include "cf.h"
#include "units.hpp"
#include "variable.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace netcdf
{

bool Variable::GetDescription(std::string& desc) const
{
    Attribute attribute = FindAttributeIgnoreCase("long_name");

    if (attribute != Attribute::MISSING && attribute.IsString())
        desc = attribute.get_string();
    else
    {
        attribute = FindAttributeIgnoreCase("description");
        if (attribute != Attribute::MISSING && attribute.IsString())
            desc = attribute.get_string();
        else
        {
            attribute = FindAttributeIgnoreCase("title");
            if (attribute != Attribute::MISSING && attribute.IsString())
                desc = attribute.get_string();
            else
            {
                attribute = FindAttributeIgnoreCase("standard_name");
                if (attribute != Attribute::MISSING && attribute.IsString())
                    desc = attribute.get_string();
            }
        }
    }

    return attribute != Attribute::MISSING;
}

// ___________________________________________________________________________//

bool Variable::GetUnitsString(std::string& units) const
{
    Attribute attribute = FindAttributeIgnoreCase("units");
    if (attribute != Attribute::MISSING && attribute.IsString())
    {
        units = attribute.get_string();
        boost::trim(units);
        return true;
    }
    return false;
}

// ___________________________________________________________________________//

void Variable::Read(std::vector<double>& data) const
{
    data.resize(GetSize());
    ncvar_->get(&data[0], &shape_[0]);

    // Set "missing" data to nan
    scale_missing_.SetMissingToNan(data);

    // Convert data with scale and offset
    scale_missing_.ConvertScaleOffset(data);
}

// ___________________________________________________________________________//

void Variable::Read(std::vector<double>& data, const std::string& to) const
{
    std::string from;

    if (!GetUnitsString(from))
        throw std::logic_error(name_ + ":" + CF::UNITS + ": no such attribute");

    Read(data);
    Units::ConvertData(from, to, data);
}

// ___________________________________________________________________________//

Variable::Variable(NcVar* ncvar) :
    name_(ncvar->name()), ncvar_(ncvar)
{
    // Set globals attributes
    for (int ix = 0; ix < ncvar_->num_atts(); ++ix)
    {
        NcAtt* ncatt = ncvar_->get_att(ix);

        attributes_.push_back(Attribute(ncatt));
        delete ncatt;
    }

    // populates defined variables
    for (int ix = 0; ix < ncvar_->num_dims(); ++ix)
    {
        NcDim* dim = ncvar_->get_dim(ix);

        shape_.push_back(dim->size());
        dimensions_.push_back(Dimension(dim->name(),
                dim->size(),
                dim->is_unlimited() == 1));
    }

    // Set scale, offset, missing and invalid data from attributes
    scale_missing_ = ScaleMissing(*this);
}

// ___________________________________________________________________________//

Variable const Variable::MISSING;

} // namespace netcdf
} // namespace lagrangian
