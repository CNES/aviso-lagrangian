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

#include "field.hpp"
#include "misc.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

/**
 * @brief Fourth-order Runge-Kutta method
 */
class RungeKutta
{
private:
    double h_;
    double h_2_;
    double h_6_;
    const Field* const field_;

    typedef void
    (RungeKutta::*MoveFunction)(const double t,
            const double x,
            const double y,
            const double u,
            const double v,
            double& xi,
            double& yi) const;

    MoveFunction pMove_;

    // Move a point in a field speed expressed in degrees per second
    inline void MoveAngular(const double t,
            const double x0,
            const double y0,
            const double u,
            const double v,
            double& x1,
            double& y1) const
    {
        x1 = x0 + u * t;
        y1 = y0 + v * t;
    }

    // Move a point in a field speed expressed in meters per second
    inline void MoveMetric(const double t,
            const double x0,
            const double y0,
            const double u,
            const double v,
            double& x1,
            double& y1) const
    {
        const double xr = DegreesToRadians(x0);
        const double yr = DegreesToRadians(y0);
        const double sin_x = sin(xr);
        const double cos_x = cos(xr);
        const double sin_y = sin(yr);
        const double cos_y = cos(yr);

        double x = kEarthRadius * cos_y * cos_x;
        double y = kEarthRadius * cos_y * sin_x;
        double z = kEarthRadius * sin_y;

        x += (-u * sin_x - v * cos_x * sin_y) * t;
        y += ( u * cos_x - v * sin_y * sin_x) * t;
        z += ( v * cos_y) * t;

        x1 = RadiansToDegrees(atan2(y, x));
        y1 = RadiansToDegrees(asin(z / sqrt(x * x + y * y + z * z)));
    }

public:

    /**
     * @brief Default constructor
     *
     * @param size_of_interval Number of time interval
     * @param field Field reader
     */
    RungeKutta(const double size_of_interval, const Field* const field) :
        h_(size_of_interval), h_2_(h_ / 2), h_6_(h_ / 6), field_(field)
    {
        switch (field_->get_unit_type())
        {
        case Field::kAngular:
            pMove_ = &RungeKutta::MoveAngular;
            break;
        case Field::kMetric:
            pMove_ = &RungeKutta::MoveMetric;
            break;
        }
    }

    /**
     * @brief Move a point in a field
     *
     * @param t Time in number of seconds elapsed since 1970
     * @param x Longitude in degrees
     * @param y Latitude in degrees
     * @param xi Longitude after the move
     * @param yi Latitude after the move
     * @param cell Cell properties of the grid used for the interpolation
     * @return
     */
    inline bool Compute(const double t,
            const double x,
            const double y,
            double& xi,
            double& yi,
            CellProperties& cell=CellProperties::NONE()) const
    {
        double u1, u2, u3, u4;
        double v1, v2, v3, v4;
        double xn, yn;

        // RK step 1
        // If asked position is not defined in the field (The method "Compute"
        // returns false): the computation is finished
        if (field_->Compute(t, x, y, u1, v1, cell))
        {
            (this->*pMove_)(h_2_, x, y, u1, v1, xn, yn);

            // RK step 2
            if (field_->Compute(t + h_2_, xn, yn, u2, v2, cell))
            {
                (this->*pMove_)(h_2_, x, y, u2, v2, xn, yn);

                // RK step 3
                if (field_->Compute(t + h_2_, xn, yn, u3, v3, cell))
                {
                    (this->*pMove_)(h_, x, y, u3, v3, xn, yn);

                    // RK step 4
                    if (field_->Compute(t + h_, xn, yn, u4, v4, cell))
                    {
                        (this->*pMove_)(h_6_,
                                x,
                                y,
                                u1 + 2 * (u2 + u3) + u4,
                                v1 + 2 * (v2 + v3) + v4,
                                xi,
                                yi);
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

} // namespace lagangian
