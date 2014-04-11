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

#include "axis_wrapper.h"

namespace wrapper
{

// ___________________________________________________________________________//

void AxisPythonModule()
{
    {
        bp::class_<lagrangian::axis::Unit>(
                "Unit",
                bp::init< >())    
            .def(
                "__call__",
                (bool (lagrangian::axis::Unit::*)(std::string const &) const)
                    (&lagrangian::axis::Unit::operator()),
                    (bp::arg("unit")));

        bp::class_<lagrangian::axis::LatitudeUnit,
            bp::bases< lagrangian::axis::Unit> >(
                "LatitudeUnit", bp::init< >());

        bp::class_<lagrangian::axis::LongitudeUnit,
            bp::bases< lagrangian::axis::Unit> >(
                "LongitudeUnit", bp::init< >());
    }
}

}
