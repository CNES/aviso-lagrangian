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

// ___________________________________________________________________________//

namespace lagrangian
{

bool FiniteLyapunovExponents::Exponents(const Position& p)
{
    // Advection time T
    const double delta_t = p.get_time() - start_time_;

    if (fabs(delta_t) < std::numeric_limits<double>::epsilon())
        return false;

    // Get element of the gradient of the flow map
    // ∇Φ =  1 / δ₀  * [ a₀₀ a₀₁ ]
    //                 [ a₁₀ a₁₁ ]
    // where δ₀ is the initial separation distance of the particules
    double a00;
    double a01;
    double a10;
    double a11;

    p.StrainTensor(a00, a01, a10, a11);

    if (field_->get_unit_type() == Field::kAngular)
    {
        a00 = NormalizeLongitude(a00, 360, 180);
        a01 = NormalizeLongitude(a01, 360, 180);
    }

    // Compute the eigenvalue of the Cauchy-Green strain tensor
    // Δ = (∇Φ)^*(∇Φ)
    // (* is the transposition operator)
    //
    // ∇Φ = [ a₀₀/δx₀ a₀₁/δy₀ ]
    //      [ a₁₀/δx₀ a₁₁/δy₀ ]
    //
    // Here the initial separation distance is isotropic: (i.e. δx₀ = δy₀ = δ₀)
    // then
    //
    // Δ = (a₀₀/δ₀²) * [ a₀₀² + a₀₁²       a₀₀*a₁₀ + a₀₁*a₁₁ ]
    //                 [ a₀₀*a₁₀ + a₀₁*a₁₁       a₁₁² + a₁₀² ]
    //
    //
    // Then eigenvalues of a 2x2 matrix are given by:
    // σ± = (1/δ²) * [ Tr(Δ) ± √( Tr(Δ)² - 4 det(Δ)) ] / 2
    //
    // where
    // Tr(Δ) = a₀₀² + a₀₁² + a₁₁² + a₁₀²
    //
    // and
    // det(Δ) = ( a₀₀² + a₀₁² ) * (a₁₁² + a₁₀²) - (a₀₀*a₁₀ + a₀₁*a₁₁)²
    const double square_a00 = Square(a00);
    const double square_a01 = Square(a01);
    const double square_a10 = Square(a10);
    const double square_a11 = Square(a11);
    
    const double f1 = 1 / (2 * delta_t);
    
    // Compute Tr(Δ):
    const double s1 = square_a00 + square_a01 + square_a10 + square_a11;

    // Compute √(Tr(Δ)² - 4 det(Δ))
    // Remark: the following factorized equivalent formula was obtained
    // using formal calculus: see end of this file
    const double s2 = sqrt((Square(a01 + a10) + Square(a00 - a11))
			   * (Square(a01 - a10) + Square(a00 + a11)));

    // f2_ = 1 / ( 2 δ₀²) 
    // f2_ * (s1 + s2) is the maximum eigenvalue
    // f2_ * (s1 - s2) is the minimum eigenvalue

    // lambda1_ is the exponent computed from the maximum eigenvalue:
    // lambda1_ = ( 1 / (2*T) ) * log( ( λmax( Δ ) )
    lambda1_ = f1 * log(f2_ * (s1 + s2));

    // lambda2 is the exponent computed from the minimum eigenvalue
    // lambda1_ = ( 1 / (2*T) ) * log( ( λmin( Δ ) )
    lambda2_ = f1 * log(f2_ * (s1 - s2));

    // Compute the orientation theta1 and theta2
    // of the corresponding eigenvalue of Δ
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

    return true;
}

}

// Details about the factorization of the eigenvalue of Δ
//
// Δ is a 2x2 symmetric matrix of the form
// (1/δ₀) * A where
// A = [ a b ]
//     [ b d ]
// whith
// a = a₀₀² + a₀₁²
// b = a₀₀*a₁₀ + a₀₁*a₁₁
// d = a₁₁² + a₁₀²
//
// On the first hand we have:
// 
// Tr(A)² - 4 det(A) = ( a - d )² + 4 b²
// = (a₀₀² + a₀₁² - a₁₁² - a₁₀² ) + 4 * (a₀₀*a₁₀ + a₀₁*a₁₁)²
//
// On the second hand we have:
//
// s₂²
// = [ (a₀₁ + a₁₀)² + (a₀₀ - a₁₁)² ] * [ (a₀₁ - a₁₀)² + (a₀₀ + a₁₁)² ]
// = (a₀₁ + a₁₀)² * (a₀₁ - a₁₀)²
// + (a₀₁ + a₁₀)² * (a₀₀ + a₁₁)²
// + (a₀₀ - a₁₁)² * (a₀₁ - a₁₀)²
// + (a₀₀ - a₁₁)² * (a₀₀ + a₁₁)²
//
// = (a₀₁² - a₁₀²)² // using (a-b)² = (a-b)*(a+b)
// + (a₀₁ + a₁₀)² * (a₀₀ + a₁₁)² + (a₀₀ - a₁₁)² * (a₀₁ - a₁₀)²
// + (a₀₀² - a₁₁²)² // using (a-b)² = (a-b)*(a+b)
//
// = (a₀₀² + a₀₁² - a₁₁² - a₁₀² ) - 2 * (a₀₁² - a₁₀²) * (a₀₀² - a₁₁² ) // using a² + b² = (a+b)² - 2*a*b
// + (a₀₁ + a₁₀)² * (a₀₀ + a₁₁)² + (a₀₀ - a₁₁)² * (a₀₁ - a₁₀)²
//
// = (a₀₀² + a₀₁² - a₁₁² - a₁₀² )
// - 2 * (a₀₁ + a₁₀ ) * (a₀₁ - a₁₀ ) * (a₀₀ + a₁₁ ) * (a₀₀ - a₁₁ )
// + (a₀₁ + a₁₀)² * (a₀₀ + a₁₁)² + (a₀₀ - a₁₁)² * (a₀₁ - a₁₀)²
//
// We recall that 
// a = a₀₀² + a₀₁²
// d = a₁₁² + a₁₀²
// b = a₀₀*a₁₀ + a₁₁*a₀₁
// and we set
// f = a₀₁ + a₁₀
// g = a₀₀ + a₁₁
// h = a₀₁ - a₁₀
// m = a₀₀ - a₁₁
//
// we get
//
// s2² = (a-d)² - 2f*h*m*g + f²*g² + m²*h²
// = (a-d)² + (f*g)² + (m*h)² - 2(f*g)*(m*h)
// = (a-d)² + (f*g - m*h)² 
//
// and
// (f*g - m*h)² =  [ (a₀₁+a₁₀) * (a₀₀+a₁₁) - (a₀₀-a₁₁)*(a₀₁-a₁₀) ] ^ 2
// = [ 2 * (a₀₀*a₁₀ + a₁₁*a₀₁) ]² // after developpement 
// = 4 b²
//
// Finally we get
// s2² = (a-d)² + 4 b² = Tr(A)² + 4 det(A)
