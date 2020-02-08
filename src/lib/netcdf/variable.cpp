// This file is part of lagrangian library.
//
// lagrangian is free software: you can redistribute it and/or modify
// it under the terms of GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// lagrangian is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of GNU Lesser General Public License
// along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
#include <boost/algorithm/string.hpp>

// ___________________________________________________________________________//

#include "lagrangian/netcdf/cf.hpp"
#include "lagrangian/netcdf/variable.hpp"
#include "lagrangian/units.hpp"

// ___________________________________________________________________________//

namespace lagrangian::netcdf {

bool Variable::GetDescription(std::string& desc) const {
  Attribute attribute = FindAttributeIgnoreCase("long_name");

  if (attribute != Attribute::MISSING && attribute.IsString()) {
    desc = attribute.get_string();
  } else {
    attribute = FindAttributeIgnoreCase("description");
    if (attribute != Attribute::MISSING && attribute.IsString()) {
      desc = attribute.get_string();
    } else {
      attribute = FindAttributeIgnoreCase("title");
      if (attribute != Attribute::MISSING && attribute.IsString()) {
        desc = attribute.get_string();
      } else {
        attribute = FindAttributeIgnoreCase("standard_name");
        if (attribute != Attribute::MISSING && attribute.IsString()) {
          desc = attribute.get_string();
        }
      }
    }
  }

  return attribute != Attribute::MISSING;
}

// ___________________________________________________________________________//

bool Variable::GetUnitsString(std::string& units) const {
  Attribute attribute = FindAttributeIgnoreCase("units");
  if (attribute != Attribute::MISSING && attribute.IsString()) {
    units = attribute.get_string();
    boost::trim(units);
    return true;
  }
  return false;
}

// ___________________________________________________________________________//

void Variable::Read(std::vector<double>& data) const {
  data.resize(GetSize());
  ncvar_.getVar(&data[0]);

  // Set "missing" data to nan
  scale_missing_.SetMissingToNan(data);

  // Convert data with scale and offset
  scale_missing_.ConvertScaleOffset(data);
}

// ___________________________________________________________________________//

void Variable::Read(std::vector<double>& data, const std::string& to) const {
  std::string from;

  if (!GetUnitsString(from)) {
    throw std::logic_error(name_ + ":" + CF::UNITS + ": no such attribute");
  }

  Read(data);
  Units::GetConverter(from, to).Convert(data);
}

// ___________________________________________________________________________//

Variable::Variable(const netCDF::NcVar& ncvar)
    : name_(ncvar.getName()), ncvar_(ncvar) {
  // Set globals attributes
  for (auto& item : ncvar_.getAtts()) {
    attributes_.emplace_back(Attribute(item.second));
  }

  // populates defined variables
  for (auto& item : ncvar_.getDims()) {
    auto size = item.getSize();

    shape_.push_back(size);
    dimensions_.emplace_back(
        Dimension(item.getName(), size, item.isUnlimited()));
  }

  // Set scale, offset, missing and invalid data from attributes
  scale_missing_ = ScaleMissing(*this);
}

// ___________________________________________________________________________//

Variable const Variable::MISSING;

}  // namespace lagrangian::netcdf
