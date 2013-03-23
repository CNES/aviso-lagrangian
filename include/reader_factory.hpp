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

#include "reader.h"
#include "netcdf_reader.hpp"

// ___________________________________________________________________________//

namespace lagrangian
{
namespace reader
{
class Factory
{
public:
    enum Type
    {
        kNetCDF
    };
    static Reader* NewReader(const Type type)
    {
        switch(type)
        {
        case kNetCDF:
            return new Netcdf();
        }
        throw std::invalid_argument("invalid ReaderFactory::Type value");
    }
};

} // namespace::reader
} // namespace::lagrangian