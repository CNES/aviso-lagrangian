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

#include "lagrangian/reader.hpp"
#include "lagrangian_api.h"

// ___________________________________________________________________________//

class WrappedReader: public lagrangian::Reader
{
private:
    PyObject* object_;
public:

    WrappedReader(PyObject* object);

    virtual ~WrappedReader();

    void Open(const std::string& filename);

    void Load(const std::string& name, const std::string& unit = "");

    double Interpolate(const double longitude,
            const double latitude,
            const double fill_value = 0,
            lagrangian::CellProperties& cell =
                    lagrangian::CellProperties::NONE()) const;

    lagrangian::DateTime GetDateTime(const std::string& name) const;
};
