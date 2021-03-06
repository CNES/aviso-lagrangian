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

#include <string>
#include <vector>

// ___________________________________________________________________________//

#include "lagrangian/axis.hpp"
#include "lagrangian/reader/factory.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Management of a time series consisting of a list of files.
 */
class FileList {
 public:
  /**
   * @brief Create a new instance of FileList.
   *
   * @param filenames List of files constituting the time series.
   * @param varname Name the variable containing data to be processed.
   * @param reader Instance of an object implementing the class Reader.
   */
  FileList(const std::vector<std::string>& filenames,
           const std::string& varname, Reader* reader);

  /**
   * @brief Given a date expressed as a number of seconds elapsed since
   * 1970, find elements around it. This mean that
   * @code
   * dates[i0] <= date < dates[i1]
   * @endcode
   *
   * @param date Date request
   * @param i0
   * @param i1
   *
   * @throw std::out_of_range if the date requested is not included in the
   * time series.
   */
  inline void FindIndexes(const double date, int& i0, int& i1) {
    if (!axis_.FindIndexes(date, i0, i1)) {
      throw std::out_of_range("date out of range");
    }
  }

  /**
   * @brief Get the number of filename in this time series
   *
   * @return number of filenames
   */
  [[nodiscard]] inline auto GetNumElements() const -> int {
    return axis_.GetNumElements();
  }

  /**
   * @brief Get the ith filename.
   *
   * @param index which filename. Between 0 and GetNumElements()-1 inclusive
   *
   * @return filename
   *
   * @throw std::out_of_range if index is out of range
   */
  inline auto GetItem(const int index) -> std::string& {
    return filenames_.at(index);
  }

  /**
   * @brief Get the ith date
   *
   * @param index which date
   *
   * @return date in JulianDay
   */
  [[nodiscard]] inline auto GetDate(const int index) const -> double {
    return axis_.GetCoordinateValue(index);
  }

  /**
   * @brief Returns true if the file list have the same spatial coordinates.
   *
   * @return false if the file list have different spatial coordinates.
   */
  [[nodiscard]] inline auto same_coordinates() const -> bool {
    return same_coordinates_;
  }

 private:
  Axis axis_;
  std::vector<std::string> filenames_;
  bool same_coordinates_;
};

// ___________________________________________________________________________//

/**
 * @brief Spatio-temporal interpolation of a series of grids.
 *
 * The series is described by a list of files containing only one date per
 * file.
 */
class TimeSerie {
 public:
  /**
   * @brief Loads the data necessary for the interpolation of the values in
   * the interval [begin, end].
   *
   * @param t0 Date of the first measurement to compute in number of
   *  seconds elapsed since 1/1/1970)
   * @param t1 Date of the last measurement to compute in number of
   *  seconds elapsed since 1/1/1970)
   */
  void Load(double t0, double t1);

  /**
   * @brief Create a new instance of TimeSerie.
   *
   * @param filenames List of files constituting the time series.
   * @param varname Name the variable containing data to be processed.
   * @param unit Unit of data required by the user. If the parameter is
   * undefined or contains an empty string, the object will not do unit
   * conversion(i.e. the unit of the interpolated value is the unit of the
   * file)
   * @param type Instance of an object implementing the class Reader. By
   * default the class uses the reader of NetCDF grids.
   */
  TimeSerie(const std::vector<std::string>& filenames, std::string varname,
            std::string unit = "",
            reader::Factory::Type type = reader::Factory::kNetCDF);

  /**
   * @brief Default method invoked when a TimeSerie is destroyed.
   */
  ~TimeSerie() {
    for (auto& item : readers_) {
      delete item;
    }
    delete time_serie_;
  }

  /**
   * @brief Computes the value of point (x, y, t) in the series.
   *
   * @param date Date (in number of seconds elapsed since 1970-1-1
   * 00:00:00.0+00:00)
   * @param longitude in degrees . In output, the longitude is normalized
   * relative to the definition of its axis.
   * @param latitude in degrees
   * @param fill_value Value to be taken into account for fill values
   * @param cell Cell properties of the grid used for the interpolation
   *
   * @return the interpolated value
   */
  auto Interpolate(double date, double longitude, double latitude,
                   double fill_value, CellProperties& cell) -> double;

  /**
   * @brief Returns the first date of the time series.
   *
   * @returns the first date
   */
  [[nodiscard]] inline auto GetFirstDate() const -> double {
    return time_serie_->GetDate(0);
  }

  /**
   * @brief Returns the last date of the time series.
   *
   * @returns the last date
   */
  [[nodiscard]] inline auto GetLastDate() const -> double {
    return time_serie_->GetDate(time_serie_->GetNumElements() - 1);
  }

  /**
   * @brief Get the number of items in this time series
   *
   * @return number of filenames
   */
  [[nodiscard]] inline auto GetNumElements() const -> int {
    return time_serie_->GetNumElements();
  }

 private:
  std::vector<Reader*> readers_;
  FileList* time_serie_;
  int first_index_, last_index_;
  std::string varname_;
  std::string unit_;
  reader::Factory::Type type_;
  std::map<std::string, int> files_;

  // Load new files in memory if necessary.
  void Load(int ix0, int ix1);
};

}  // namespace lagrangian
