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

#include "lagrangian/field.hpp"
#include "lagrangian_api.h"

// ___________________________________________________________________________//

class WrappedField: public lagrangian::Field
{
private:
    PyObject* object_;
public:

    WrappedField(PyObject* object,
            const lagrangian::Field::UnitType unit_type =
                    lagrangian::Field::kMetric);

    virtual ~WrappedField();

    bool Compute(const double t,
            const double x,
            const double y,
            double& u,
            double& v,
            lagrangian::CellProperties& cell =
                    lagrangian::CellProperties::NONE()) const;
};
