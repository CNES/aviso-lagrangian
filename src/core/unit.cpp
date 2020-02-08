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
#include "lagrangian/units.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void init_unit(pybind11::module& m) {
  py::module units = m.def_submodule("units");
  py::class_<lagrangian::Units>(
      units, "Units", "Provides support for units of physical quantities.")
      .def_static(
          "get_converter",
          [](const std::string& from, const std::string& to) -> py::tuple {
            auto converter = lagrangian::Units::GetConverter(from, to);
            return py::make_tuple(converter.get_offset(),
                                  converter.get_scale());
          },
          py::arg("from"), py::arg("to"),
          "Computes a converter of numeric values in unit 'from' to "
          "numeric values in unit 'to'.");
}
