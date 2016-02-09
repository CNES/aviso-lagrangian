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

#include <boost/date_time.hpp>
#include <boost/math/special_functions.hpp>
#include <limits>

// ___________________________________________________________________________//

#include "lagrangian/field.hpp"
#include "lagrangian/datetime.hpp"
#include "lagrangian/runge_kutta.hpp"
#include "lagrangian/stencil.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

// ___________________________________________________________________________//

/**
 * @brief Handles the time integration
 */
class Integration
{
protected:
    double size_of_interval_;   //!< Integration time in number of seconds
    Field* field_;              //!< Field used to compute the velocity
    double start_time_;         //!< Start time of the integration
    double end_time_;           //!< End time of the integration
    RungeKutta rk_;             //!< Runge-Kutta object

public:

    /**
     * @brief Default constructor
     *
     * @param start_time Start time of the integration (number of seconds
     * elapsed 1970)
     * @param end_time End date of the integration (number of seconds
     * elapsed 1970)
     * @param delta_t Time interval, in seconds
     * @param field Field to use for computing the velocity of a point.
     *
     * @throw std::runtime_error if the time interval is negative
     */
    Integration(const DateTime& start_time,
            const DateTime& end_time,
            const boost::posix_time::time_duration& delta_t,
            Field* field) :
            size_of_interval_(delta_t.total_seconds()), field_(field), start_time_(
                    start_time.ToUnixTime()), end_time_(end_time.ToUnixTime()), rk_(
                    size_of_interval_ * (start_time_ > end_time_ ? -1 : 1),
                    field_)
    {
        if (size_of_interval_ < 0)
            throw std::runtime_error("Time delta must be positive");
    }

    /**
     * @brief Default method invoked when an instance is destroyed.
     */
    virtual ~Integration()
    {
    }

    /**
     * @brief Return an iterator that describes the integration period
     *
     * @return The iterator
     */
    Iterator GetIterator() const
    {
        return Iterator(start_time_, end_time_, size_of_interval_);
    }

    /**
     * Perform the tasks before a new time step (eg load grids required)
     *
     * @param t Time step in seconds
     */
    void Fetch(const double t) const
    {
        start_time_ < end_time_ ?
                field_->Fetch(t, t + size_of_interval_) :
                field_->Fetch(t, t - size_of_interval_);
    }

    /**
     * @brief Calculate the new position of the particle
     *
     * @param it Iterator
     * @param x0 Longitude in degrees
     * @param y0 Latitude in degrees
     * @param x1 New longitude in degrees
     * @param y1 New latitude in degrees
     *
     * @return true if the new position is defined otherwise false.
     */
     inline bool Compute(const Iterator& it,
            const double x0,
            const double y0,
            double& x1,
            double& y1) const
    {
        return rk_.Compute(it(), x0, y0, x1, y1);
    }
};

// ___________________________________________________________________________//

/**
 * @brief Handles the movement of a particle using the Runge-Kutta method.
 */
class Path: public Integration
{
public:

    /**
     * @brief Default constructor
     *
     * @param start_time Start time of the integration (number of seconds
     * elapsed 1970)
     * @param end_time End date of the integration (number of seconds
     * elapsed 1970)
     * @param delta_t Time interval, in seconds
     * @param field Field to use for computing the velocity of a point.
     */
    Path(const DateTime& start_time,
            const DateTime& end_time,
            const boost::posix_time::time_duration& delta_t,
            Field* field) :
            Integration(start_time, end_time, delta_t, field)
    {
    }

    /**
     * @brief Default method invoked when an instance is destroyed.
     */
    virtual ~Path()
    {
    }
};

// ___________________________________________________________________________//

/**
 * @brief %Index a matrix cell
 */
class Index
{
private:
    int i_, j_;

public:

    /**
     * @brief Default constructor
     */
    Index() :
            i_(), j_()
    {
    }

    /**
     * @brief Construct a new object defining an index
     *
     * @param i %Index i
     * @param j %Index j
     */
    Index(const int i, const int j) :
            i_(i), j_(j)
    {
    }

    /**
     * @brief Get the index i
     *
     * @return The nth i
     */
    inline int get_i() const
    {
        return i_;
    }

    /**
     * @brief Get the index j
     *
     * @return The nth j
     */
    inline double get_j() const
    {
        return j_;
    }
};

// ___________________________________________________________________________//

/**
 * Storing Lyapunov coefficients calculated.
 *
 * @see FiniteLyapunovExponentsIntegration
 */
class FiniteLyapunovExponents
{
private:
    double delta_t_;
	double final_separation_;
    double lambda1_;
    double lambda2_;
    double theta1_;
    double theta2_;
public:
    /**
     * @brief Set the effective advection time
     *
     * @param delta_t The advection time (unit number of seconds elapsed since
     * the beginning of the integration)
     */
    inline void set_delta_t(const double delta_t)
    {
    	delta_t_ = delta_t;
    }

    /**
     * @brief Get the actual advection time
     *
     * @return The advection time (unit number of seconds elapsed since the
     * beginning of the integration)
     */
    inline double get_delta_t() const
    {
    	return delta_t_;
    }

    /**
     * @brief Set the orientation of the eigenvectors associated to the
     * maximum eigenvalues of Cauchy-Green strain tensor
     *
     * @param lambda1 λ₁ (unit 1/day)
     */
    inline void set_lambda1(const double lambda1)
    {
    	lambda1_ = lambda1;
    }

    /**
     * @brief Get the orientation of the eigenvectors associated to the
     * maximum eigenvalues of Cauchy-Green strain tensor
     *
     * @return λ₁ (unit 1/day)
     */
    inline double get_lambda1() const
    {
    	return lambda1_;
    }

    /**
     * @brief Set the orientation of the eigenvectors associated to the
     * minimum eigenvalues of Cauchy-Green strain tensor
     *
     * @param lambda1 λ₂ (unit 1/day)
     */
    inline void set_lambda2(const double lambda2)
    {
    	lambda2_ = lambda2;
    }

    /**
     * @brief Get the orientation of the eigenvectors associated to the
     * minimum eigenvalues of Cauchy-Green strain tensor
     *
     * @return λ₂ (unit 1/day)
     */
    inline double get_lambda2() const
    {
    	return lambda2_;
    }

    /**
     * @brief Set FLE associated to the maximum eigenvalues of Cauchy-Green
     * strain tensor
     *
     * @param theta1 θ₁ (unit degrees)
     */
    inline void set_theta1(const double theta1)
    {
    	theta1_ = theta1;
    }

    /**
     * @brief Get FLE associated to the maximum eigenvalues of Cauchy-Green
     * strain tensor
     *
     * @return θ₁ (unit degrees)
     */
    inline double get_theta1() const
    {
    	return theta1_;
    }

    /**
     * @brief Set FLE associated to the maximum eigenvalues of Cauchy-Green
     * strain tensor
     *
     * @param theta2 θ₂ (unit degrees)
     */
    inline void set_theta2(const double theta2)
    {
    	theta2_ = theta2;
    }

    /**
     * @brief Get FLE associated to the maximum eigenvalues of Cauchy-Green
     * strain tensor
     *
     * @return θ₂ (unit degrees)
     */
    inline double get_theta2() const
    {
    	return theta2_;
    }

    /**
     * @brief Set the final separation distance
     *
     * @param separation The final separation distance (unit degree)
     */
    inline void set_final_separation(const double effective_separation)
    {
    	final_separation_ = effective_separation;
    }

    /**
     * @brief Get the final separation distance
     *
     * @return the final separation distance (unit degree)
     */
    inline double get_final_separation() const
    {
    	return final_separation_;
    }

    /**
     * Set Lyapunov coefficients to NaN when they are undefined.
     */
    inline void NaN()
    {
		lambda1_ = lambda2_ = theta1_ = theta2_ =
		        std::numeric_limits<double>::quiet_NaN();
    }
};

/**
 * @brief Handles the computation of Lyapunov Exponent
 *
 * Finite Size Lyapunov Exponent (FSLE) is a scalar local notion that
 * represents the rate of separation of initially neighbouring particles
 * over a finite-time window [t₀, t₀ + T], where T is the time two
 * particules need to be advected in order to be separated from a given
 * distance d.
 *
 * Let x(t) = x(t; x₀, t₀) be the position of a lagrangian particle
 * at time t, started at x₀ at t=t₀ and advected by the time-dependent
 * fluid flow u(x, t).
 *
 * The Forward Finite-Time Lyapunov Exponent at a point x₀
 * and for the advection time T is defined as the growth factor of
 * the norm of the perturbation dx0 started around x₀ and advected
 * by the flow after the finite advection time T.
 *
 * Maximal stretching occurs when dx0 is aligned with the eigenvector
 * associated with the maximum eigenvalue δmax of the Cauchy-Green strain
 * tensor Δ:
 *
 * Δ = [ ∇Φ₀ᵀ (x₀) ]^* [ ∇Φ₀ᵀ (x₀) ]
 *
 * where Φ₀ᵀ : x₀ ➜ x(t, x₀, t₀) is the flow map of the advection equation:
 * it links the location x₀ of a lagragian particule at t=t₀ to its position
 * x(t,x₀,t₀) at time t. (* denotes the transposition operator).
 *
 * FTLE is defined as
 *
 * σ = ( 1 / (2*T) ) * log( λmax( Δ ) )
 *
 * Finite-Size Lyapunov Exponent is similary defined: T is choosen so that
 * neighbouring particules separate from a given distance d.
 *
 * ComputeExponents(const Position& position) function implements the
 * computation of the Lyapunov exponents based on maximal and minimal
 * eigenvalues and orientation of eigenvectors of Δ given the elements of
 * ∇Φ₀ᵀ matrix.
 *
 * For more details see:
 *
 * 1. G. Haller, Lagrangian coherent structures and the rate of strain in
 * two-dimensional turbulence Phys. Fluids A 13 (2001) 3365-3385
 * (http://georgehaller.com/reprints/approx.pdf)
 * Remark: In this paper, FTLE is referred to as the Direct Lyapunov
 * Exponent (DLE)
 *
 * 2. http://mmae.iit.edu/shadden/LCS-tutorial/FTLE-derivation.html
 */
class FiniteLyapunovExponentsIntegration: public Integration
{
public:
    /**
     * @brief Mode of integration
     */
    enum Mode
    {
        kFSLE,  //!< Finite Size Lyapunov Exponent
        kFTLE   //!< Finite Time Lyapunov Exponent
    };

    /**
     * @brief Type of stencil known
     */
    enum Stencil
    {
        kTriplet,    //!< kTriplet
        kQuintuplet  //!< kQuintuplet
    };

private:
    typedef bool
    (FiniteLyapunovExponentsIntegration::*SeparationFunction)(const Position* const p) const;

    const double delta_;
    double min_separation_;
    Mode mode_;
    SeparationFunction pSeparation_;
    double f2_;

    inline bool SeparationFSLE(const Position* const position) const
    {
        return position->MaxDistance() > min_separation_;
    }

    inline bool SeparationFTLE(const Position* const) const
    {
        return false;
    }

public:

    /**
     * @brief Default constructor
     *
     * @param start_time Start time of the integration
     * @param end_time End date of the integration
     * @param delta_t Time interval
     * @param mode %Integration mode
     * @param min_separation Minimal separation in degrees
     * @param delta The gap between two consecutive dots, in degrees, of the
     * grid
     * @param field %Field to use for computing the velocity of a point.
     *
     * @throw std::invalid_argument if the mode of integration is unknown.
     */
    FiniteLyapunovExponentsIntegration(const DateTime& start_time,
            const DateTime& end_time,
            const boost::posix_time::time_duration& delta_t,
            const Mode mode,
            const double min_separation,
            const double delta,
            Field* field) :
            Integration(start_time, end_time, delta_t, field), delta_(delta), mode_(
                    mode), f2_(0.5 * (1 / (delta_ * delta_)))
    {
        switch (mode_)
        {
        case kFSLE:
            min_separation_ = min_separation;
            pSeparation_ = &FiniteLyapunovExponentsIntegration::SeparationFSLE;
            break;
        case kFTLE:
            min_separation_ = -1;
            pSeparation_ = &FiniteLyapunovExponentsIntegration::SeparationFTLE;
            break;
        default:
            throw std::invalid_argument(
                    "invalid FiniteLyapunovExponents::Mode computation");
        }
    }

    /**
     * @brief Default method invoked when an instance is destroyed.
     */
    virtual ~FiniteLyapunovExponentsIntegration()
    {
    }

    /**
     * @brief Set the value of the initial point
     *
     * @param x Longitude
     * @param y Latitude
     * @param stencil Type of stencil used
     *
     * @return The position of the initial point
     */
    inline Position* SetInitialPoint(const double x,
            const double y,
            const Stencil stencil) const
    {
        switch (stencil)
        {
        case kTriplet:
            return new Triplet(x, y, delta_);
        case kQuintuplet:
            return new Quintuplet(x, y, delta_);
        default:
            throw std::invalid_argument(
                    "invalid FiniteLyapunovExponents::Stencil type");
        }
    }

    /**
     * @brief Determine whether the particle is deemed to be separate
     *
     * @param position %Position of the particle
     *
     * @return True if the particle is separated.
     */
    inline bool Separation(const Position* const position) const
    {
        return (this->*pSeparation_)(position);
    }

    /**
     * @brief Get mode of integration
     *
     * @return The mode of the integration
     */
    inline Mode get_mode() const
    {
        return mode_;
    }

    /**
     * @brief Calculate the integration
     *
     * @param it %Iterator
     * @param position %Position of the particle
     * @param cell Cell properties of the grid used for the interpolation
     *
     * @return True if the integration is defined otherwise false
     */
    inline bool Compute(const Iterator& it,
            Position* const position,
            CellProperties& cell) const
    {
        return position->Compute(rk_, it, cell);
    }

    /**
     * @brief Compute the eigenvalue and the orientation of the eigenvectors
     * of the Cauchy-Green strain tensor
     *
     * @param position %Position of the particle
     * @param fle Finite Lyapunov Exponents computed
     *
     * @return True if the exponents are defined
     */
	bool ComputeExponents(const Position* const position,
	        FiniteLyapunovExponents& fle);
};

} // namespace lagrangian
