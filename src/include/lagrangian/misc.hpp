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

#include <cmath>

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * Earth Radius in centimeters
 */
const double kEarthRadius = 6371000;

// ___________________________________________________________________________//

/**
 * @brief Compute the fractional part
 *
 * @return the fractional part of x
 */
inline auto FractionalPart(const double x) -> double {
  double i;

  return std::modf(x, &i);
}

// ___________________________________________________________________________//

/**
 * @brief Compute the square
 *
 * @return x squared
 */
inline auto Square(const double x) -> double { return x * x; }

// ___________________________________________________________________________//

/**
 * @brief Convert degrees to radians
 *
 * @return x converted in radians
 */
inline auto DegreesToRadians(const double x) -> double {
  static const double f = M_PI / 180.0;
  return x * f;
}

// ___________________________________________________________________________//

/**
 * @brief Convert radians to degrees
 *
 * @return x converted in degrees
 */
inline auto RadiansToDegrees(const double x) -> double {
  static const double f = 180.0 / M_PI;
  return x * f;
}

// ___________________________________________________________________________//

/**
 * @brief Calculate distance between two points in a Cartesian space
 *
 * @return distance in meters
 */
inline auto Distance(const double x0, const double y0, const double x1,
                     const double y1) -> double {
  auto dx = x1 - x0;
  auto dy = y1 - y0;
  return std::sqrt((dx * dx) + (dy * dy));
}

/**
 * @brief Calculate distance between two points in a spherical equatorial
 * coordinates system
 *
 * @return distance in degrees
 */
inline auto GeodeticDistance(const double lon0, const double lat0,
                             const double lon1, const double lat1) -> double {
  auto y0 = DegreesToRadians(lat0);
  auto x0 = DegreesToRadians(lon0);
  auto y1 = DegreesToRadians(lat1);
  auto x1 = DegreesToRadians(lon1);

  return RadiansToDegrees(
      acos(sin(y0) * sin(y1) + cos(y0) * cos(y1) * cos(x1 - x0)));
}

/**
 * @brief Standardization of longitude
 *
 * @param x Longitude position
 * @param circle Value of the trigonometric circle in this coordinate system
 * @param half_circle Value of the trigonometric circle / 2 in this coordinate
 * system
 *
 * @return Longitude between [-half_circle, half_circle]
 */
inline auto NormalizeLongitude(double x, const double circle,
                               const double half_circle) -> double {
  while (x < -half_circle) {
    x += circle;
  }
  while (x >= half_circle) {
    x -= circle;
  }
  return x;
}

}  // namespace lagrangian
