/*
  This file is part of lagrangian library.

  lagrangian is free software: you can redistribute it and/or modify
  it under the terms of GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  lagrangian is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of GNU Lesser General Public License
  along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// ___________________________________________________________________________//

#include <limits>
#include <stdexcept>
#include <vector>

// ___________________________________________________________________________//

#include "lagrangian/misc.hpp"
#include "lagrangian/runge_kutta.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Definition of an iterator over a time period
 */
class Iterator {
 private:
  double end_;
  double inc_;
  double ix_;

 public:
  /**
   * @brief Default constructor
   *
   * @param begin Begin of the period expressed in number of seconds
   * elapsed since 1970
   * @param end End of the period expressed in number of seconds
   * elapsed since 1970
   * @param inc Time increment in seconds
   */
  Iterator(const double begin, const double end, const double inc)
      : end_(end), inc_(begin > end ? -inc : inc), ix_(begin) {}

  /**
   * @brief Test whether there is still a time step to cross into the time
   * interval defined.
   *
   * @return True if the path in the time interval is not complete.
   */
  inline bool GoAfter() const { return inc_ > 0 ? ix_ <= end_ : ix_ >= end_; }

  /**
   * @brief Move to the next time step.
   */
  inline void operator++() { ix_ += inc_; }

  /**
   * @brief Get the current time in the time interval defined
   *
   * @return The current time expressed in number of seconds elapsed since
   * 1970
   */
  inline double operator()() const { return ix_; }

  /**
   * @brief Get the time step defined
   *
   * @return The time step expressed in seconds
   */
  inline double inc() const { return inc_; }
};

// ___________________________________________________________________________//

/**
 * @brief Define the position of N points Mᵢ = (xᵢ, yᵢ)
 *
 * <PRE>
 *            Mᵢ₊₁
 *            |
 *    Mᵢ₊ᵢ ⎯⎯ M₀ ⎯⎯  Mᵢ
 *            |
 *            Mᵢ₊ₙ
 * </PRE>
 */
class Position {
 protected:
  /// Abscissas of the point
  std::vector<double> x_;

  /// Ordinates of the point
  std::vector<double> y_;

  /// Integration time (number of seconds elapsed since 1970)
  double time_{0};

  /// Indicate whether the integration is over or not
  bool completed_{false};

 private:
  /**
   * @brief Update the position
   *
   * @param time Time step (number of seconds elapsed since 1970)
   * @param x Longitude of points
   * @param y Latitude of points
   */
  inline void Update(const double time, const std::vector<double>& x,
                     const std::vector<double>& y) {
    x_ = x;
    y_ = y;
    time_ = time;
  }

 public:
  /**
   * @brief Default constructor
   * @details this constructor may lead to an error in diagnostic
   * because start time is set to zero: if stencil has not been advected then
   * delta_t is equal to minus advection starting time (- start_time_) which is
   * not correct. (see FiniteLyapunovExponentsIntegration::ComputeExponents in
   * integration.cpp)
   */
  Position() = default;

  /**
   * @brief Constructor with start_time setting
   *
   */
  Position(const double start_time)
      : x_(), y_(), time_(start_time) {}

  /**
   * Move constructor
   *
   * @param rhs right value
   */
  Position(Position&& rhs) = default;

  /**
   * Move assignment operator
   *
   * @param rhs right value
   */
  Position& operator=(Position&& rhs) = default;

  /**
   * @brief Destructor
   */
  virtual ~Position() = default;

  /**
   * @brief Get the longitude of the point \#idx
   *
   * @return The longitude in degrees
   */
  inline double get_xi(const size_t idx) const { return x_.at(idx); }

  /**
   * @brief Get the latitude of the point \#idx
   *
   * @return The latitude in degrees
   */
  inline double get_yi(const size_t idx) const { return y_.at(idx); }

  /**
   * @brief Get the time at the end of the integration
   *
   * @return The time expressed in number of seconds elapsed since 1970
   */
  inline double get_time() const { return time_; }

  /**
   * @brief Test if the integration is over
   *
   * @return True if the integration is over
   */
  inline bool get_completed() const { return completed_; }

  /**
   * @brief Indicate that the integration is complete.
   */
  inline void set_completed() { completed_ = true; }

  /**
   * @brief Set the instance to represent a missing position.
   */
  inline void Missing() {
    x_.clear();
    y_.clear();
  }

  /**
   * @brief Test if the integration is defined.
   *
   * @return True if the integration is defined.
   */
  inline bool IsMissing() { return x_.empty() && y_.empty(); }

  /**
   * @brief Compute the distance max
   *
   * @return The max distance
   */
  inline double MaxDistance() const {
    double result = 0;

    for (size_t idx = 1; idx < x_.size(); ++idx) {
      double distance = Distance(x_[0], y_[0], x_[idx], y_[idx]);
      if (distance > result) result = distance;
    }
    return result;
  }

  /**
   * @brief To move a particle with a velocity field.
   *
   * @param rk Runge-Kutta handler
   * @param it Iterator
   * @param cell Cell properties of the grid used for the interpolation.
   *
   * @return True if the particle could be moved otherwise false
   */
  bool Compute(const RungeKutta& rk, const Iterator& it, CellProperties& cell) {
    std::vector<double> x(x_.size());
    std::vector<double> y(y_.size());

    for (size_t ix = 0; ix < x_.size(); ++ix) {
      if (!rk.Compute(it(), x_.at(ix), y_.at(ix), x[ix], y[ix], cell))
        return false;
    }
    Update(it(), x, y);
    return true;
  }

  /**
   * @brief TODO
   *
   * @param a00
   * @param a01
   * @param a10
   * @param a11
   */
  virtual void StrainTensor(double& a00, double& a01, double& a10,
                            double& a11) const {
    // make clang-check happy
    a00 = a01 = a10 = a11 = std::numeric_limits<double>::quiet_NaN();
  }
};

/**
 * @brief Define the position of 3 points
 */
class Triplet : public Position {
 public:
  /**
   * Default constructor
   */
  Triplet() : Position() {}

  /**
   * @brief Construct a new object defining the position of the N points
   *
   * @param x Longitude of the initial point
   * @param y Latitude of the initial point
   * @param delta Initial separation in degrees of neighboring
   * @param start_time Advection starting time
   *  particles
   */
  Triplet(const double x, const double y, const double delta,
          const double start_time)
      : Position(start_time) {
    x_.push_back(x);
    x_.push_back(x + delta);
    x_.push_back(x);
    y_.push_back(y);
    y_.push_back(y);
    y_.push_back(y + delta);
  }

  /**
   * Move constructor
   *
   * @param rhs right value
   */
  Triplet(Triplet&& rhs) = default;

  /**
   * Move assignment operator
   *
   * @param rhs right value
   */
  Triplet& operator=(Triplet&& rhs) = default;

  /**
   * @brief TODO
   *
   * @param a00
   * @param a01
   * @param a10
   * @param a11
   */
  inline void StrainTensor(double& a00, double& a01, double& a10,
                           double& a11) const override {
    a00 = x_[1] - x_[0];
    a01 = x_[2] - x_[0];
    a10 = y_[1] - y_[0];
    a11 = y_[2] - y_[0];
  }
};

/**
 * @brief Define the position of 5 points
 */
class Quintuplet : public Position {
 public:
  /**
   * Default constructor
   */
  Quintuplet() : Position() {}

  /**
   * @brief Construct a new object defining the position of the N points
   *
   * @param x Longitude of the initial point
   * @param y Latitude of the initial point
   * @param delta Initial separation in degrees of neighboring
   * @param start_time Advection starting time
   *  particles
   */
  Quintuplet(const double x, const double y, const double delta,
             const double start_time)
      : Position(start_time) {
    x_.push_back(x);
    x_.push_back(x + delta);
    x_.push_back(x);
    x_.push_back(x - delta);
    x_.push_back(x);

    y_.push_back(y);
    y_.push_back(y);
    y_.push_back(y + delta);
    y_.push_back(y);
    y_.push_back(y - delta);
  }

  /**
   * Move constructor
   *
   * @param rhs right value
   */
  Quintuplet(Quintuplet&& rhs) = default;

  /**
   * Move assignment operator
   *
   * @param rhs right value
   */
  Quintuplet& operator=(Quintuplet&& rhs) = default;

  /**
   * @brief TODO
   *
   * @param a00
   * @param a01
   * @param a10
   * @param a11
   */
  inline void StrainTensor(double& a00, double& a01, double& a10,
                           double& a11) const override {
    a00 = x_[1] - x_[3];
    a01 = x_[2] - x_[4];
    a10 = y_[1] - y_[3];
    a11 = y_[2] - y_[4];
  }
};
}  // namespace lagrangian
