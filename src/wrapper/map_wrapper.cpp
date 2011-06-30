/*
    This file is part of lagrangian library.

    lagrangian is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lagrangian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "numpy.hpp"
#include "map_wrapper.h"

namespace wrapper
{

MapProperties::MapProperties(const int nx,
        const int ny,
        const double x_min,
        const double y_min,
        const double step) :
    lagrangian::MapProperties(nx, ny, x_min, y_min, step)
{
}

bp::numeric::array MapProperties::GetXAxis() const
{
    bp::numeric::array result = MakeVector(get_nx());
    double* data = static_cast<double*> PyArray_DATA(result.ptr());
    for (int ix = 0; ix < get_nx(); ++ix)
    data[ix] = GetXValue(ix);
    return result;
}

bp::numeric::array MapProperties::GetYAxis() const
{
    bp::numeric::array result = MakeVector(get_ny());
    double* data = static_cast<double*> PyArray_DATA(result.ptr());
    for (int iy = 0; iy < get_ny(); ++iy)
    data[iy] = GetYValue(iy);
    return result;
}

// ___________________________________________________________________________//

bp::numeric::array MapOfFiniteLyapunovExponents::GetMapOfExponents(
        double const nan,
        const lagrangian::Iterator& it,
        lagrangian::FiniteLyapunovExponents& fle,
        GetExponent pGetExponent) const
{
    std::vector<intp> dims;

    dims.push_back(map_.get_nx());
    dims.push_back(map_.get_ny());

    bp::numeric::array result = MakeMatrix(dims);
    double* data = static_cast<double*> PyArray_DATA(result.ptr());

    for (int ix = 0; ix < map_.get_nx(); ++ix)
    {
        for (int iy = 0; iy < map_.get_ny(); ++iy)
        {
            lagrangian::Triplet t = map_.GetItem(ix, iy);
            if (t.IsMissing())
            {
                data[ix * map_.get_ny() + iy] = nan;
            }
            else
            {
                fle.Exponents(it, t);
                data[ix * map_.get_ny() + iy] = (fle.*pGetExponent)();
            }
        }
    }
    return result;
}

MapOfFiniteLyapunovExponents::MapOfFiniteLyapunovExponents(const int nx,
        const int ny,
        const double x_min,
        const double y_min,
        const double step) :
lagrangian::map::FiniteLyapunovExponents(nx, ny, x_min, y_min, step),
map_properties_(nx, ny, x_min, y_min, step)
{
}

bp::numeric::array MapOfFiniteLyapunovExponents::GetMapOfLambda1(
        const double nan,
        lagrangian::Iterator const & it,
        lagrangian::FiniteLyapunovExponents& fle) const
{
    return GetMapOfExponents(nan,
            it,
            fle,
            &lagrangian::FiniteLyapunovExponents::get_lambda1);
}

bp::numeric::array MapOfFiniteLyapunovExponents::GetMapOfLambda2(
        const double nan,
        lagrangian::Iterator const & it,
        lagrangian::FiniteLyapunovExponents& fle) const
{
    return GetMapOfExponents(nan,
            it,
            fle,
            &lagrangian::FiniteLyapunovExponents::get_lambda2);
}

bp::numeric::array MapOfFiniteLyapunovExponents::GetMapOfTheta1(
        const double nan,
        lagrangian::Iterator const & it,
        lagrangian::FiniteLyapunovExponents& fle) const
{
    return GetMapOfExponents(nan,
            it,
            fle,
            &lagrangian::FiniteLyapunovExponents::get_theta1);
}

bp::numeric::array MapOfFiniteLyapunovExponents::GetMapOfTheta2(
        const double nan,
        lagrangian::Iterator const & it,
        lagrangian::FiniteLyapunovExponents& fle) const
{
    return GetMapOfExponents(nan,
            it,
            fle,
            &lagrangian::FiniteLyapunovExponents::get_theta2);
}

MapProperties MapOfFiniteLyapunovExponents::get_map_properties() const
{
    return map_properties_;
}

// ___________________________________________________________________________//

void MapPythonModule()
{
    //
    // Map
    //
    bp::class_<MapProperties>(
            "MapProperties",
            bp::init<int, int, double, double, double >((
                    bp::arg("nx"),
                    bp::arg("ny"),
                    bp::arg("x_min"),
                    bp::arg("y_min"),
                    bp::arg("step"))))
        .def(
            "get_nx",
            (int (MapProperties::*)() const)
                (&MapProperties::get_ny))
        .def(
            "get_ny",
            (int (MapProperties::*)() const)
                (&MapProperties::get_ny))
        .def(
            "get_step",
            (double (MapProperties::*)() const)
                (&MapProperties::get_step))
        .def(
            "get_x_min",
            (double (MapProperties::*)() const)
                (&MapProperties::get_x_min))
        .def(
            "get_y_min",
            (double (MapProperties::*)() const)
                (&MapProperties::get_y_min))
        .def(
            "GetXAxis",
            (bp::numeric::array (MapProperties::*)() const)
                (&MapProperties::GetXAxis))
        .def(
            "GetYAxis",
            (bp::numeric::array (MapProperties::*)() const)
                (&MapProperties::GetYAxis));
    //
    // lagrangian::MapOfFiniteLyapunovExponents
    //
    bp::class_<MapOfFiniteLyapunovExponents>(
            "MapOfFiniteLyapunovExponents",
            bp::init<int, int, double, double, double>((
                    bp::arg("nx"),
                    bp::arg("ny"),
                    bp::arg("x_min"),
                    bp::arg("y_min"),
                    bp::arg("step"))))
        .def(
            "Compute",
            (void (MapOfFiniteLyapunovExponents::*)
                (lagrangian::FiniteLyapunovExponents &))
                    (&MapOfFiniteLyapunovExponents::Compute),
            (bp::arg("fle")))
        .def(
            "GetMapOfLambda1",
            (bp::numeric::array (MapOfFiniteLyapunovExponents::* )
                (double const,
                 lagrangian::Iterator const &,
                 lagrangian::FiniteLyapunovExponents &))
                    (&MapOfFiniteLyapunovExponents::GetMapOfLambda1),
            (bp::arg("nan"),
             bp::arg("it"),
             bp::arg("fle")))
        .def(
            "GetMapOfLambda2",
            (bp::numeric::array (MapOfFiniteLyapunovExponents::* )
                (double const,
                 lagrangian::Iterator const &,
                 lagrangian::FiniteLyapunovExponents &))
                    (&MapOfFiniteLyapunovExponents::GetMapOfLambda2),
            (bp::arg("nan"),
             bp::arg("it"),
             bp::arg("fle")))
        .def(
            "GetMapOfTheta1",
            (bp::numeric::array (MapOfFiniteLyapunovExponents::* )
                (double const,
                 lagrangian::Iterator const &,
                 lagrangian::FiniteLyapunovExponents &))
                    (&MapOfFiniteLyapunovExponents::GetMapOfTheta1),
            (bp::arg("nan"),
             bp::arg("it"),
             bp::arg("fle")))
        .def(
            "GetMapOfTheta2",
            (bp::numeric::array (MapOfFiniteLyapunovExponents::* )
                (double const,
                 lagrangian::Iterator const &,
                 lagrangian::FiniteLyapunovExponents &))
                    (&MapOfFiniteLyapunovExponents::GetMapOfTheta2),
            (bp::arg("nan"),
             bp::arg("it"),
             bp::arg("fle")))
        .def(
            "get_map_properties",
            (wrapper::MapProperties (MapOfFiniteLyapunovExponents::* )
                ())
                    (&MapOfFiniteLyapunovExponents::get_map_properties));
}

}
