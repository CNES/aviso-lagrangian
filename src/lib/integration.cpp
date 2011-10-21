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

#include "integration.hpp"

namespace lagrangian
{

void FiniteLyapunovExponents::Exponents(const Triplet& p)
{
    const double delta_t = p.get_time() - start_time_;

    double a00 = p.get_x1() - p.get_x0();
    double a01 = p.get_x2() - p.get_x0();
    double a10 = p.get_y1() - p.get_y0();
    double a11 = p.get_y2() - p.get_y0();

    if (field_->get_unit_type() == Field::kAngular)
    {
        a00 = NormalizeLongitude(a00, 360, 180);
        a01 = NormalizeLongitude(a01, 360, 180);
    }

    const double square_a00 = Square(a00);
    const double square_a01 = Square(a01);
    const double square_a10 = Square(a10);
    const double square_a11 = Square(a11);

    const double f1 = 1 / (2 * delta_t);
    const double s1 = square_a00 + square_a01 + square_a10 + square_a11;
    const double s2 = sqrt((Square(a01 + a10) + Square(a00 - a11))
            * (Square(a01 - a10) + Square(a00 + a11)));

    lambda1_ = f1 * log(f2_ * (s1 + s2));
    lambda2_ = f1 * log(f2_ * (s1 - s2));

    if ((a01 == 0) && (a10 == 0))
    {
        if (a00 > a11)
        {
            theta1_ = 0;
            theta2_ = 90;
        }
        else
        {
            theta2_ = 0;
            theta1_ = 90;
        }
    }
    else
    {
        const double at1 = 2 * (a00 * a01 + a10 * a11);
        const double at2 = square_a00 - square_a01 + square_a10 - square_a11;
        theta1_ = RadiansToDegrees(atan(at1 / (at2 + s2)));
        theta2_ = RadiansToDegrees(-atan(at1 / (-at2 + s2)));
    }
}

}
