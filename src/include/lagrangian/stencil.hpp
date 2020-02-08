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
  [[nodiscard]] inline auto GoAfter() const -> bool {
    return inc_ > 0 ? ix_ <= end_ : ix_ >= end_;
  }

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
  inline auto operator()() const -> double { return ix_; }

  /**
   * @brief Get the time step defined
   *
   * @return The time step expressed in seconds
   */
  [[nodiscard]] inline auto inc() const -> double { return inc_; }

 private:
  double end_;
  double inc_;
  double ix_;
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
 *            Mᵢ
 * </PRE>
 */
class Position {
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
   * @param start_time Advection starting time particles
   * @param spherical_equatorial True if the coordinates system is Lon/lat
   * otherwise false
   */
  explicit Position(const double start_time, const bool spherical_equatorial)
      : time_(start_time),
        pDistance_(spherical_equatorial ? &GeodeticDistance : &Distance) {}

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
  auto operator=(Position&& rhs) -> Position& = default;

  /**
   * @brief Destructor
   */
  virtual ~Position() = default;

  /**
   * @brief Get the number of positions handled by this instance
   *
   * @return The number of positions
   */
  [[nodiscard]] inline auto size() const -> size_t { return x_.size(); }

  /**
   * @brief Get the longitude of the point \#idx
   *
   * @return The longitude in degrees
   */
  [[nodiscard]] inline auto get_xi(const size_t idx) const -> double {
    return x_.at(idx);
  }

  /**
   * @brief Get the latitude of the point \#idx
   *
   * @return The latitude in degrees
   */
  [[nodiscard]] inline auto get_yi(const size_t idx) const -> double {
    return y_.at(idx);
  }

  /**
   * @brief Get the time at the end of the integration
   *
   * @return The time expressed in number of seconds elapsed since 1970
   */
  [[nodiscard]] inline auto get_time() const -> double { return time_; }

  /**
   * @brief Test if the integration is over
   *
   * @return True if the integration is over
   */
  [[nodiscard]] inline auto is_completed() const -> bool { return completed_; }

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
  inline auto IsMissing() -> bool { return x_.empty() && y_.empty(); }

  /**
   * @brief Compute the distance max
   *
   * @return The max distance
   */
  [[nodiscard]] inline auto MaxDistance() const -> double {
    double result = 0;

    for (size_t idx = 1; idx < x_.size(); ++idx) {
      double distance = (*pDistance_)(x_[0], y_[0], x_[idx], y_[idx]);
      if (distance > result) {
        result = distance;
      }
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
  auto Compute(const RungeKutta& rk, const Iterator& it, CellProperties& cell) -> bool {
    std::vector<double> x(x_.size());
    std::vector<double> y(y_.size());

    for (size_t ix = 0; ix < x_.size(); ++ix) {
      if (!rk.Compute(it(), x_.at(ix), y_.at(ix), x[ix], y[ix], cell)) {
        return false;
      }
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

 protected:
  /// Distance calculation function
  using DistanceCalculator = double (*)(const double x0, const double x1,
                                        const double y0, const double y1);

  /// Abscissas of the point
  std::vector<double> x_;

  /// Ordinates of the point
  std::vector<double> y_;

  /// Integration time (number of seconds elapsed since 1970)
  double time_{0};

  /// Indicate whether the integration is over or not
  bool completed_{false};

  /// Function used to calculate distance
  DistanceCalculator pDistance_{&GeodeticDistance};

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
};

/**
 * @brief Define the position of 3 points
 */
class Triplet : public Position {
 public:
  /**
   * Default constructor
   */
  Triplet() = default;

  /**
   * @brief Construct a new object defining the position of the N points
   *
   * @param x Longitude of the initial point
   * @param y Latitude of the initial point
   * @param delta Initial separation in degrees of neighboring
   * @param start_time Advection starting time particles
   * @param spherical_equatorial True if the coordinates system is Lon/lat
   * otherwise false
   */
  Triplet(const double x, const double y, const double delta,
          const double start_time, const bool spherical_equatorial)
      : Position(start_time, spherical_equatorial) {
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
  auto operator=(Triplet&& rhs) -> Triplet& = default;

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
  Quintuplet() = default;

  /**
   * @brief Construct a new object defining the position of the N points
   *
   * @param x Longitude of the initial point
   * @param y Latitude of the initial point
   * @param delta Initial separation in degrees of neighboring
   * @param start_time Advection starting time particles
   * @param spherical_equatorial True if the coordinates system is Lon/lat
   * otherwise false
   */
  Quintuplet(const double x, const double y, const double delta,
             const double start_time, const bool spherical_equatorial)
      : Position(start_time, spherical_equatorial) {
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
  auto operator=(Quintuplet&& rhs) -> Quintuplet& = default;

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
