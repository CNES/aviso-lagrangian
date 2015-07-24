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

#include "lagrangian/units.hpp"
#include "lagrangian/netcdf/cf.hpp"
#include "lagrangian/netcdf/variable.hpp"

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
    ncvar_.getVar(&data[0]);

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
    Units::GetConverter(from, to).Convert(data);
}

// ___________________________________________________________________________//

Variable::Variable(const netCDF::NcVar& ncvar) :
        name_(ncvar.getName()), ncvar_(ncvar)
{
    std::map<std::string, netCDF::NcVarAtt> atts = ncvar_.getAtts();
    std::vector<netCDF::NcDim> dims = ncvar_.getDims();

    // Set globals attributes
    for (std::map<std::string, netCDF::NcVarAtt>::iterator it = atts.begin();
            it != atts.end(); ++it)
        attributes_.push_back(Attribute(it->second));

    // populates defined variables
    for (std::vector<netCDF::NcDim>::iterator it = dims.begin();
            it != dims.end(); ++it)
    {
        shape_.push_back(it->getSize());
        dimensions_.push_back(Dimension(it->getName(),
                it->getSize(),
                it->isUnlimited()));
    }

    // Set scale, offset, missing and invalid data from attributes
    scale_missing_ = ScaleMissing(*this);
}

// ___________________________________________________________________________//

Variable const Variable::MISSING;

} // namespace netcdf
} // namespace lagrangian
