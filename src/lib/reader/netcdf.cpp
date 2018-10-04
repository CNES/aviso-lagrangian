/*
 This file is part of lagrangian library.

 lagrangian is free software: you can redistribute it and/or modify
 it under the terms of GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 lagrangian is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of GNU Lesser General Public License
 along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lagrangian/reader/netcdf.hpp"

// ___________________________________________________________________________//

namespace lagrangian {
namespace reader {

static inline double BilinearInterpolation(const double x0, const double x1,
                                           const double y0, const double y1,
                                           const double z00, const double z10,
                                           const double z01, const double z11,
                                           const double x,
                                           const double y) noexcept {
  double dx0 = x - x0;
  double dy0 = y - y0;
  double dx1 = x1 - x;
  double dy1 = y1 - y;

  return (dy1 * (dx1 * z00 + dx0 * z10) + dy0 * (dx1 * z01 + dx0 * z11)) /
         ((x1 - x0) * (y1 - y0));
}

// ___________________________________________________________________________//

void Netcdf::Open(const std::string& filename) {
  netcdf_ = lagrangian::Netcdf(filename);

  auto variables = netcdf_.get_variables();
  auto it = variables.begin();

  while (it != variables.end()) {
    if ((*it).IsCoordinateVariable()) {
      Axis axis(*it);

      switch (axis.get_type()) {
        // Spatial coordinate
        case Axis::kLatitude:
          axis_y_ = std::move(axis);
          break;
        case Axis::kLongitude:
          axis_x_ = std::move(axis);
          break;
        // Generic spatial coordinate
        case Axis::kX:
          axis_x_ = std::move(axis);
          break;
        case Axis::kY:
          axis_y_ = std::move(axis);
          break;
        default:
          break;
      }
      it = variables.erase(it);
    } else {
      it++;
    }
  }

  if (axis_x_.get_type() == Axis::kUnknown ||
      axis_y_.get_type() == Axis::kUnknown) {
    throw std::logic_error(
        "Unable to Find the description of spatial"
        " coordinates.");
  }

  // The axes are to be defined in degrees.
  if (axis_x_.get_type() == Axis::kLongitude) {
    axis_x_.Convert("degrees");
  }
  if (axis_y_.get_type() == Axis::kLatitude) {
    axis_y_.Convert("degrees");
  }
}

// ___________________________________________________________________________//

void Netcdf::Load(const std::string& name, const std::string& unit) {
  netcdf::Variable variable = FindVariable(name);

  unit.empty() ? variable.Read(data_) : variable.Read(data_, unit);

  pGetIndex_ =
      variable.get_shape(0) == static_cast<size_t>(axis_y_.GetNumElements())
          ? &Netcdf::GetIndexYX
          : &Netcdf::GetIndexXY;
}

// ___________________________________________________________________________//

double Netcdf::Interpolate(const double longitude, const double latitude,
                           const double fill_value,
                           CellProperties& cell) const {
  if (data_.empty()) {
    throw std::logic_error("No data loaded into memory");
  }

  double x = axis_x_.Normalize(longitude, 360);

  if (!cell.Contains(x, latitude)) {
    int ix0, ix1;
    int iy0, iy1;

    if (!axis_x_.FindIndexes(x, ix0, ix1) ||
        !axis_y_.FindIndexes(latitude, iy0, iy1)) {
      // The search for the new cell is forced for the next call to this
      // method.
      cell = CellProperties::NONE();
      return fill_value;
    }

    cell.Update(axis_x_.GetCoordinateValue(ix0),
                axis_x_.GetCoordinateValue(ix1),
                axis_y_.GetCoordinateValue(iy0),
                axis_y_.GetCoordinateValue(iy1), ix0, ix1, iy0, iy1);
  }

  return BilinearInterpolation(cell.x0(), cell.x1(), cell.y0(), cell.y1(),
                               GetValue(cell.ix0(), cell.iy0(), fill_value),
                               GetValue(cell.ix1(), cell.iy0(), fill_value),
                               GetValue(cell.ix0(), cell.iy1(), fill_value),
                               GetValue(cell.ix1(), cell.iy1(), fill_value), x,
                               latitude);
}

// ___________________________________________________________________________//

DateTime Netcdf::GetDateTime(const std::string& name) const {
  netcdf::Variable variable = FindVariable(name);
  netcdf::Attribute attribute = variable.FindAttributeIgnoreCase("date");

  if (attribute == netcdf::Attribute::MISSING) {
    throw std::logic_error(name + ":date: No such attribute");
  }

  return DateTime(attribute.get_string());
}

}  // namespace reader
}  // namespace lagrangian
