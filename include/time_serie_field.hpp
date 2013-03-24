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

#pragma once

// ___________________________________________________________________________//

#include "parameter.hpp"
#include "time_serie.hpp"
#include "field.hpp"
#include "reader_factory.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace field
{

class TimeSerie: public Field
{
private:
    lagrangian::TimeSerie *u_;
    lagrangian::TimeSerie *v_;
    bool same_coordinates_;

public:
    TimeSerie(const std::string& configuration_file,
            const Field::UnitType unit_type = kMetric,
            const reader::Factory::Type reader_type = reader::Factory::kNetCDF);

    ~TimeSerie()
    {
        delete u_;
        delete v_;
    }

    inline void Fetch(const double t0, const double t1)
    {
        u_->Load(t0, t1);
        v_->Load(t0, t1);
    }

    bool Compute(const double t,
            const double x,
            const double y,
            double& u,
            double& v) const;

    inline JulianDay StartTime() const
    {
        boost::tuple<double const&, double const&> minmax =
                boost::minmax(u_->GetFirstDate(), v_->GetFirstDate());
        return JulianDay(JulianDay::FromUnixTime(minmax.get<1> ()));
    }

    inline JulianDay EndTime() const
    {
        boost::tuple<double const&, double const&> minmax =
                boost::minmax(u_->GetLastDate(), v_->GetLastDate());
        return JulianDay(JulianDay::FromUnixTime(minmax.get<0> ()));
    }
};

} // namespace field
} // namespace lagrangian
