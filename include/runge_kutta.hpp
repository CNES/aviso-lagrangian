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

#ifndef RUNGE_KUTTA_HPP_
#define RUNGE_KUTTA_HPP_

#include "field.hpp"
#include "misc.hpp"

namespace lagrangian
{

/**
 * Cartesian coordinate system
 */

class Cartesian
{
private:
    double x_;
    double y_;
    double z_;

public:
    Cartesian(const double x, const double y, const double z) :
        x_(x), y_(y), z_(z)
    {
    }

    /**
     * @brief Build a new instance from spherical coordinate system
     *
     * @param longitude longitude in radians
     * @param latitude latitude in radians
     */
    Cartesian(const double longitude, const double latitude)
    {
        const double cos_y = cos(latitude);

        x_ = kEarthRadius * cos_y * cos(longitude);
        y_ = kEarthRadius * cos_y * sin(longitude);
        z_ = kEarthRadius * sin(latitude);
    }

    /**
     * @brief Returns the x component in Cartesian coordinate system
     *
     * @return x
     */
    inline double get_x() const
    {
        return x_;
    }

    /**
     * @brief Returns the y component in Cartesian coordinate system
     *
     * @return y
     */
    inline double get_y() const
    {
        return y_;
    }

    /**
     * @brief Returns the z component in Cartesian coordinate system
     *
     * @return z
     */
    inline double get_z() const
    {
        return z_;
    }

    /**
     * @brief Computes longitude value from Cartesian coordinate
     *
     * @return longitude in degrees
     */
    inline double GetLongitude() const
    {
        return RadiansToDegrees(atan2(y_, x_));
    }

    /**
     * @brief Computes latitude value from Cartesian coordinate
     *
     * @return latitude in degrees
     */
    inline double GetLatitude() const
    {
        return RadiansToDegrees(asin(z_ / sqrt(x_ * x_ + y_ * y_ + z_ * z_)));
    }
};

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

    inline void MoveAngular(const double t,
            const double x,
            const double y,
            const double u,
            const double v,
            double& xi,
            double& yi) const
    {
        xi = x + u * t;
        yi = y + v * t;
    }

    inline void MoveMetric(const double t,
            const double x,
            const double y,
            const double u,
            const double v,
            double& xi,
            double& yi) const
    {
        const double xr = DegreesToRadians(x);
        const double yr = DegreesToRadians(y);
        const double sin_x = sin(xr);
        const double cos_x = cos(xr);
        const double sin_y = sin(yr);

        Cartesian c0(xr, yr);

        Cartesian c1(c0.get_x() + (-u * sin_x - v * cos_x * sin_y) * t,
                c0.get_y() + (u * cos_x - v * sin_y * sin_x) * t,
                c0.get_z() + (v * cos(yr)) * t);

        xi = c1.GetLongitude();
        yi = c1.GetLatitude();
    }

public:
    /*
     * time_intervals number of time intervals.
     * (h) size of interval : (end_time - start_time) / time_intervals
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

    bool Compute(const double t,
            const double x,
            const double y,
            double &xi,
            double &yi) const
    {
        double u1, u2, u3, u4;
        double v1, v2, v3, v4;
        double xn, yn;

        // RK step 1
        // If asked position is not defined in the field (The method "Compute"
        // returns false): the computation is finished
        if (field_->Compute(t, x, y, u1, v1))
        {
            (this->*pMove_)(h_2_, x, y, u1, v1, xn, yn);

            // RK step 2
            if (field_->Compute(t + h_2_, xn, yn, u2, v2))
            {
                (this->*pMove_)(h_2_, x, y, u2, v2, xn, yn);

                // RK step 3
                if (field_->Compute(t + h_2_, xn, yn, u3, v3))
                {
                    (this->*pMove_)(h_, x, y, u3, v3, xn, yn);

                    // RK step 4
                    if (field_->Compute(t + h_, xn, yn, u4, v4))
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
}
#endif /* RUNGE_KUTTA_HPP_ */
