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

#include <cmath>

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * Earth Radius in centimeters
 */
const double kEarthRadius = 6371000;

// ___________________________________________________________________________//

/**
 * @brief Compute the fracionnal part
 *
 * @return the fractionnal part of x
 */
inline double FractionnalPart(const double x) {
  double i;

  return std::modf(x, &i);
}

// ___________________________________________________________________________//

/**
 * @brief Compute the square
 *
 * @return x squared
 */
inline double Square(const double x) { return x * x; }

// ___________________________________________________________________________//

/**
 * @brief Convert degrees to radians
 *
 * @return x converted in radians
 */
inline double DegreesToRadians(const double x) {
  static const double f = M_PI / 180.0;
  return x * f;
}

// ___________________________________________________________________________//

/**
 * @brief Convert radians to degrees
 *
 * @return x converted in degrees
 */
inline double RadiansToDegrees(const double x) {
  static const double f = 180.0 / M_PI;
  return x * f;
}

// ___________________________________________________________________________//

/**
 * @brief Calculate distance between two latitudes/longitudes
 *
 * @return distance in degrees
 */
inline double Distance(const double lon0, const double lat0, const double lon1,
                       const double lat1) {
  double y0 = DegreesToRadians(lat0);
  double x0 = DegreesToRadians(lon0);
  double y1 = DegreesToRadians(lat1);
  double x1 = DegreesToRadians(lon1);

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
inline double NormalizeLongitude(double x, const double circle,
                                 const double half_circle) {
  while (x < -half_circle) {
    x += circle;
  }
  while (x >= half_circle) {
    x -= circle;
  }
  return x;
}

}  // namespace lagrangian
