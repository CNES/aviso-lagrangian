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
#include "datetime.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

/// Class used to validate C++ <-> Python conversion
class DateTime {
 public:
  explicit DateTime(const lagrangian::DateTime &dt) : dt_(dt) {}

  auto operator()() const -> const lagrangian::DateTime & { return dt_; }

 private:
  lagrangian::DateTime dt_;
};

class TimeDuration {
 public:
  explicit TimeDuration(boost::posix_time::time_duration td)
      : td_(std::move(td)) {}

  auto operator()() const -> const boost::posix_time::time_duration & {
    return td_;
  }

 private:
  boost::posix_time::time_duration td_;
};

void init_datetime(py::module &m) {
  py::class_<DateTime>(m, "DateTime")
      .def(py::init<lagrangian::DateTime>())
      .def("to_datetime",
           [](const DateTime &self) -> lagrangian::DateTime { return self(); });

  py::class_<TimeDuration>(m, "TimeDuration")
      .def(py::init<boost::posix_time::time_duration>())
      .def("to_timedelta",
           [](const TimeDuration &self) -> boost::posix_time::time_duration {
             return self();
           });
}
