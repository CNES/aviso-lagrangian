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

#include <algorithm>
#include <cmath>
#include <set>
#include <utility>

// ___________________________________________________________________________//

#include "lagrangian/netcdf/cf.hpp"
#include "lagrangian/netcdf/variable.hpp"
#include "lagrangian/units.hpp"

// ___________________________________________________________________________//

namespace lagrangian {
namespace axis {

/**
 * @brief Units known for a given type of axis.
 */
class Unit {
 protected:
  /**
   * @brief Unit known
   */
  std::set<std::string> unit_;

 public:
  /**
   * @brief Checks if the unit can define this type of axis.
   *
   * @param unit Unit to check
   *
   * @return if the unit can define this type of axis
   */
  inline bool operator()(const std::string& unit) const {
    return unit_.find(unit) != unit_.end();
  }

  /**
   * @brief Default method invoked when an Unit is destroyed.
   */
  virtual ~Unit() = default;

  /**
   * @brief Constructor
   */
  Unit() {}
};

// ___________________________________________________________________________//

/**
 * @brief Units setting out latitudes
 */
class LatitudeUnit : public Unit {
 public:
  /**
   * @brief Default constructor
   */
  LatitudeUnit() {
    unit_.insert("degrees_north");
    unit_.insert("degree_north");
    unit_.insert("degree_N");
    unit_.insert("degrees_N");
    unit_.insert("degreeN");
    unit_.insert("degreesN");
  }
};

// _____________________________________________________________________________

/**
 * @brief Units setting out longitudes
 */
class LongitudeUnit : public Unit {
 public:
  /**
   * @brief Default constructor
   */
  LongitudeUnit() {
    unit_.insert("degrees_east");
    unit_.insert("degree_east");
    unit_.insert("degree_E");
    unit_.insert("degrees_E");
    unit_.insert("degreeE");
    unit_.insert("degreesE");
  }
};

// _____________________________________________________________________________

}  // namespace axis

// _____________________________________________________________________________

/**
 * @brief A coordinate axis is a Variable that specifies one of the coordinates
 * of a Variable's values.
 *
 * Mathematically it is a vector function F from index space to Sn:
 * @code
 * F(i,j,k,...) -> (S1, S2, ...Sn)
 * where i,j,k are integers, and S is the set of reals (R).
 * @endcode
 * The components of F are just its coordinate axes:
 * @code
 * F = (A1, A2, ...An)
 *
 * A1(i,j,k,...) -> S1
 * A2(i,j,k,...) -> S1
 * An(i,j,k,...) -> Sn
 * @endcode
 */
class Axis {
 public:
  /**
   * @brief Type of axis
   */
  enum Type { kUnknown, kLatitude, kLongitude, kTime, kX, kY };

 private:
  Type type_{kUnknown};
  std::vector<double> points_;
  std::vector<double> edges_;
  std::string unit_;
  double start_{};
  double increment_{};
  bool is_regular_{false};
  bool is_ascending_{false};
  bool is_circle_{false};

  // Check if  value(i) = start_ + i * increment_.
  void CalcIsRegular();

  // Put longitude into the range [0, 360] degrees.
  void NormalizeLongitude();

  // Computes the edges, if the axis data are not spaced regularly.
  void MakeEdges();

  // Get the index of the given points. Compute index from formula:
  // (value - start) / step
  inline int FindIndexRegular(const double coordinate, bool bounded) const {
    auto index =
        static_cast<int>(std::round((coordinate - start_) / increment_));

    if (index < 0) {
      return bounded ? 0 : -1;
    }

    if (index >= static_cast<int>(points_.size())) {
      return bounded ? static_cast<int>(points_.size() - 1) : -1;
    }

    return index;
  }

  // Perform a binary search to find of the element of the array whose value
  // is contained in the interval. Values must be contiguous.
  int FindIndexIrregular(double coordinate, bool bounded) const;

  // Computes axis's properties
  void ComputeProperties() {
    // Normalizes longitudes
    NormalizeLongitude();

    // Determine if axis data are spaced regularly
    CalcIsRegular();

    // If the axis data are not spaced regularly, compute edges.
    MakeEdges();
  }

 public:
  Axis() = default;

  /**
   * @brief Create a coordinate axis from an existing NetCDF Variable.
   *
   * @param variable an existing NetCDF Variable
   */
  explicit Axis(const netcdf::Variable& variable);

  /**
   * @brief Create a coordinate axis from values.
   *
   * @param points axis values
   * @param type axis type
   * @param unit unit
   * @param type of axis
   */
  Axis(std::vector<double> points, const Axis::Type type, std::string unit = "")
      : type_(type), points_(std::move(points)), unit_(std::move(unit)) {
    ComputeProperties();
  }

  /**
   * Move constructor
   *
   * @param rhs right value
   */
  Axis(Axis&& rhs) = default;

  /**
   * Move assignment operator
   *
   * @param rhs right value
   */
  Axis& operator=(Axis&& rhs) = default;

  /**
   * @brief Get type of axis
   *
   * @return type of axis
   */
  inline Type get_type() const { return type_; }

  /**
   * @brief Get the ith coordinate value.
   *
   * @param index which coordinate. Between 0 and GetNumElements()-1 inclusive
   *
   * @return coordinate value
   */
  inline double GetCoordinateValue(const int index) const {
    return points_.at(index);
  }

  /**
   * @brief Get the minimum coordinate value.
   *
   * @return minimum coordinate value
   */
  inline double GetMinValue() const {
    return *(std::min_element(points_.begin(), points_.end()));
  }

  /**
   * @brief Get the maximum coordinate value.
   *
   * @return maximum coordinate value
   */
  inline double GetMaxValue() const {
    return *(std::max_element(points_.begin(), points_.end()));
  }

  /**
   * @brief Get the number of values for this axis
   *
   * @return the number of values
   */
  inline int GetNumElements() const { return points_.size(); }

  /**
   * @brief The axis values are spaced regularly
   *
   * @return true if value(i) = GetStart() + i * GetIncrement()
   */
  inline bool is_regular() const { return is_regular_; }

  /**
   * @brief Given a coordinate position, find what grid element contains it.
   * This mean that
   * @code
   * edge[i] <= pos < edge[i+1] (if values are ascending)
   * edge[i] > pos >= edge[i+1] (if values are descending)
   * @endcode
   *
   * @param coordinate position in this coordinate system
   *
   * @return index of the grid point containing it or -1 if outside grid area
   */
  inline int FindIndex(double coordinate) const {
    return is_regular_ ? FindIndexRegular(coordinate, false)
                       : FindIndexIrregular(coordinate, false);
  }

  /**
   * @brief Given a coordinate position, find what grid element contains it or
   * is closest to it.
   *
   * @param coordinate position in this coordinate system
   * @return index of the grid point containing it or -1 if outside grid area
   */
  inline int FindIndexBounded(double coordinate) const {
    return is_regular_ ? FindIndexRegular(coordinate, true)
                       : FindIndexIrregular(coordinate, true);
  }

  /**
   * @brief  Standardization of longitude
   *
   * @param coordinate position in this coordinate system
   * @param circle value of the trigonometric circle in this coordinate system
   *
   * @return Longitude between [GetMinValue(), GetMinValue() + circle]
   */
  double Normalize(const double coordinate, const double circle) const {
    static const double epsilon = 1e-9;
    double result = coordinate;

    if (type_ == kLongitude) {
      while (result >= points_[0] + circle - epsilon) {
        result -= circle;
      }
      while (result < points_[0] - epsilon) {
        result += circle;
      }
      if (fabs(result - coordinate) <= epsilon) {
        result = coordinate;
      }
    }
    return result;
  }

  /**
   * @brief Get the unit string for this axis
   *
   * @param units unit found
   *
   * @return true if units attribute exists otherwise false
   */
  inline bool get_units(std::string& units) const {
    units = unit_;
    return !unit_.empty();
  }

  /**
   * @brief Converts the axis data from unit @c get_units() to @c unit. Can
   * be called only if the function @c get_units is true.
   *
   * @param unit the new unit
   */
  void Convert(const std::string& unit) {
    if (unit_.empty()) {
      throw std::logic_error("The unit of axis is not defined");
    }

    UnitConverter converter = Units::GetConverter(unit_, unit);
    if (!converter.IsNull()) {
      converter.Convert(points_);
      ComputeProperties();
    }
  }

  /**
   * @brief Given a coordinate position, find grids elements around it.
   * This mean that
   * @code
   * points[i0] <= coordinate < points[i1]
   * @endcode
   *
   * @param coordinate position in this coordinate system
   * @param i0
   * @param i1
   *
   * @return true if coordinate is inside grid area
   */
  bool FindIndexes(const double coordinate, int& i0, int& i1) const {
    i0 = i1 = FindIndex(coordinate);

    if (i0 == -1 && is_circle_) {
      i0 = 0;
      i1 = GetNumElements() - 1;
    } else if (GetNumElements() < 2) {
      i1 = i0;
    } else if (i0 != -1) {
      // The element found is mingled with the first axis item, so the
      // next item is located after it.
      if (i0 == 0) {
        ++i1;
      }
      // The element found is mingled with the last axis item, so the
      // previous item is located before it.
      else if (i0 == GetNumElements() - 1) {
        --i0;
      }
      // General case
      else {
        GetCoordinateValue(i0) - coordinate > 1e-4 ? --i0 : ++i1;
      }

      if (is_circle_) {
        i0 %= GetNumElements();
        i1 %= GetNumElements();
      }
    }
    return i0 >= 0 && i1 < GetNumElements();
  }

  /**
   * @brief Get starting value if is_regular()
   *
   * @return starting value if is_regular()
   */
  inline double get_start() const { return start_; }

  /**
   * @brief Get increment value if is_regular()
   *
   * @return increment value if is_regular()
   */
  inline double get_increment() const { return increment_; }

  /**
   * @brief compare two variables instances
   *
   * @param a A variable to compare
   * @param b An other variable to compare
   *
   * @return if variables are equals
   */
  inline friend bool operator==(Axis const& a, Axis const& b) {
    return a.points_ == b.points_ && a.unit_ == b.unit_ && a.type_ == b.type_;
  }
  inline friend bool operator!=(Axis const& a, Axis const& b) {
    return !(a == b);
  }
};

}  // namespace lagrangian
