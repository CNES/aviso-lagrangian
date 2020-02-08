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
#include "lagrangian/axis.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void init_axis(pybind11::module& m) {
  py::module axis = m.def_submodule("axis");
  py::class_<lagrangian::axis::LatitudeUnit>(axis, "LatitudeUnit",
                                             "Units setting out latitudes")
      .def(py::init<>(), "Default constructor")
      .def("__call__",
           [](const lagrangian::axis::LatitudeUnit& self,
              const std::string& unit) -> bool { return self(unit); },
           py::arg("unit"), "Checks if the unit can define a latitude axis");

  py::class_<lagrangian::axis::LongitudeUnit>(axis, "LongitudeUnit",
                                              "Units setting out longitudes")
      .def(py::init<>(), "Default constructor")
      .def("__call__",
           [](const lagrangian::axis::LongitudeUnit& self,
              const std::string& unit) -> bool { return self(unit); },
           py::arg("unit"), "Checks if the unit can define a longitude axis");
}
