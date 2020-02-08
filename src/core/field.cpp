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
#include "lagrangian/field/time_serie.hpp"
#include "lagrangian/field/vonkarman.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <utility>

namespace py = pybind11;

class Python : public lagrangian::Field {
 public:
  Python(py::object parent, const UnitType unit_type,
         const CoordinatesType coordinates_type)
      : lagrangian::Field(unit_type, coordinates_type),
        parent_(std::move(std::move(parent))) {}

  auto Compute(const double t, const double x, const double y, double& u,
               double& v, lagrangian::CellProperties& /*cell*/) const
      -> bool override {
    auto method = parent_.attr("compute");
    auto datetime = to_datetime(lagrangian::DateTime::FromUnixTime(t));
    py::object result = method(datetime, x, y);
    if (py::isinstance<py::tuple>(result)) {
      auto tuple = py::cast<py::tuple>(result);
      u = tuple[0].cast<double>();
      v = tuple[1].cast<double>();
      return true;
    }
    u = std::numeric_limits<double>::quiet_NaN();
    v = std::numeric_limits<double>::quiet_NaN();

    return false;
  }

  void Fetch(const double t0, const double t1) override {
    auto method = parent_.attr("fetch");
    method(to_datetime(lagrangian::DateTime::FromUnixTime(t0)),
           to_datetime(lagrangian::DateTime::FromUnixTime(t1)));
  }

 private:
  py::object parent_;
};

void init_field(pybind11::module& m) {
  py::enum_<lagrangian::Field::UnitType>(m, "UnitType", "Unit field")
      .value("METRIC", lagrangian::Field::kMetric,
             "The field velocity is expressed in the metric system (eg. m/s)")
      .value(
          "ANGULAR", lagrangian::Field::kAngular,
          "The field velocity is expressed in the angular system (eg. deg/s)");

  py::enum_<lagrangian::Field::CoordinatesType>(m, "CoordinatesType",
                                                "Type of coordinates")
      .value("CARTESIAN", lagrangian::Field::kCartesian, "Cartesian")
      .value("SPHERICAL_EQUATORIAL", lagrangian::Field::kSphericalEquatorial,
             "Spherical equatorial");

  py::class_<lagrangian::Field>(m, "Field",
                                "Abstract class defining a field where it is "
                                "possible to calculate a speed")
      .def(py::init<lagrangian::Field::UnitType,
                    lagrangian::Field::CoordinatesType>(),
           py::arg("unit_type"),
           py::arg("coordinate_type") = lagrangian::Field::kSphericalEquatorial,
           R"__doc__(
Default constructor

Args:
    unit_type (lagrangian.Field): Unit field
    coordinate_type (lagrangian.CoordinateType): Type of the coordinate system

Raises:
   ValueError: if the type of unit or the coordinate system is unknown
)__doc__")
      .def_property_readonly("unit_type", &lagrangian::Field::get_unit_type,
                             "Unit type used by this field.")
      .def_property_readonly("coordinate_type",
                             &lagrangian::Field::get_coordinates_type,
                             "Coordinate type used by this field.")
      .def("unit", &lagrangian::Field::GetUnit,
           R"__doc__(Returns the unit used by this field
Returns:
    str: unit used by this field
)__doc__")
      .def(
          "fetch",
          [](lagrangian::Field& self, const lagrangian::DateTime& t0,
             const lagrangian::DateTime& t1) -> void {
            self.Fetch(t0.ToUnixTime(), t1.ToUnixTime());
          },
          py::arg("first"), py::arg("last"), R"__doc__(
Loads the grids used to interpolate the velocities in the interval
[first, last]

Args:
    first (datetime.datetime): First date of the interval
    last (datetime.datetime): Last date of the interval
)__doc__")
      .def(
          "compute",
          [](const lagrangian::Field& self, const lagrangian::DateTime& t,
             const double x, const double y,
             lagrangian::CellProperties& cell) -> py::object {
            double u;
            double v;
            if (self.Compute(t.ToUnixTime(), x, y, u, v, cell)) {
              return py::make_tuple(u, v);
            }
            return py::none();
          },
          py::arg("t"), py::arg("x"), py::arg("y"),
          py::arg("cell") = lagrangian::CellProperties::NONE(), R"__doc__(
Interpolates the velocity to the wanted spatio temporal position.

Args:
    t (datetime.datetime): Date to evaluate
    x (float): Longitude Longitude expressed as degree
    y (float): Latitude expressed as degree
    cell (lagrangian.CellProperties): Cell properties of the grid used for the
        interpolation

Returns:
    tuple, optional: U and V components of the velocities evaluated or None if
    the velocity is undefined for the requested space-time position
)__doc__");

  py::module field = m.def_submodule("field");

  py::class_<lagrangian::field::TimeSerie, lagrangian::Field>(
      field, "TimeSerie", "Time series of velocity field")
      .def(
          py::init<std::string, lagrangian::Field::UnitType,
                   lagrangian::Field::CoordinatesType,
                   lagrangian::reader::Factory::Type>(),
          py::arg("configuration_file"),
          py::arg("unit_type") = lagrangian::Field::kMetric,
          py::arg("coordinates_type") = lagrangian::Field::kSphericalEquatorial,
          py::arg("reader_type") = lagrangian::reader::Factory::kNetCDF,
          R"__doc__(
 Default constructor

 Args:
    configuration_file (str): The configuration file contains the list of
        files to take into account to interpolate velocities.
    unit_type (lagrangian.UnitType) Unit fields.
    coordniates_type (lagrangian.CoordinatesType) : Type of coordinates
    reader_type (lagrangian.reader.Type) The reader used to read grids
        containing velocities.
)__doc__")
      .def("start_time", &lagrangian::field::TimeSerie::StartTime,
           "Returns the date of the first grid constituting the time series.")
      .def("end_time", &lagrangian::field::TimeSerie::EndTime,
           "Returns the date of the last grid constituting the time series.");

  py::class_<lagrangian::field::Vonkarman, lagrangian::Field>(
      field, "Vonkarman", "Vonkarman field")
      .def(py::init<>());

  py::class_<Python, lagrangian::Field>(
      field, "Python", "Class to implement a velocity field in Python")
      .def(py::init<py::object, lagrangian::Field::UnitType,
                    lagrangian::Field::CoordinatesType>(),
           py::arg("self"), py::arg("unit_type") = lagrangian::Field::kMetric,
           py::arg("coordinates_type") = lagrangian::Field::kCartesian);
}
