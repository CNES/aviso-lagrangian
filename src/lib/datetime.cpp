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

#include "lagrangian/datetime.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{

std::string DateTime::ToString(const std::string& format) const
{
    std::string s;
    boost::posix_time::time_facet* f =
            new boost::posix_time::time_facet(format.c_str());
    std::stringstream ss;

    ss.imbue(std::locale(ss.getloc(), f));
    ss << datetime_;
    // do not call "delete f"; f is a smart pointer !
    return ss.str();
}

}
