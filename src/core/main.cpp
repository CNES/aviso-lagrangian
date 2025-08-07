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

#include "lagrangian/trace.hpp"

namespace py = pybind11;

extern void init_axis(pybind11::module &m);
extern void init_datetime(pybind11::module &m);
extern void init_field(pybind11::module &m);
extern void init_integration(pybind11::module &m);
extern void init_map(pybind11::module &m);
extern void init_reader(pybind11::module &m);
extern void init_unit(pybind11::module &m);

PYBIND11_MODULE(core, m) {
  init_axis(m);
  init_datetime(m);
  init_reader(m);
  init_field(m);
  init_integration(m);
  init_map(m);
  init_unit(m);

  m.def("version", &lagrangian::Version,
        R"__doc__(Return the version number

Return:
  str: Version number
)__doc__")
      .def("set_verbose", &lagrangian::SetVerbose, py::arg("value"),
           R"__doc__(Enable or disable verbose mode

Args:
  value (bool): True to enable verbose mode
)__doc__")
      .def("debug", &lagrangian::Debug, py::arg("message"),
           R"__doc__(Display a debugging message

Args:
  message (str): Message to display
)__doc__");
}
