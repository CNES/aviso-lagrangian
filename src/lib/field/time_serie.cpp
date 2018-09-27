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
    along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lagrangian/field/time_serie.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

// ___________________________________________________________________________//

namespace field {

TimeSerie::TimeSerie(const std::string& configuration,
                     const Field::UnitType unit_type,
                     const reader::Factory::Type reader_type)
    : Field(unit_type), u_(nullptr), v_(nullptr) {
  Parameter p(configuration);

  u_ = new lagrangian::TimeSerie(p.Values<std::string>("U"),
                                 p.Value<std::string>("U_NAME"), GetUnit(),
                                 reader_type);
  v_ = new lagrangian::TimeSerie(p.Values<std::string>("V"),
                                 p.Value<std::string>("V_NAME"), GetUnit(),
                                 reader_type);
  fill_value_ = p.Exists("FILL_VALUE") ? p.Value<double>("FILL_VALUE") : 0;
}

// ___________________________________________________________________________//

bool TimeSerie::Compute(const double t, const double x, const double y,
                        double& u, double& v, CellProperties& cell) const {
  u = u_->Interpolate(t, x, y, fill_value_, cell);
  v = v_->Interpolate(t, x, y, fill_value_, cell);

  return !(std::isnan(u) || std::isnan(v));
}

}  // namespace field
}  // namespace lagrangian
