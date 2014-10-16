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

  /*

    Finite Size Lyapunov Exponent (FSLE) is a scalar local notion that represents
    the rate of separation of initially neighbouring particles over a finite-time
    window [t0, t0 + T ], where T is the time two particules need
    to be advected in order to be separated from a given distance.
    
    Let x(t ) = x(t ; x0, t0) be the position of a Lagrangian particle
    at time t, started at x0 at t=t0 advected by the time-dependent
    fluid flow u(x, t). 
    
    The Forward Size-Time Lyapunov Exponent at a point x0 
    and for the advection time T is defined as the growth factor of
    the norm of the perturbation dx0 started around x0 and advected
    by the flow after the finite advection time T.
    Remark the T depends on x0.

    Maximal stretching occurs when dx0 is aligned with the eigenvector associated
    with the maximum eigenvalue \lambda_max of the Cauchy-Green strain tensor \Delta:

    \Delta = [ \grad \Phi_0^T (x0) ]^* [ \grad \Phi_0^T (x0) ]

    where \Phi_0^t : x0 -> x(t,x0,t0) is the flow map of the system: it links
    the location x0 of a lagragian particule at t=t0 to its position x(t,x0,t0)
    at time t. * denotes the transposition operator.

    FSLE is defined as

    \sigma = 1/T log( \sqrt( \lamda_max( \Delta ) ) / \delta_0 )
           = ( 1 / (2*T) ) * log( ( \lamda_max( \Delta ) ) / \delta_0 )

    where \delta_0 is the initial separation distance of the particules

  */

namespace lagrangian
{

bool FiniteLyapunovExponents::Exponents(const Triplet& p)
{

  /*

    Compute the eigenvalue and the orientation of the eigenvectors 
    of the Cauchy-Green strain tensor
    
   */

  // delta_t is the advection time T

  const double delta_t = p.get_time() - start_time_;

    if (fabs(delta_t) < std::numeric_limits<double>::epsilon())
        return false;

    /*

      Get element of the gradient of the flow map 

      \grad \Phi = [ a00 a01 ]
                   [ a10 a11 ]

     */
    
    double a00 = p.get_x1() - p.get_x0();
    double a01 = p.get_x2() - p.get_x0();
    double a10 = p.get_y1() - p.get_y0();
    double a11 = p.get_y2() - p.get_y0();

    if (field_->get_unit_type() == Field::kAngular)
    {
        a00 = NormalizeLongitude(a00, 360, 180);
        a01 = NormalizeLongitude(a01, 360, 180);
    }

    /*

      Compute the eigenvalue of the Cauchy-Green strain tensor
      \Delta = (\grad \Phi)^* (\grad \Phi)
      
      \Delta = [ a00^2 + a01^2       a00*a10 + a01*a11 ]
               [ a00*a10 + a01*a11       a11^2 + a10^2 ]

      Then eigenvalues of a 2x2 matrix are given by

      \sigma_(+/-) = ( Tr(\Delta) +/- \sqrt( Tr(\Delta)^2 - 4 det(\Delta) ) ) / 2

      where
      Tr(\Delta) = a00^2 + a01^2 + a11^2 + a10^2

      and
      det(\Delta) = ( a00^2 + a01^2 ) * (a11^2 + a10^2) - (a00*a10 + a01*a11)^2

    */

    const double square_a00 = Square(a00);
    const double square_a01 = Square(a01);
    const double square_a10 = Square(a10);
    const double square_a11 = Square(a11);
    
    const double f1 = 1 / (2 * delta_t);
    // Compute trace(\Delta):
    const double s1 = square_a00 + square_a01 + square_a10 + square_a11;
    // Compute \sqrt(Tr(\Delta)^2 - 4 det(\Delta))
    // Remark: the following factorized equivalent formula was obtained
    // using formal calculus: see end of this file
    
    const double s2 = sqrt((Square(a01 + a10) + Square(a00 - a11))
			   * (Square(a01 - a10) + Square(a00 + a11)));

    // (s1 + s2)/2 is the maximum eigenvalue
    // (s1 - s2)/2 is the minimum eigenvalue
    // f2_ = 1/ ( 2 delta_0^2) where delta_0 is the initial spatial separation

    // lambda1 is the exponent computed from the maximum eigenvalue
    lambda1_ = f1 * log(f2_ * (s1 + s2));

    // lambda2 is the exponent computed from the minimum eigenvalue
    lambda2_ = f1 * log(f2_ * (s1 - s2));

    // Compute the orientation theta1 and theta2
    // of the corresponding eigenvalue of \Delta
    
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

/*
  Details about the factorization of the eigenvalue of \Delta

   \Delta is a 2x2 symmetric matrix of the form
   [ a b ]
   [ b d ]
   whith
   a = a00^2 + a01^2
   b = a00*a10 + a01*a11
   d = a11^2 + a10^2

   On the first hand we have:
   
   Tr(\Delta)^2 - 4 det(\Delta) = ( a - d )^2 + 4 b^2
   = (a00^2 + a01^2 - a11^2 - a10^2 ) + 4 * (a00*a10 + a01*a11)^2


   On the second hand we have:

   s2^2
   = [ (a01 + a10)^2 + (a00 - a11)^2 ] * [ (a01 - a10)^2 + (a00 + a11)^2 ]
   = (a01 + a10)^2 * (a01 - a10)^2
   + (a01 + a10)^2 * (a00 + a11)^2
   + (a00 - a11)^2 * (a01 - a10)^2
   + (a00 - a11)^2 * (a00 + a11)^2

   = (a01^2 - a10^2)^2 // using (a-b)^2 = (a-b)*(a+b)
   + (a01 + a10)^2 * (a00 + a11)^2 + (a00 - a11)^2 * (a01 - a10)^2
   + (a00^2 - a11^2)^2 // using (a-b)^2 = (a-b)*(a+b)

   = (a00^2 + a01^2 - a11^2 - a10^2 ) - 2 * (a01^2 - a10^2) * (a00^2 - a11^2 ) // using a^2 + b^2 = (a+b)^2 - 2*a*b
   + (a01 + a10)^2 * (a00 + a11)^2 + (a00 - a11)^2 * (a01 - a10)^2

   = (a00^2 + a01^2 - a11^2 - a10^2 )
   - 2 * (a01 + a10 ) * (a01 - a10 ) * (a00 + a11 ) * (a00 - a11 )
   + (a01 + a10)^2 * (a00 + a11)^2 + (a00 - a11)^2 * (a01 - a10)^2

   We recall that 
   a = a00^2 + a01^2
   d = a11^2 + a10^2
   b = a00*a10 + a11*a01
   and we set
   f = a01 + a10
   g = a00 + a11
   h = a01 - a10
   m = a00 - a11

   we get

   s2^2 = (a-d)^2 - 2f*h*m*g + f^2*g^2 + m^2*h^2
   = (a-d)^2 + (f*g)^2 + (m*h)^2 - 2(f*g)*(m*h)
   = (a-d)^2 + (f*g - m*h)^2 

   and
   (f*g - m*h)^2 =  [ (a01+a10) * (a00+a11) - (a00-a11)*(a01-a10) ] ^ 2
   = [ 2 * (a00*a10 + a11*a01) ]^2 // after developpement 
   = 4 b^2

   Finally we get
   s2^2 = (a-d)^2 + 4 b^2 = Tr(\Delta)^2 + 4 det(\Delta)
   
*/
