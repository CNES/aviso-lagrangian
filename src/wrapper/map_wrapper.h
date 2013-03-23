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

#pragma once

// ___________________________________________________________________________//

#include <boost/python.hpp>

// ___________________________________________________________________________//

#include "map.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

class MapProperties: public lagrangian::MapProperties
{
public:
    MapProperties(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step);

    bp::numeric::array GetXAxis() const;

    bp::numeric::array GetYAxis() const;
};

// ___________________________________________________________________________//

class MapOfFiniteLyapunovExponents: public lagrangian::map::FiniteLyapunovExponents
{
private:
    MapProperties map_properties_;

    typedef double (lagrangian::FiniteLyapunovExponents::*GetExponent)() const;

    bp::numeric::array GetMapOfExponents(const double nan,
            lagrangian::FiniteLyapunovExponents& fle,
            GetExponent pGetExponent);

public:
    MapOfFiniteLyapunovExponents(const int nx,
            const int ny,
            const double x_min,
            const double y_min,
            const double step);

    bp::numeric::array GetMapOfLambda1(const double fill_value,
            lagrangian::FiniteLyapunovExponents& fle);

    bp::numeric::array GetMapOfLambda2(const double fill_value,
            lagrangian::FiniteLyapunovExponents& fle);

    bp::numeric::array GetMapOfTheta1(const double fill_value,
            lagrangian::FiniteLyapunovExponents& fle);

    bp::numeric::array GetMapOfTheta2(const double fill_value,
            lagrangian::FiniteLyapunovExponents& fle);

    wrapper::MapProperties get_map_properties() const;
};

// ___________________________________________________________________________//

void MapPythonModule();

}