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
#include "lagrangian/integration.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

#include "datetime.hpp"

namespace py = pybind11;

struct Iterator : public lagrangian::Iterator {
  explicit Iterator(lagrangian::Iterator self) : lagrangian::Iterator(self) {}
  bool first_or_done_{true};
};

void init_integration(pybind11::module& m) {
  py::class_<lagrangian::RungeKutta>(m, "RungeKutta",
                                     "Fourth-order Runge-Kutta method")
      .def(py::init<double, lagrangian::Field*>(), R"__doc__(
Default constructor

Args:
    size_of_interval (float): Number of time interval
    field (lagrangian.Field): Field reader
)__doc__",
           py::keep_alive<1, 3>())
      .def(
          "compute",
          [](const lagrangian::RungeKutta& self, const lagrangian::DateTime& t,
             const double x, const double y,
             lagrangian::CellProperties& cell) -> py::tuple {
            double xi;
            double yi;
            if (self.Compute(t.ToUnixTime(), x, y, xi, yi, cell)) {
              return py::make_tuple(xi, yi);
            }
            return py::none();
          },
          py::arg("t"), py::arg("x"), py::arg("y"),
          py::arg("cell") = lagrangian::CellProperties::NONE(), R"__doc__(
Move a point in a veocity field

Args:
    t (datetime.datetime): Date
    x (float): Longitude in degrees
    y (float): Latitude in degrees
    cell (lagrangian.CellProperties): Cell properties of the grid used for
        the interpolation

Returns:
    tuple, optional: A tuple that contains the longitude/latitude after the
    shift or None if the velocity field is undefined for the requested
    position
)__doc__");

  py::enum_<lagrangian::FiniteLyapunovExponentsIntegration::Mode>(
      m, "IntegrationMode", "Mode of integration")
      .value("FSLE", lagrangian::FiniteLyapunovExponentsIntegration::kFSLE,
             "Finite Size Lyapunov Exponent")
      .value("FTLE", lagrangian::FiniteLyapunovExponentsIntegration::kFTLE,
             "Finite Time Lyapunov Exponent");

  py::enum_<lagrangian::FiniteLyapunovExponentsIntegration::Stencil>(
      m, "Stencil", "Type of stencils known")
      .value("TRIPLET",
             lagrangian::FiniteLyapunovExponentsIntegration::kTriplet,
             "Define a stencil with 3 points")
      .value("QUINTUPLET",
             lagrangian::FiniteLyapunovExponentsIntegration::kQuintuplet,
             "Define a stencil with 5 points");

  py::class_<lagrangian::Position>(
      m, "Position", "Define the position of N points Mᵢ = (xᵢ, yᵢ)")
      .def(py::init<>(), R"__doc__(
Default constructor

.. note::

    This constructor may lead to an error in diagnostic because start time is
    set to zero: if stencil has not been advected then delta_t is equal to
    minus advection starting time (- start_time_) which is not correct. (see
    FiniteLyapunovExponentsIntegration::ComputeExponents in integration.cpp)
)__doc__")
      .def(py::init([](const lagrangian::DateTime& date,
                       const bool spherical_equatorial) {
             return std::make_unique<lagrangian::Position>(
                 date.ToUnixTime(), spherical_equatorial);
           }),
           py::arg("start_time"), py::arg("spherical_equatorial") = true,
           R"__doc__(
Constructor with start_time setting

Args:
    start_time (datetime.datetime): Advection starting time particles
    spherical_equatorial (bool): True if the coordinates system is Lon/lat
        otherwise false
)__doc__")
      .def(
          "xi",
          [](const lagrangian::Position& self) -> py::array_t<double> {
            auto result =
                py::array_t<double>(py::array::ShapeContainer({self.size()}));
            auto result_ = result.mutable_unchecked<1>();

            for (size_t ix = 0; ix < self.size(); ++ix) {
              result_(ix) = self.get_xi(ix);
            }
            return result;
          },
          "Returns the x-axis of the point")
      .def(
          "yi",
          [](const lagrangian::Position& self) -> py::array_t<double> {
            auto result =
                py::array_t<double>(py::array::ShapeContainer({self.size()}));
            auto result_ = result.mutable_unchecked<1>();

            for (size_t iy = 0; iy < self.size(); ++iy) {
              result_(iy) = self.get_yi(iy);
            }
            return result;
          },
          "Returns the y-axis of the point")
      .def_property_readonly(
          "time",
          [](const lagrangian::Position& self) -> lagrangian::DateTime {
            return lagrangian::DateTime::FromUnixTime(self.get_time());
          },
          "Returns the time at the end of the integration")
      .def_property_readonly("is_completed",
                             &lagrangian::Position::is_completed,
                             "Test if the integration is over")
      .def_property("missing", &lagrangian::Position::Missing,
                    &lagrangian::Position::IsMissing,
                    "True if the integration is complete.")
      .def("max_distance", &lagrangian::Position::MaxDistance,
           "Compute the distance max")
      .def(
          "compute",
          [](lagrangian::Position& self, const lagrangian::RungeKutta& rk4,
             const Iterator& it, lagrangian::CellProperties& cell) -> bool {
            return self.Compute(rk4, it, cell);
          },
          py::arg("rk4"), py::arg("it"), py::arg("cell"), R"__doc__(
To move a particle with a velocity field.

Args:
   rk (lagrangian.RungeKutta): Runge-Kutta handler
   it (lagrangian.Iterator): Iterator
   cell (lagrangian.CellProperties): Cell properties of the grid used for the
    interpolation.

Returns:
    bool: True if the particle could be moved otherwise false
)__doc__")
      .def("strain_tensor", [](const lagrangian::Position& self) -> py::tuple {
        double a00;
        double a01;
        double a10;
        double a11;
        self.StrainTensor(a00, a01, a10, a11);
        return py::make_tuple(a00, a01, a10, a11);
      });

  py::class_<lagrangian::Triplet, lagrangian::Position>(
      m, "Triplet", "Define the position of 3 points")
      .def(py::init<>())
      .def(py::init([](const double x, const double y, const double delta,
                       const lagrangian::DateTime& date,
                       const bool spherical_equatorial) {
             return std::make_unique<lagrangian::Triplet>(
                 x, y, delta, date.ToUnixTime(), spherical_equatorial);
           }),
           py::arg("x"), py::arg("y"), py::arg("delta"), py::arg("start_time"),
           py::arg("spherical_equatorial") = true,
           R"__doc__(
Construct a new object defining the position of the 3 points

Args:
    x (float): Longitude of the initial point
    y (float): Latitude of the initial point
    delta (float): Initial separation in degrees of neighboring
    start_time (datetime.datetime): Advection starting time particles
    spherical_equatorial (bool): True if the coordinates system is Lon/lat
        otherwise false
)__doc__");

  py::class_<lagrangian::Quintuplet, lagrangian::Position>(
      m, "Quintuplet", "Define the position of 5 points")
      .def(py::init<>())
      .def(py::init([](const double x, const double y, const double delta,
                       const lagrangian::DateTime& date,
                       const bool spherical_equatorial) {
             return std::make_unique<lagrangian::Quintuplet>(
                 x, y, delta, date.ToUnixTime(), spherical_equatorial);
           }),
           py::arg("x"), py::arg("y"), py::arg("delta"), py::arg("start_time"),
           py::arg("spherical_equatorial") = true,
           R"__doc__(
Construct a new object defining the position of the 5 points

Args:
    x (float): Longitude of the initial point
    y (float): Latitude of the initial point
    delta (float): Initial separation in degrees of neighboring
    start_time (datetime.datetime): Advection starting time particles
    spherical_equatorial (bool): True if the coordinates system is Lon/lat
        otherwise false
)__doc__");

  py::class_<Iterator>(m, "Iterator",
                       "Definition of an iterator over a time period")
      .def("__iter__", [](Iterator& self) -> Iterator& { return self; })
      .def("__next__", [](Iterator& self) -> lagrangian::DateTime {
        if (!self.first_or_done_) {
          ++self;
        } else {
          self.first_or_done_ = false;
        }
        if (!self.GoAfter()) {
          self.first_or_done_ = true;
          throw py::stop_iteration();
        }
        return lagrangian::DateTime::FromUnixTime(self());
      });

  py::class_<lagrangian::Integration>(m, "Integration",
                                      "Handles the time integration")
      .def(py::init<lagrangian::DateTime, lagrangian::DateTime,
                    boost::posix_time::time_duration, lagrangian::Field*>(),
           py::arg("start_time"), py::arg("end_time"), py::arg("delta_t"),
           py::arg("field"), R"__doc__(Default constructor

Args:
    start_time (datetime.datetime): Start time of the integration
    end_time (datetime.datetime): End date of the integration
    delta_t (datetime.timedelta): Time interval
    field (lagrangian.Field): Field to use for computing the velocity of a
        point.
)__doc__",
           py::keep_alive<1, 5>())
      .def(
          "iterator",
          [](const lagrangian::Integration& self) -> Iterator {
            return Iterator(self.GetIterator());
          },
          R"__doc__(Return an iterator that describes the integration period

Returns:
    lagrangian.Iterator: the iterator that describes the integration period
)__doc__")
      .def(
          "fetch",
          [](lagrangian::Integration& self, const lagrangian::DateTime& date)
              -> void { self.Fetch(date.ToUnixTime()); },
          py::arg("date"),
          R"__doc__(Perform the tasks before a new time step (eg load grids required)

Args:
    date (datetime.datetime): Time step to process
)__doc__")
      .def(
          "compute",
          [](const lagrangian::Integration& self, const Iterator& it,
             const double x0, const double y0) -> py::object {
            double x1;
            double y1;
            if (self.Compute(it, x0, y0, x1, y1)) {
              return py::make_tuple(x1, y1);
            }
            return py::none();
          },
          py::arg("it"), py::arg("x0"), py::arg("y0"),
          R"__doc__(
Calculate the new position of the particle

Args:
    it (lagrangian.Iterator): Iterator
    x0 (float): Longitude in degrees
    y0 (float): Latitude in degrees

Returns:
    tuple, optional: the position updated otherwise None if new position
    is undefined
)__doc__");

  py::class_<lagrangian::Path, lagrangian::Integration>(
      m, "Path",
      "Handles the movement of a particle using the Runge-Kutta method.")
      .def(py::init<lagrangian::DateTime, lagrangian::DateTime,
                    boost::posix_time::time_duration, lagrangian::Field*>(),
           py::arg("start_time"), py::arg("end_time"), py::arg("delta_t"),
           py::arg("field"), R"__doc__(Default constructor

Args:
    start_time (datetime.datetime): Start time of the integration
    end_time (datetime.datetime): End date of the integration
    delta_t (datetime.timedelta): Time interval
    field (lagrangian.Field): Field to use for computing the velocity of a
        point.
)__doc__",
           py::keep_alive<1, 5>());

  py::class_<lagrangian::FiniteLyapunovExponents>(
      m, "FiniteLyapunovExponents", "Storing Lyapunov coefficients calculated")
      .def(py::init<>())
      .def_property("delta_t",
                    &lagrangian::FiniteLyapunovExponents::get_delta_t,
                    &lagrangian::FiniteLyapunovExponents::set_delta_t,
                    "Effective advection time")
      .def_property("lambda1",
                    &lagrangian::FiniteLyapunovExponents::get_lambda1,
                    &lagrangian::FiniteLyapunovExponents::set_lambda1,
                    "FLE associated to the maximum eigenvalue of Cauchy-Green "
                    "strain tensor")
      .def_property("lambda2",
                    &lagrangian::FiniteLyapunovExponents::get_lambda2,
                    &lagrangian::FiniteLyapunovExponents::set_lambda2,
                    "FLE associated to the minimum eigenvalue of Cauchy-Green "
                    "strain tensor")
      .def_property("theta1", &lagrangian::FiniteLyapunovExponents::get_theta1,
                    &lagrangian::FiniteLyapunovExponents::set_theta1,
                    "FLE associated to the minimum eigenvalue of the "
                    "Cauchy-Green strain tensor")
      .def_property("theta2", &lagrangian::FiniteLyapunovExponents::get_theta2,
                    &lagrangian::FiniteLyapunovExponents::set_theta2,
                    "FLE associated to the maximum eigenvalue of the "
                    "Cauchy-Green strain tensor")
      .def_property("final_separation",
                    &lagrangian::FiniteLyapunovExponents::get_final_separation,
                    &lagrangian::FiniteLyapunovExponents::set_final_separation,
                    "Final separation distance");

  py::class_<lagrangian::FiniteLyapunovExponentsIntegration,
             lagrangian::Integration>(m, "FiniteLyapunovExponentsIntegration",
                                      R"__doc__(
Handles the computation of Lyapunov Exponent

Finite Size Lyapunov Exponent (FSLE) is a scalar local notion that
represents the rate of separation of initially neighbouring particles
over a finite-time window [t₀, t₀ + T], where T is the time two
particules need to be advected in order to be separated from a given
distance d.

Let x(t) = x(t; x₀, t₀) be the position of a lagrangian particle
at time t, started at x₀ at t=t₀ and advected by the time-dependent
fluid flow u(x, t).

The Forward Finite-Time Lyapunov Exponent at a point x₀
and for the advection time T is defined as the growth factor of
the norm of the perturbation dx0 started around x₀ and advected
by the flow after the finite advection time T.

Maximal stretching occurs when dx0 is aligned with the eigenvector
associated with the maximum eigenvalue δmax of the Cauchy-Green strain
tensor Δ:

Δ = [ ∇Φ₀ᵀ (x₀) ]^* [ ∇Φ₀ᵀ (x₀) ]

where Φ₀ᵀ : x₀ ➜ x(t, x₀, t₀) is the flow map of the advection equation:
it links the location x₀ of a lagragian particule at t=t₀ to its position
x(t,x₀,t₀) at time t. (* denotes the transposition operator).

FTLE is defined as

σ = ( 1 / (2*T) ) * log( λmax( Δ ) )

Finite-Size Lyapunov Exponent is similarly defined: T is chosen so that
neighbouring particules separate from a given distance d.

ComputeExponents(const Position& position) function implements the
computation of the Lyapunov exponents based on maximal and minimal
eigenvalues and orientation of eigenvectors of Δ given the elements of
∇Φ₀ᵀ matrix.

For more details see:

1. G. Haller, Lagrangian coherent structures and the rate of strain in
   two-dimensional turbulence Phys. Fluids A 13 (2001) 3365-3385
   (http://georgehaller.com/reprints/approx.pdf)
   Remark: In this paper, FTLE is referred to as the Direct Lyapunov
   Exponent (DLE)

 2. http://mmae.iit.edu/shadden/LCS-tutorial/FTLE-derivation.html
)__doc__")
      .def(py::init<lagrangian::DateTime, lagrangian::DateTime,
                    boost::posix_time::time_duration,
                    lagrangian::FiniteLyapunovExponentsIntegration::Mode,
                    double, double, lagrangian::Field*>(),
           py::arg("start_time"), py::arg("end_time"), py::arg("delta_t"),
           py::arg("mode"), py::arg("min_sepration"), py::arg("delta"),
           py::arg("field") = nullptr, R"__doc__(Default constructor

Args:
    start_time (datetime.datetime): Start time of the integration
    end_time (datetime.datetime): End date of the integration
    delta_t (datetime.timedelta): Time interval
    mode (lagrangian.IntegrationMode) Integration mode
    min_separation (float): Minimal separation in degrees
    delta (float): The gap between two consecutive dots, in degrees, of the
        grid
    field (lagrangian.Field): Field to use for computing the velocity of a
        point.
)__doc__",
           py::keep_alive<1, 8>())
      .def("set_initial_point",
           &lagrangian::FiniteLyapunovExponentsIntegration::SetInitialPoint,
           py::arg("x"), py::arg("y"), py::arg("stencil"),
           py::arg("spherical_equatorial") = true, R"__doc__(
Set the value of the initial point

Args:
    x (float): Longitude
    y (float): Latitude
    stencil (lagrangian.Stencil):  Type of stencil used
    spherical_equatorial (bool):  True if the coordinates system is Lon/lat
        otherwise false

Returns:
    lagrangian.Position: The position of the initial point
)__doc__")
      .def("separation",
           &lagrangian::FiniteLyapunovExponentsIntegration::Separation,
           py::arg("position"), R"__doc__(
Determine whether the particle is deemed to be separate

Args:
    position (lagrangian.Position): Position of the particle

Returns:
    bool: True if the particle is separated.
)__doc__")
      .def_property_readonly(
          "mode", &lagrangian::FiniteLyapunovExponentsIntegration::get_mode,
          "Mode of integration")
      .def(
          "compute",
          [](const lagrangian::FiniteLyapunovExponentsIntegration& self,
             const Iterator& it, lagrangian::Position* const position,
             lagrangian::CellProperties& cell) -> bool {
            return self.Compute(it, position, cell);
          },
          py::arg("it"), py::arg("position"), py::arg("cell"), R"__doc__(
Calculate the integration

Args:
    it (lagrangian.Iterator): Iterator
    position (lagrangian.Position): Position of the particle
    cell (lagrangian.CellProperties): Cell properties of the grid used for the
        interpolation

Returns:
    True if the integration is defined otherwise false
)__doc__")
      .def("exponents",
           &lagrangian::FiniteLyapunovExponentsIntegration::ComputeExponents,
           py::arg("position"), py::arg("fsle"), R"__doc__(
Compute the eigenvalue and the orientation of the eigenvectors of the
Cauchy-Green strain tensor

Args:
    position (lagrangian.Position): Position of the particle
    fle (lagrangian.FiniteLyapunovExponents): Finite Lyapunov Exponents
        computed

Returns:
   bool: True if the exponents are defined
)__doc__");
}
