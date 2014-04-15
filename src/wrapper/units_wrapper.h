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
#include "units.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

// ___________________________________________________________________________//

struct Units : lagrangian::Units, bp::wrapper<lagrangian::Units>
{
    Units() : lagrangian::Units(), bp::wrapper<lagrangian::Units>()
    {
    }

    static bp::tuple GetConverter(const std::string& from,
        const std::string& to);
};

// ___________________________________________________________________________//

void UnitsPythonModule();

}