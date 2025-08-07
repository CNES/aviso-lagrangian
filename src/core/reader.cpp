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
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "datetime.hpp"
#include "lagrangian/reader/factory.hpp"

namespace py = pybind11;

class Reader : public lagrangian::Reader {
 public:
  using lagrangian::Reader::Reader;

  void Open(const std::string &path) override {
    PYBIND11_OVERLOAD_PURE(void, lagrangian::Reader, Open, path);
  }

  void Load(const std::string &name, const std::string &unit = "") override {
    PYBIND11_OVERLOAD_PURE(void, lagrangian::Reader, Load, name, unit);
  }

  auto Interpolate(double longitude, double latitude, double fill_value = 0,
                   lagrangian::CellProperties &cell =
                       lagrangian::CellProperties::NONE()) const
      -> double override {
    PYBIND11_OVERLOAD_PURE(double, lagrangian::Reader, Interpolate, longitude,
                           latitude, fill_value, cell);
  }

  [[nodiscard]] auto GetDateTime(const std::string &name) const
      -> lagrangian::DateTime override {
    PYBIND11_OVERLOAD_PURE(lagrangian::DateTime, lagrangian::Reader,
                           GetDateTime, name);
  }
};

class NetCDF : public lagrangian::reader::NetCDF {
 public:
  using lagrangian::reader::NetCDF::NetCDF;

  void Open(const std::string &path) override {
    PYBIND11_OVERLOAD(void, lagrangian::reader::NetCDF, Open, path);
  }

  void Load(const std::string &name, const std::string &unit = "") override {
    PYBIND11_OVERLOAD(void, lagrangian::reader::NetCDF, Load, name, unit);
  }

  auto Interpolate(double longitude, double latitude, double fill_value = 0,
                   lagrangian::CellProperties &cell =
                       lagrangian::CellProperties::NONE()) const
      -> double override {
    PYBIND11_OVERLOAD(double, lagrangian::reader::NetCDF, Interpolate,
                      longitude, latitude, fill_value, cell);
  }

  [[nodiscard]] auto GetDateTime(const std::string &name) const
      -> lagrangian::DateTime override {
    PYBIND11_OVERLOAD(lagrangian::DateTime, lagrangian::reader::NetCDF,
                      GetDateTime, name);
  }
};

void init_reader(pybind11::module &m) {
  py::module reader = m.def_submodule("reader");
  py::enum_<lagrangian::reader::Factory::Type>(reader, "Type",
                                               "Type of fields reader known")
      .value("NETCDF", lagrangian::reader::Factory::kNetCDF, "netCDF");

  py::class_<lagrangian::CellProperties>(
      m, "CellProperties",
      "Cell properties of the grid used for the interpolation.")
      .def(py::init<>(), "Default constructor")
      .def_static("none", &lagrangian::CellProperties::NONE,
                  "Return the representation of a cell unhandled");

  py::class_<lagrangian::Reader, Reader>(
      m, "Reader", "Abstract class that defines a velocity reader fields.")
      .def(py::init<>());

  py::class_<lagrangian::reader::NetCDF, lagrangian::Reader, NetCDF>(
      reader, "NetCDF", R"__doc__(
 Grid NetCDF CF reader.

 The grid must contain at least one variable and two vectors defining the
 axes of the longitudes and latitudes of the variable. For example :

 .. code::

    dimensions:
      y = 915 ;
      x = 1080 ;
    variables:
       double y(y) ;
           y:long_name = "Latitudes" ;
           y:units = "degrees_north" ;
       double x(x) ;
           x:long_name = "Longitudes" ;
           x:units = "degrees_east" ;
       float u(x, y) ;
           u:_FillValue = 999f ;
           u:long_name = "U" ;
           u:units = "cm/s" ;
           u:date = "2012-01-01 00:00:00.000000 UTC" ;
        float v(y, x) ;
           v:_FillValue = 999f ;
           v:long_name = "U" ;
           v:units = "cm/s" ;
           v:date = "2012-01-01 00:00:00.000000 UTC" ;

.. note::

  The variable to be read must set an attribute named "date" that
  define the date of data contained in the variable.
)__doc__")
      .def(py::init<>())
      .def("open", &lagrangian::reader::NetCDF::Open, py::arg("path"),
           R"__doc__(
Opens a NetCDF grid in read-only.

Args:
  path (str): Path to the NetCDF grid

Raises:
  RuntimeError: If the function can not find the definition of
    longitudes or latitudes in the file.
)__doc__")
      .def("load", &lagrangian::reader::NetCDF::Load, py::arg("name"),
           py::arg("unit") = "", R"__doc__(
Load into memory grid data

Args:
  varname (str): name of the NetCDF grid who contains grid data
  unit (str): Unit of data loaded into memory. If the parameter is
      undefined or contains an empty string, the object will not do unit
      conversion(i.e. the unit of the data loaded into memory is the unit
      defined in the NetCDF file)
)__doc__")
      .def("interpolate", &lagrangian::reader::NetCDF::Interpolate,
           py::arg("lon"), py::arg("lat"), py::arg("fill_value") = 0,
           py::arg("cell") = lagrangian::CellProperties::NONE(), R"__doc__(
Computes the value of the grid point requested by bilinear interpolation

Args:
  longitude (float): Longitude in degrees
  latitude (float): Latitude in degrees
  fill_value (float): Value to be taken into account for fill values
  cell (lagrangian.CellProperties) Properties of the grid used for the
    interpolation.

Returns:
  float: Interpolated value or ``nan`` if point is outside the grid or
    undefined.
)__doc__")
      .def("date", &lagrangian::reader::NetCDF::GetDateTime, py::arg("name"),
           R"__doc__(
 Returns the date of the grid

Args:
  name (str): The variable name containing the date

Returns:
  datetime.datetime: The date of the grid
)__doc__");
}
