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

#include <boost/math/special_functions.hpp>
#include <cmath>

// ___________________________________________________________________________//

#include "misc.hpp"
#include "vonkarman_field.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace field
{
bool Vonkarman::Compute(const double t,
        const double x,
        const double y,
        double& u,
        double& v) const
{
    const double x2 = Square(x);
    const double y2 = Square(y);
    const double rho = sqrt(x2 + y2);
    const double xv1 = 1.0 + l_ * FractionnalPart(t / tc_);
    const double xv2 = 1.0 + l_ * FractionnalPart((t - tc_ / 2.0) / tc_);
    const double d4 = exp(-(x2 - 2 * x + 1) / alpha2_ - y2);
    const double s = 1 - d4;
    const double h1 = fabs(sin(M_PI * t / tc_));
    const double h2 = fabs(sin(M_PI * (t - tc_ / 2) / tc_));
    const double g1 = exp(-r0_ * (alpha2_ * Square(y - y0_) + Square(x - xv1)));
    const double g2 = exp(-r0_ * (alpha2_ * Square(y + y0_) + Square(x - xv2)));
    const double g = s * u0_ * y + (g2 * h2 - g1 * h1) * w_;
    const double a = -2 * r0_;
    const double b = a * alpha2_;
    const double gx = ((d4 * u0_ * (2 * x - 2) * y) / alpha2_) + (a * g2 * h2
            * w_ * (-xv2 - a * g1 * h1 * (x - xv1) + x));
    const double gy = b * g2 * h2 * w_ * (y0_ + y - b * g1 * h1 * (y - y0_))
            + u0_ * (2 * d4 * y2 + s);
    const double f = -exp(-a_ * Square(rho - 1));
    const double d11 = (-2 * a_ * (rho - 1)) / rho;

    u = f * (gy + (y * d11) * g) + gy;
    v = -(f * (gx + (x * d11) * g) + gx);

    return true;
}

} // namespace field
} // namespace lagrangian
