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

#include "time_serie_field.hpp"

namespace lagrangian
{
namespace field
{

TimeSerie::TimeSerie(const std::string& configuration,
        const Field::UnitType unit_type) :
    Field(unit_type), u_(NULL), v_(NULL)
{
    Parameter p(configuration);

    u_ = new lagrangian::TimeSerie(p.Values<std::string> ("U"),
            p.Value<std::string> ("U_NAME"),
            GetUnit());
    v_ = new lagrangian::TimeSerie(p.Values<std::string> ("V"),
            p.Value<std::string> ("V_NAME"),
            GetUnit());
}

bool TimeSerie::Compute(const double t,
        const double x,
        const double y,
        double& u,
        double& v) const
{
    u = u_->Interpolate(t, x, y);
    v = v_->Interpolate(t, x, y);

    return isnan(u) || isnan(v) ? false : true;
}

} // namespace field
} // namespace lagrangian
