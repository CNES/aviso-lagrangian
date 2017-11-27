/*
    This file is part of lagrangian library.

    lagrangian is free software: you can redistribute it and/or modify
    it under the terms of GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lagrangian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of GNU Lesser General Public License
    along with lagrangian.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// ___________________________________________________________________________//

#include <boost/format.hpp>

// ___________________________________________________________________________//

namespace lagrangian {

/**
 * @brief Display a debugging message
 *
 * @param message Message to display
 */
void Debug(std::string const& message);

/**
 * @brief Enable or disable verbose mode
 *
 * @param value True to enable verbose mode
 */
void SetVerbose(const bool value);

/**
 * @brief Return the version number
 *
 * @return Version number
 */
std::string Version();

}  // namespace lagrangian
