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

#include "field.hpp"
#include "vonkarman_field.hpp"
#include "time_serie_field.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

// ___________________________________________________________________________//

namespace wrapper
{

struct Field: lagrangian::Field, bp::wrapper<lagrangian::Field>
{
    Field(lagrangian::Field::UnitType const unit_type);

    virtual bool Compute(double const t,
            double const x,
            double const y,
            double & u,
            double & v) const;
};

// ___________________________________________________________________________//

struct VonkarmanField: lagrangian::field::Vonkarman, bp::wrapper<
        lagrangian::field::Vonkarman>
{
    VonkarmanField(lagrangian::field::Vonkarman const & arg);

    VonkarmanField(double const a = 1,
            double const w = 35.06,
            double const r0 = 0.35,
            double const tc = 1,
            double const alpha = 2,
            double const y0 = 0.3,
            double const l = 2,
            double const u0 = 14);

    bp::tuple WrapperCompute(double const t,
            double const x,
            double const y) const;
};

// ___________________________________________________________________________//

struct TimeSerieField: lagrangian::field::TimeSerie, bp::wrapper<
        lagrangian::field::TimeSerie>
{

    TimeSerieField(lagrangian::field::TimeSerie const & arg);

    TimeSerieField(std::string const & configuration_file,
            lagrangian::Field::UnitType const unit_type =
                    lagrangian::Field::kMetric,
            lagrangian::reader::Factory::Type reader_type =
                    lagrangian::reader::Factory::kNetCDF);

    bp::tuple WrapperCompute(double const t,
            double const x,
            double const y) const;
};

void FieldPythonModule();

}