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

#include "parameter_wrapper.h"

// ___________________________________________________________________________//

namespace wrapper
{

void ParameterPythonModule()
{
    { //lagrangian::Parameter
        bp::class_<lagrangian::Parameter> ParameterExposer = bp::class_<
                lagrangian::Parameter>("Parameter",
                bp::init<std::string const &>((bp::arg("filename"))));
        bp::scope ParameterScope(ParameterExposer);
        bp::implicitly_convertible<std::string const &, lagrangian::Parameter>();
        ParameterExposer.def( bp::init< >() );
        { //lagrangian::Parameter::Clear

            ParameterExposer.def(
                "Clear",
                (void (lagrangian::Parameter::*)
                        (std::string const &))
                            (&lagrangian::Parameter::Clear),
                (bp::arg("key")));

        }
        { //lagrangian::Parameter::Clear

            ParameterExposer.def(
                "Clear",
                (void (lagrangian::Parameter::*)
                        ())
                            (&lagrangian::Parameter::Clear));

        }
        { //lagrangian::Parameter::Exists

            ParameterExposer.def(
                "Exists",
                (bool (lagrangian::Parameter::*)
                        (std::string const & ) const)
                            (&lagrangian::Parameter::Exists),
                (bp::arg("key")));

        }
        { //lagrangian::Parameter::Keys

            ParameterExposer.def(
                "Keys",
                (std::vector< std::string > (lagrangian::Parameter::*)
                        () const)
                            (&lagrangian::Parameter::Keys));

        }
        { //lagrangian::Parameter::Load

            ParameterExposer.def(
                "Load",
                (void(lagrangian::Parameter::*)
                        (std::string const &))
                            (&lagrangian::Parameter::Load),
                (bp::arg("filename")));
        }
        { //lagrangian::Parameter::Size

            ParameterExposer.def(
                "Size",
                (unsigned int(lagrangian::Parameter::*)
                    (std::string const &) const)
                        (&lagrangian::Parameter::Size),
                (bp::arg("key")));
        }
        { //lagrangian::Parameter::Size

            ParameterExposer.def(
                "Size",
                (unsigned int (lagrangian::Parameter::*)
                    () const)
                        ( &::lagrangian::Parameter::Size ) );

        }
        { //lagrangian::Parameter::Values

            ParameterExposer.def(
                "Values",
                (std::vector<std::string>(lagrangian::Parameter::*)
                        (std::string const &) const)
                            (&lagrangian::Parameter::Values),
                (bp::arg("key")));
        }
        ParameterExposer.def( bp::self_ns::str( bp::self ) );
    }
}

}
