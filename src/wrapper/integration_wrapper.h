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

#include "integration.hpp"

// ___________________________________________________________________________//

namespace bp = boost::python;

namespace wrapper
{

struct Path: lagrangian::Path, bp::wrapper<lagrangian::Path>
{
    Path(lagrangian::Path const & arg);

    Path(lagrangian::JulianDay const & start_time,
            lagrangian::JulianDay const & end_time,
            boost::posix_time::time_duration const & delta_t,
            lagrangian::Field* field);

    bp::tuple WrapperCompute(const lagrangian::Iterator& it,
            const double x0,
            const double y0) const;
};

struct FiniteLyapunovExponents: lagrangian::FiniteLyapunovExponents, bp::wrapper<
        lagrangian::FiniteLyapunovExponents>
{
    FiniteLyapunovExponents(lagrangian::FiniteLyapunovExponents const & arg);

    FiniteLyapunovExponents(lagrangian::JulianDay const & start_time,
            lagrangian::JulianDay const & end_time,
            boost::posix_time::time_duration const & delta_t,
            lagrangian::FiniteLyapunovExponents::Mode mode,
            double const min_separation,
            double const delta,
            lagrangian::Field* field);

    bool WrapperCompute(const lagrangian::Iterator& it,
            lagrangian::Position* const position) const;
};

struct Position : lagrangian::Position, bp::wrapper<
        lagrangian::Position >
{

    Position();

    virtual void StrainTensor(double & a00,
        double& a01,
        double& a10,
        double& a11 ) const;
};

struct Triplet : lagrangian::Triplet, bp::wrapper<
    lagrangian::Triplet >
{

    Triplet(lagrangian::Triplet const & arg );

    Triplet( );

    Triplet(double const x, double const y, double const delta);

    virtual void StrainTensor(double & a00,
        double& a01,
        double& a10,
        double& a11) const;
    
    
    void default_StrainTensor(double & a00,
        double& a01,
        double& a10,
        double& a11) const;
};

struct Quintuplet : lagrangian::Quintuplet, bp::wrapper<
    lagrangian::Quintuplet >
{

    Quintuplet(lagrangian::Quintuplet const & arg );

    Quintuplet( );

    Quintuplet(double const x, double const y, double const delta);

    virtual void StrainTensor(double & a00,
        double& a01,
        double& a10,
        double& a11) const;
    
    
    void default_StrainTensor(double& a00,
        double& a01,
        double& a10,
        double& a11) const;
};

// ___________________________________________________________________________//

void IntegrationPythonModule();

}