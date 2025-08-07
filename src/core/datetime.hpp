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
#include <pybind11/pybind11.h>
// clang-format off
// This Python header must be included after pybind11/pybind11.h
#include <datetime.h>
// clang-format on

#include "lagrangian/datetime.hpp"

namespace pybind11::detail {

auto inline microseconds =
    [](const boost::posix_time::time_duration& time_duration) {
      static int ticks = boost::posix_time::time_duration::ticks_per_second();
      int fractional = time_duration.fractional_seconds();

      return ticks > 1000000 ? fractional / (ticks * 1e-6)
                             : fractional * (1000000 / ticks);
    };

/// type_caster for boost::posix_time::time_duration
template <>
struct type_caster<boost::posix_time::time_duration> {
 public:
  /// Caster type for pybind
  PYBIND11_TYPE_CASTER(boost::posix_time::time_duration,
                       _("boost::posix_time::time_duration"));

  /// Conversion part 1 (Python->C++): convert a PyObject into an
  /// boost::posix_time::time_duration instance or return false upon failure.
  /// The second argument indicates whether implicit conversions should be
  /// applied.
  auto load(handle src, bool /*unused*/) -> bool {
    // Lazy initialise the PyDateTime import
    if (PyDateTimeAPI == nullptr) {
      PyDateTime_IMPORT;
    }
    if (!src) {
      return false;
    }
    // If invoked with datetime.delta object
    if (PyDelta_Check(src.ptr())) {
      value = boost::posix_time::seconds(
                  PyDateTime_DELTA_GET_DAYS(src.ptr()) * 86400 +
                  PyDateTime_DELTA_GET_SECONDS(src.ptr())) +
              boost::posix_time::microseconds(
                  PyDateTime_DELTA_GET_MICROSECONDS(src.ptr()));
      return true;
    }
    return false;
  }

  /// Conversion part 2 (C++ -> Python): convert an
  /// boost::posix_time::time_duration instance into a Python object. The second
  /// and third arguments are used to indicate the return value policy and
  /// parent object (for
  /// ``return_value_policy::reference_internal``) and are generally
  /// ignored by implicit casters.
  static auto cast(const boost::posix_time::time_duration& src,
                   return_value_policy /* policy */, handle /* parent */)
      -> handle {
    // Lazy initialise the PyDateTime import
    if (PyDateTimeAPI == nullptr) {
      PyDateTime_IMPORT;
    }
    return PyDelta_FromDSU(0, src.total_seconds(), microseconds(src));
  }
};

/// type_caster for boost::posix_time::ptime
template <>
struct type_caster<lagrangian::DateTime> {
 public:
  /// Caster type for pybind
  PYBIND11_TYPE_CASTER(lagrangian::DateTime, _("lagrangian::DateTime"));

  /// Conversion part 1 (Python->C++): convert a PyObject into a
  /// boost::posix_time::ptime instance or return false upon failure. The second
  /// argument indicates whether implicit conversions should be applied.
  auto load(handle src, bool /*unused*/) -> bool {
    // Lazy initialise the PyDateTime import
    if (PyDateTimeAPI == nullptr) {
      PyDateTime_IMPORT;
    }

    if (!src) {
      return false;
    }

    if (PyDateTime_Check(src.ptr())) {
      if (reinterpret_cast<_PyDateTime_BaseTZInfo*>(src.ptr())->hastzinfo !=
          0) {
        throw std::invalid_argument(
            "only the naive datetime object can be converted to the "
            "corresponding C++ type.");
      }
      auto ptime = boost::posix_time::ptime(
          boost::gregorian::date(PyDateTime_GET_YEAR(src.ptr()),
                                 PyDateTime_GET_MONTH(src.ptr()),
                                 PyDateTime_GET_DAY(src.ptr())),
          boost::posix_time::hours(PyDateTime_DATE_GET_HOUR(src.ptr())) +
              boost::posix_time::minutes(
                  PyDateTime_DATE_GET_MINUTE(src.ptr())) +
              boost::posix_time::seconds(
                  PyDateTime_DATE_GET_SECOND(src.ptr())) +
              boost::posix_time::microseconds(
                  PyDateTime_DATE_GET_MICROSECOND(src.ptr())));
      value = lagrangian::DateTime(ptime);

      return true;
    }
    return false;
  }

  /// Conversion part 2 (C++ -> Python): convert an UTC instance into
  /// a Python object. The second and third arguments are used to
  /// indicate the return value policy and parent object (for
  /// ``return_value_policy::reference_internal``) and are generally
  /// ignored by implicit casters.
  static auto cast(const lagrangian::DateTime& src,
                   return_value_policy /* policy */, handle /* parent */)
      -> handle {
    // Lazy initialise the PyDateTime import
    if (PyDateTimeAPI == nullptr) {
      PyDateTime_IMPORT;
    }

    return PyDateTime_FromDateAndTime(src.Year(), src.Month(), src.Day(),
                                      src.Hour(), src.Minute(), src.Second(),
                                      src.Microsecond());
  }
};

}  // namespace pybind11::detail

inline auto to_datetime(const lagrangian::DateTime& datetime)
    -> pybind11::handle {
  if (PyDateTimeAPI == nullptr) {
    PyDateTime_IMPORT;
  }
  return PyDateTime_FromDateAndTime(
      datetime.Year(), datetime.Month(), datetime.Day(), datetime.Hour(),
      datetime.Minute(), datetime.Second(), datetime.Microsecond());
}
