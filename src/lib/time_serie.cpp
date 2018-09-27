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

#include <algorithm>
#include <cfloat>
#include <utility>

// ___________________________________________________________________________//

#include "lagrangian/parameter.hpp"
#include "lagrangian/time_serie.hpp"
#include "lagrangian/trace.hpp"

// ___________________________________________________________________________//

namespace lagrangian {

void TimeSerie::Load(int ix0, const int ix1) {
  // Should we load new data into memory ?
  if (ix0 < first_index_ || ix0 > last_index_ || ix1 < first_index_ ||
      ix1 > last_index_) {
    std::map<std::string, int> new_files;

    first_index_ = ix0;
    last_index_ = ix1;

    // Creation of the new association "open file" and "readers index"
    for (int ix = ix0; ix <= ix1; ++ix) {
      new_files[time_serie_->GetItem(ix)] = ix - first_index_;
    }

    // Swap readers
    for (auto& new_file : new_files) {
      auto it = files_.find(new_file.first);

      // If the file is already open
      if (it != files_.end()) {
        int i1 = new_file.second;
        int i2 = it->second;

        if (i1 != i2) {
          // Search index previously used by this file
          for (auto& file : files_) {
            // If the index exists (it cannot exist if we create a
            // new driver) we update the information about these files
            if (file.second == new_file.second) {
              files_[it->first] = i1;
              files_[file.first] = i2;
              break;
            }
          }

          // Swap readers
          std::swap(readers_[i2], readers_[i1]);
        }
      }
    }

    for (auto& new_file : new_files) {
      // If it's a new file, we need to open it.
      if (files_.find(new_file.first) == files_.end()) {
        Debug(str(boost::format("Loading %s from %s") % varname_ %
                  new_file.first));
        readers_[new_file.second]->Open(new_file.first);
        readers_[new_file.second]->Load(varname_, unit_);
      }
    }
    files_ = new_files;
  }
}

// ___________________________________________________________________________//

/**
 * @brief Predicate sorting time series
 */
struct SortPredicate {
  /**
   * @brief Test dates for two elements of the time series
   *
   * @param a Date a
   * @param b Date b
   *
   * @return true if date a is less than the date b otherwise false
   */
  bool operator()(const std::pair<double, std::string>& a,
                  const std::pair<double, std::string>& b) {
    return a.first < b.first;
  }
};

// ___________________________________________________________________________//

FileList::FileList(const std::vector<std::string>& filenames,
                   const std::string& varname, Reader* const reader) {
  std::vector<std::pair<double, std::string> > files;
  Axis axis_x;
  Axis axis_y;

  same_coordinates_ = true;

  // For all files, find the time to create an associative array: date,
  // filename.
  for (auto& item : filenames) {
    reader->Open(item);
    files.emplace_back(
        std::make_pair(reader->GetDateTime(varname).ToUnixTime(), item));
  }

  // Data are sorted according file date
  std::sort(files.begin(), files.end(), SortPredicate());

  std::vector<double> points;

  // Creates the axis to obtain a filename from a date.
  for (auto& item : files) {
    points.push_back(item.first);
    filenames_.push_back(item.second);
  }
  axis_ = Axis(points, Axis::kTime);
}

// ___________________________________________________________________________//

TimeSerie::TimeSerie(const std::vector<std::string>& filenames,
                     std::string varname, std::string unit,
                     const reader::Factory::Type type)
    : first_index_(-1),
      last_index_(-1),
      varname_(std::move(varname)),
      unit_(std::move(unit)),
      type_(type) {
  // Instances used to read velocity fields
  try {
    readers_.emplace_back(reader::Factory::NewReader(type_));
    readers_.emplace_back(reader::Factory::NewReader(type_));
  } catch (...) {
    for (auto& item : readers_) {
      delete item;
    }
    readers_.clear();
    throw;
  }

  // Create the time series
  time_serie_ = new FileList(filenames, varname_, readers_[0]);
}

// ___________________________________________________________________________//

double TimeSerie::Interpolate(const double date, const double longitude,
                              const double latitude, const double fill_value,
                              CellProperties& cell) {
  int it0, it1;

  // Verify that the user has loaded the grids needed for interpolation
  if (first_index_ == -1) {
    throw std::logic_error("No data loaded into memory");
  }

  time_serie_->FindIndexes(date, it0, it1);

  const double t0 = time_serie_->GetDate(it0);
  const double t1 = time_serie_->GetDate(it1);

  it0 = it0 - first_index_;
  it1 = it1 - first_index_;

  const double dx = 1 / (t1 - t0);

  const double x0 =
      readers_[it0]->Interpolate(longitude, latitude, fill_value, cell);
  const double x1 =
      readers_[it1]->Interpolate(longitude, latitude, fill_value, cell);

  const double w0 = (t1 - date) * dx;
  const double w1 = (date - t0) * dx;

  return (w0 * x0 + w1 * x1) / (w0 + w1);
}

// ___________________________________________________________________________//

void TimeSerie::Load(const double t0, const double t1) {
  int it00, it01, it10, it11;

  time_serie_->FindIndexes(t0, it00, it01);
  time_serie_->FindIndexes(t1, it10, it11);

  it00 = it00 < it10 ? it00 : it10;
  it11 = it11 > it01 ? it11 : it01;

  size_t required_size = abs(it11 - it00) + 1;

  // The current size of the buffer is not adequate
  if (required_size > readers_.size()) {
    size_t previous_size = readers_.size();
    readers_.resize(required_size);

    for (size_t ix = previous_size; ix < required_size; ++ix) {
      readers_[ix] = reader::Factory::NewReader(type_);
    }
  }

  // Loading the needed data
  Load(it00, it11);
}

}  // namespace lagrangian
