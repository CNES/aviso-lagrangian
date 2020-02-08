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
#include "lagrangian/map.hpp"
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

class MapOfFiniteLyapunovExponents {
 private:
  lagrangian::MapOfFiniteLyapunovExponents map_;
  lagrangian::FiniteLyapunovExponentsIntegration fle_;

 public:
  MapOfFiniteLyapunovExponents(
      const lagrangian::MapProperties& map_properties,
      const lagrangian::FiniteLyapunovExponentsIntegration& fle,
      const lagrangian::FiniteLyapunovExponentsIntegration::Stencil& stencil,
      const lagrangian::Reader* reader = nullptr)
      : map_(map_properties.get_nx(), map_properties.get_ny(),
             map_properties.get_x_min(), map_properties.get_y_min(),
             map_properties.get_step()),
        fle_(fle) {
    if (reader != nullptr) {
      map_.Initialize(fle_, reader, stencil);
    } else {
      map_.Initialize(fle_, stencil);
    }
  }

  void compute(const size_t num_threads) {
    auto gil = py::gil_scoped_release();
    map_.Compute(fle_, num_threads);
  }

  auto get_map_of_lambda1(const double nan) -> py::array_t<double> {
    return get_map(nan, map_.GetMapOfLambda1(nan, fle_));
  }

  auto get_map_of_lambda2(const double nan) -> py::array_t<double> {
    return get_map(nan, map_.GetMapOfLambda2(nan, fle_));
  }

  auto get_map_of_theta1(const double nan) -> py::array_t<double> {
    return get_map(nan, map_.GetMapOfTheta1(nan, fle_));
  }

  auto get_map_of_theta2(const double nan) -> py::array_t<double> {
    return get_map(nan, map_.GetMapOfTheta2(nan, fle_));
  }

  auto get_map_of_delta_t(const double nan) -> py::array_t<double> {
    return get_map(nan, map_.GetMapOfDeltaT(nan, fle_));
  }

  auto get_map_of_final_separation(const double nan) -> py::array_t<double> {
    return get_map(nan, map_.GetMapOfFinalSeparation(nan, fle_));
  }

 private:
  static inline auto get_map(const double fill_value,
                             lagrangian::Map<double>* map)
      -> py::array_t<double> {
    auto result = py::array_t<double>(
        py::array::ShapeContainer({map->get_nx(), map->get_ny()}));
    auto result_ = result.mutable_unchecked<2>();

    {
      auto gil = py::gil_scoped_release();

      for (auto ix = 0; ix < map->get_nx(); ++ix) {
        for (auto iy = 0; iy < map->get_ny(); ++iy) {
          auto value = map->GetItem(ix, iy);
          result_(ix, iy) = std::isnan(value) ? fill_value : value;
        }
      }
    }
    return result;
  }
};

void init_map(pybind11::module& m) {
  py::class_<lagrangian::MapProperties>(m, "MapProperties",
                                        "Properties of a regular grid")
      .def(py::init<int, int, double, double, double>(), py::arg("nx"),
           py::arg("ny"), py::arg("x_min"), py::arg("y_min"), py::arg("step"),
           R"__doc__(
Default constructor

Args:
     nx (int): Number of longitudes
     ny (int): Number of latitudes
     x_min (float): Minimal longitude
     y_min (float): Minimal latitude
     step (float): Step between two consecutive longitudes and latitudes
)__doc__")
      .def_property_readonly("nx", &lagrangian::MapProperties::get_nx,
                             "Number of longitudes in the grid")
      .def_property_readonly("ny", &lagrangian::MapProperties::get_ny,
                             "Number of latitudes in the grid")
      .def_property_readonly("x_min", &lagrangian::MapProperties::get_x_min,
                             "Minimal longitude")
      .def_property_readonly("y_min", &lagrangian::MapProperties::get_y_min,
                             "Minimal latitude")
      .def_property_readonly(
          "step", &lagrangian::MapProperties::get_step,
          "Step between two consecutive longitudes and latitudes")
      .def(
          "x_axis",
          [](const lagrangian::MapProperties& self) -> py::array_t<double> {
            auto result =
                py::array_t<double>(py::array::ShapeContainer({self.get_nx()}));
            auto result_ = result.mutable_unchecked<1>();
            for (auto ix = 0; ix < self.get_nx(); ++ix) {
              result_(ix) = self.GetXValue(ix);
            }
            return result;
          },
          "Gets the x-axis values")
      .def(
          "y_axis",
          [](const lagrangian::MapProperties& self) -> py::array_t<double> {
            auto result =
                py::array_t<double>(py::array::ShapeContainer({self.get_ny()}));
            auto result_ = result.mutable_unchecked<1>();
            for (auto iy = 0; iy < self.get_ny(); ++iy) {
              result_(iy) = self.GetYValue(iy);
            }
            return result;
          },
          "Gets the y-axis values");

  py::class_<MapOfFiniteLyapunovExponents>(
      m, "MapOfFiniteLyapunovExponents",
      "Handles a map of Finite Size or Time Lyapunov Exponents")
      .def(py::init<lagrangian::MapProperties,
                    lagrangian::FiniteLyapunovExponentsIntegration,
                    lagrangian::FiniteLyapunovExponentsIntegration::Stencil,
                    lagrangian::Reader*>(),
           py::arg("map_properties"), py::arg("fle"),
           py::arg("stencil") =
               lagrangian::FiniteLyapunovExponentsIntegration::kTriplet,
           py::arg("reader") = nullptr, R"__doc__(
Default constructor

Args:
     map_properties (lagrangian.MapProperties): Properties of the regular
          grid to create
     fle (lagrangian.FiniteLyapunovExponents): FLE handler
     stencil (lagrangian.Stencil): Type of stencil used for the calculation of
          finite difference.
     reader (lagrangian.reader.NetCDF):  NetCDF used to locate the hidden
          values​​ (eg continents). These cells identified will not be taken
          into account during the calculation process, in order to accelerate
          it. If this parameter is not defined, all cells will be processed in
          the calculation step.
)__doc__",
           py::keep_alive<1, 5>())
      .def("compute", &MapOfFiniteLyapunovExponents::compute, R"__doc__(
Compute the map

Args:
     num_threads (int, optional): The number of threads to use for the
          computation. If 0 all CPUs are used. If 1 is given, no parallel
          computing code is used at all, which is useful for debugging.
          Defaults to 0.
)__doc__",
           py::arg("num_threads") = 0)
      .def("map_of_lambda1", &MapOfFiniteLyapunovExponents::get_map_of_lambda1,
           py::arg("fill_value") = std::numeric_limits<double>::quiet_NaN(),
           R"__doc__(
Get the map of the FLE associated to the maximum eigenvalues of
Cauchy-Green strain tensor

Args:
     fill_value (float): value used for missing cells

Returns:
     numpy.ndarray: The map of λ₁ (unit 1/sec)
)__doc__")
      .def("map_of_lambda2", &MapOfFiniteLyapunovExponents::get_map_of_lambda2,
           py::arg("fill_value") = std::numeric_limits<double>::quiet_NaN(),
           R"__doc__(
Get the map of the FLE associated to the minimum eigenvalues of
Cauchy-Green strain tensor

Args:
     fill_value (float): value used for missing cells

Returns:
     numpy.ndarray: The map of λ₂ (unit 1/sec)
)__doc__")
      .def("map_of_theta1", &MapOfFiniteLyapunovExponents::get_map_of_theta1,
           py::arg("fill_value") = std::numeric_limits<double>::quiet_NaN(),
           R"__doc__(
Get the map of the orientation of the eigenvectors associated
to the maximum eigenvalues of Cauchy-Green strain tensor

Args:
     fill_value (float): value used for missing cells

Returns:
     numpy.ndarray: The map of θ₁ (unit degrees)
)__doc__")
      .def("map_of_theta2", &MapOfFiniteLyapunovExponents::get_map_of_theta2,
           py::arg("fill_value") = std::numeric_limits<double>::quiet_NaN(),
           R"__doc__(
Get the map of the orientation of the eigenvectors associated
to the minimum eigenvalues of Cauchy-Green strain tensor

Args:
     fill_value (float): value used for missing cells

Returns:
     numpy.ndarray: The map of θ₂ (unit degrees)
)__doc__")
      .def("map_of_delta_t", &MapOfFiniteLyapunovExponents::get_map_of_delta_t,
           py::arg("fill_value") = std::numeric_limits<double>::quiet_NaN(),
           R"__doc__(
Get the map of the advection time

Args:
     fill_value (float): value used for missing cells

Returns:
     numpy.ndarray: The map of advection time (unit number of seconds elapsed
          since the beginning of the integration)
)__doc__")
      .def("map_of_final_separation",
           &MapOfFiniteLyapunovExponents::get_map_of_final_separation,
           py::arg("fill_value") = std::numeric_limits<double>::quiet_NaN(),
           R"__doc__(
Get the map of the effective final separation distance (unit degree)

Args:
     fill_value (float): value used for missing cells

Returns:
     The map of the effective final separation distance (unit degree)
)__doc__");
}
