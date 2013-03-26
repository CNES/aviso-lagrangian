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

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/minmax.hpp>

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
    
    /**
     * @brief Default constructor
     *
     * @param configuration_file The configuration file contains the list of
     * files to take into account to interpolate speeds.
     * @param unit_type Unit fields.
     * @param reader_type The reader used to read grids containing speeds.
     */
    TimeSerie(const std::string& configuration_file,
            const Field::UnitType unit_type = kMetric,
            const reader::Factory::Type reader_type = reader::Factory::kNetCDF);

    /**
    * @brief Default method invoked when a TimeSerie is destroyed.
    */
    ~TimeSerie()
    {
        delete u_;
        delete v_;
    }

    /**
     * @brief Loads the grids used to interpolate the velocities in the
     * interval [t0, t1]
     *
     * @param t0 First date of the interval expressed as a number of seconds
     * elapsed since 1970.
     * @param t1 Last date of the interval expressed as a number of seconds
     * elapsed since 1970.
     */
    inline void Fetch(const double t0, const double t1)
    {
        u_->Load(t0, t1);
        v_->Load(t0, t1);
    }

    /**
     * @brief Interpolates the velocity to the wanted spatio temporal position.
     *
     * @param t Time expressed as a number of seconds elapsed since 1970.
     * @param x Longitude expressed as degree
     * @param y Latitude expressed as degree
     * @param u
     * @param v
     *
     * @return true if the value of the speed is set otherwise false.
     */
    bool Compute(const double t,
            const double x,
            const double y,
            double& u,
            double& v) const;

    /**
     * @brief Returns the date of the first grid constituting the time series.
     *
     * @return the julian day of the first date
     */
    inline JulianDay StartTime() const
    {
        boost::tuple<double const&, double const&> minmax =
                boost::minmax(u_->GetFirstDate(), v_->GetFirstDate());
        return JulianDay(JulianDay::FromUnixTime(minmax.get<1> ()));
    }

    /**
    * @brief Returns the date of the last grid constituting the time series.
    *
    * @return the julian day of the last date
    */
    inline JulianDay EndTime() const
    {
        boost::tuple<double const&, double const&> minmax =
                boost::minmax(u_->GetLastDate(), v_->GetLastDate());
        return JulianDay(JulianDay::FromUnixTime(minmax.get<0> ()));
    }
};

} // namespace field
} // namespace lagrangian
