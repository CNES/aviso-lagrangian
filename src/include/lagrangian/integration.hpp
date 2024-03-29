// This file is part of lagrangian library.
//
// lagrangian is free software: you can redistribute it and/or modify
// it under the terms of GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// lagrangian is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of GNU Lesser General Public License
// along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
#pragma once

// ___________________________________________________________________________//

#include <boost/date_time.hpp>
#include <boost/math/special_functions.hpp>
#include <limits>

// ___________________________________________________________________________//

#include "lagrangian/datetime.hpp"
#include "lagrangian/field.hpp"
#include "lagrangian/runge_kutta.hpp"
#include "lagrangian/stencil.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

// ___________________________________________________________________________//

/**
 * @brief Handles the time integration
 */
class Integration {
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
  Integration(const DateTime& start_time, const DateTime& end_time,
              const boost::posix_time::time_duration& delta_t, Field* field)
      : size_of_interval_(delta_t.total_microseconds() * 1e-6),
        field_(field),
        start_time_(start_time.ToUnixTime()),
        end_time_(end_time.ToUnixTime()),
        rk_(size_of_interval_ * (start_time_ > end_time_ ? -1 : 1), field_) {
    if (size_of_interval_ < 0) {
      throw std::runtime_error("Time delta must be positive");
    }
  }

  /**
   * @brief Default method invoked when an instance is destroyed.
   */
  virtual ~Integration() = default;

  /**
   * @brief Return an iterator that describes the integration period
   *
   * @return The iterator
   */
  [[nodiscard]] auto GetIterator() const -> Iterator {
    return {start_time_, end_time_, size_of_interval_};
  }

  /**
   * Perform the tasks before a new time step (eg load grids required)
   *
   * @param t Time step in seconds
   */
  void Fetch(const double t) const {
    start_time_ < end_time_ ? field_->Fetch(t, t + size_of_interval_)
                            : field_->Fetch(t, t - size_of_interval_);
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
  inline auto Compute(const Iterator& it, const double x0, const double y0,
                      double& x1, double& y1) const -> bool {
    return rk_.Compute(it(), x0, y0, x1, y1);
  }

  /**
   * Gets the field used for computing the velocity of a point.
   *
   * @return the velocity field used.
   */
  [[nodiscard]] auto get_field() const -> const Field* { return field_; }

  /**
   * @brief Gets the Runge-Kutta object
   */
  [[nodiscard]] auto get_rk4() const -> RungeKutta const& { return rk_; }

  /**
   * @brief Gets start time of the integration
   */
  [[nodiscard]] auto get_start_time() const -> double { return start_time_; }

 protected:
  double size_of_interval_;  //!< Integration time in number of seconds
  Field* field_;             //!< Field used to compute the velocity
  double start_time_;        //!< Start time of the integration
  double end_time_;          //!< End time of the integration
  RungeKutta rk_;            //!< Runge-Kutta object
};

// ___________________________________________________________________________//

/**
 * @brief Handles the movement of a particle using the Runge-Kutta method.
 */
class Path : public Integration {
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
  Path(const DateTime& start_time, const DateTime& end_time,
       const boost::posix_time::time_duration& delta_t, Field* field)
      : Integration(start_time, end_time, delta_t, field) {}

  /**
   * @brief Default method invoked when an instance is destroyed.
   */
  ~Path() override = default;
};

// ___________________________________________________________________________//

/**
 * @brief %Index a matrix cell
 */
class Index {
 private:
  int i_{}, j_{};

 public:
  /**
   * @brief Default constructor
   */
  Index() = default;

  /**
   * @brief Move constructor
   */
  Index(Index&& rhs) = default;

  /**
   * @brief Move assignment operator
   */
  auto operator=(Index&& rhs) -> Index& = default;

  /**
   * @brief Construct a new object defining an index
   *
   * @param i %Index i
   * @param j %Index j
   */
  Index(const int i, const int j) : i_(i), j_(j) {}

  /**
   * @brief Get the index i
   *
   * @return The nth i
   */
  [[nodiscard]] inline auto get_i() const -> int { return i_; }

  /**
   * @brief Get the index j
   *
   * @return The nth j
   */
  [[nodiscard]] inline auto get_j() const -> double { return j_; }
};

// ___________________________________________________________________________//

/**
 * Storing Lyapunov coefficients calculated.
 *
 * @see FiniteLyapunovExponentsIntegration
 */
class FiniteLyapunovExponents {
 public:
  /**
   * @brief Set the effective advection time
   *
   * @param delta_t The advection time (unit number of seconds elapsed since
   * the beginning of the integration)
   */
  inline void set_delta_t(const double delta_t) { delta_t_ = delta_t; }

  /**
   * @brief Get the actual advection time
   *
   * @return The advection time (unit number of seconds elapsed since the
   * beginning of the integration)
   */
  [[nodiscard]] inline auto get_delta_t() const -> double { return delta_t_; }

  /**
   * @brief Get the effective advection time (delta_t) in case of undefined FLE
   * @details FSLE and FTLE are undefined when stencil has not been advected
   * FSLE is also undefined when stencil has not been separated
   * from the final separation distance before maximum advection time
   * This value serve as diagnostic.
   *
   * @return The effective advection time (unit number of seconds elapsed since
   * the beginning of the integration)
   */
  [[nodiscard]] inline auto GetUndefinedDeltaT() const -> double {
    return delta_t_;
  }

  /**
   * @brief Set the FLE associated to the maximum eigenvalue of Cauchy-Green
   * strain tensor
   *
   * @param lambda1 λ₁ (unit 1/sec)
   */
  inline void set_lambda1(const double lambda1) { lambda1_ = lambda1; }

  /**
   * @brief Get the FLE associated to the maximum eigenvalue of the Cauchy-Green
   * strain tensor
   *
   * @return λ₁ (unit 1/sec)
   */
  [[nodiscard]] inline auto get_lambda1() const -> double { return lambda1_; }

  /**
   * @brief Set the FLE associated to the minimum eigenvalue of the Cauchy-Green
   * strain tensor
   *
   * @param lambda1 λ₂ (unit 1/sec)
   */
  inline void set_lambda2(const double lambda2) { lambda2_ = lambda2; }

  /**
   * @brief Get the FLE associated to the minimum eigenvalue of the Cauchy-Green
   * strain tensor
   *
   * @return λ₂ (unit 1/sec)
   */
  [[nodiscard]] inline auto get_lambda2() const -> double { return lambda2_; }

  /**
   * @brief Get missing value of FLE in case of undefined FLE
   * @details FSLE and FTLE are undefined when stencil has not been advected
   * FSLE is also undefined when stencil has not been separated
   * from the final separation distance before the maximum advection time
   * This value may serve as diagnostic together with effective adection time
   * information
   *
   * @return default value (unit 1/sec)
   */
  [[nodiscard]] inline auto GetUndefinedExponent() const -> double {
    return 0.0;
  }

  /**
   * @brief Set the orientation of the eigenvector associated to the maximum
   * eigenvalue of the Cauchy-Green strain tensor
   *
   * @param theta1 θ₁ (unit degrees)
   */
  inline void set_theta1(const double theta1) { theta1_ = theta1; }

  /**
   * @brief Get the orientation of the eigenvector associated to the maximum
   * eigenvalue of the Cauchy-Green strain tensor
   *
   * @return θ₁ (unit degrees)
   */
  [[nodiscard]] inline auto get_theta1() const -> double { return theta1_; }

  /**
   * @brief Set the orientation of the eigenvector associated to the minimum
   * eigenvalue of the Cauchy-Green strain tensor
   *
   * @param theta2 θ₂ (unit degrees)
   */
  inline void set_theta2(const double theta2) { theta2_ = theta2; }

  /**
   * @brief Get the orientation of the eigenvector associated to the minimum
   * eigenvalue of the Cauchy-Green strain tensor
   *
   * @return θ₂ (unit degrees)
   */
  [[nodiscard]] inline auto get_theta2() const -> double { return theta2_; }

  /**
   * @brief Get the missing value of the orientation ot the eigenvectors of the
   * Cauchy-Green strain tensor in case of undefined FLE
   * @details FSLE and FTLE are undefined when stencil has not been advected
   * FSLE is also undefined when stencil has not been separated
   * from the final separation distance before the maximum advection time
   * This value may serve as diagnostic together with effective adection time
   * information
   *
   * @return default value (unit degrees)
   */
  [[nodiscard]] inline auto GetUndefinedVector() const -> double { return 0.0; }

  /**
   * @brief Set the final separation distance
   *
   * @param separation The final separation distance (unit degree)
   */
  inline void set_final_separation(const double effective_separation) {
    final_separation_ = effective_separation;
  }

  /**
   * @brief Get the final separation distance
   *
   * @return the final separation distance (unit degree)
   */
  [[nodiscard]] inline auto get_final_separation() const -> double {
    return final_separation_;
  }

  /**
   * @brief Get the missing value of the final separation distance in case of
   * undefined FLE
   * @details FSLE and FTLE are undefined when stencil has not been advected
   * FSLE is also undefined when stencil has not been separated
   * from the final separation distance before the maximum advection time
   * missing value is the to the effective separation distance when advection is
   * stopped This value may serve as diagnostic together with effective
   * advection time information
   *
   * @return the missing value of final separation distance (unit degree)
   */
  [[nodiscard]] inline auto GetUndefinedFinalSeparation() const -> double {
    return final_separation_;
  }

  /**
   * Set Lyapunov coefficients to NaN when they are undefined.
   */
  inline void NaN() {
    lambda1_ = lambda2_ = theta1_ = theta2_ =
        std::numeric_limits<double>::quiet_NaN();
  }

 private:
  double delta_t_;
  double final_separation_;
  double lambda1_;
  double lambda2_;
  double theta1_;
  double theta2_;
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
class FiniteLyapunovExponentsIntegration : public Integration {
 public:
  /**
   * @brief Mode of integration
   */
  enum Mode {
    kFSLE,  //!< Finite Size Lyapunov Exponent
    kFTLE   //!< Finite Time Lyapunov Exponent
  };

  /**
   * @brief Type of stencil known
   */
  enum Stencil {
    kTriplet,    //!< kTriplet
    kQuintuplet  //!< kQuintuplet
  };

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
  FiniteLyapunovExponentsIntegration(
      const DateTime& start_time, const DateTime& end_time,
      const boost::posix_time::time_duration& delta_t, const Mode mode,
      const double min_separation, const double delta, Field* field)
      : Integration(start_time, end_time, delta_t, field),
        delta_(delta),
        mode_(mode),
        f2_(0.5 * (1 / (delta_ * delta_))) {
    switch (mode_) {
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
  ~FiniteLyapunovExponentsIntegration() override = default;

  /**
   * @brief Set the value of the initial point
   *
   * @param x Longitude
   * @param y Latitude
   * @param stencil Type of stencil used
   * @param spherical_equatorial True if the coordinates system is Lon/lat
   * otherwise false
   *
   * @return The position of the initial point
   */
  [[nodiscard]] inline auto SetInitialPoint(
      const double x, const double y, const Stencil stencil,
      const bool spherical_equatorial) const -> Position* {
    switch (stencil) {
      case kTriplet:
        return new Triplet(x, y, delta_, start_time_, spherical_equatorial);
      case kQuintuplet:
        return new Quintuplet(x, y, delta_, start_time_, spherical_equatorial);
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
  inline auto Separation(const Position* const position) const -> bool {
    return (this->*pSeparation_)(position);
  }

  /**
   * @brief Get mode of integration
   *
   * @return The mode of the integration
   */
  [[nodiscard]] inline auto get_mode() const -> Mode { return mode_; }

  /**
   * @brief Calculate the integration
   *
   * @param it %Iterator
   * @param position %Position of the particle
   * @param cell Cell properties of the grid used for the interpolation
   *
   * @return True if the integration is defined otherwise false
   */
  inline auto Compute(const Iterator& it, Position* const position,
                      CellProperties& cell) const -> bool {
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
  auto ComputeExponents(const Position* position, FiniteLyapunovExponents& fle)
      -> bool;

 private:
  using SeparationFunction = bool (FiniteLyapunovExponentsIntegration::*)(
      const Position* const p) const;

  const double delta_;
  double min_separation_;
  Mode mode_;
  SeparationFunction pSeparation_;
  double f2_;

  inline auto SeparationFSLE(const Position* const position) const -> bool {
    return position->MaxDistance() > min_separation_;
  }

  inline auto SeparationFTLE(const Position* const /*unused*/) const -> bool {
    return false;
  }
};

}  // namespace lagrangian
