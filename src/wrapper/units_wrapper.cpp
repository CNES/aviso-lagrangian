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

#include "units_wrapper.h"

// ___________________________________________________________________________//

namespace wrapper
{

bp::tuple Units::GetConverter(const std::string& from,
    const std::string& to)
{
  double offset, scale;

  lagrangian::Units::GetConverter(from, to, offset, scale);

  return bp::make_tuple(offset, scale);
}

// ___________________________________________________________________________//

void UnitsPythonModule()
{
    bp::class_< Units, boost::noncopyable >( "Units", bp::init< >() )
        .def(
            "AreConvertible",
            (bool (*)
                (std::string const &, std::string const &))
                    (&lagrangian::Units::AreConvertible),
            (bp::arg("unit1"), bp::arg("unit2")))
        .def(
            "GetConverter",
            (bp::tuple (*)
                (std::string const &, std::string const &))
                    (Units::GetConverter),
            (bp::arg("from"), bp::arg("to")))
        .def(
            "IsTime",
            (bool (*)
                (std::string const &))
                    (lagrangian::Units::IsTime),
            (bp::arg("unit")))
        .staticmethod("AreConvertible")
        .staticmethod("GetConverter")
        .staticmethod("IsTime");
}

} // namespace wrapper