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

#include "date_wrapper.h"
#include "field_wrapper.h"
#include "integration_wrapper.h"
#include "reader_wrapper.h"
#include "map_wrapper.h"

#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include <numpy/noprefix.h>

BOOST_PYTHON_MODULE(lagrangian)
{
    PyDateTime_IMPORT;

    import_array();
    bp::numeric::array::set_module_and_type("numpy", "ndarray");

    wrapper::PtimeFromPythonDatetime();
    wrapper::TimeDurationFromPython();

    bp::to_python_converter<const boost::posix_time::ptime,
        wrapper::PtimeToPythonDatetime>();
    bp::to_python_converter<const boost::posix_time::time_duration,
        wrapper::TimeDurationToPython>();

    wrapper::DatePythonModule();
    wrapper::FieldPythonModule();
    wrapper::ReaderPythonModule();
    wrapper::IntegrationPythonModule();
    wrapper::MapPythonModule();
}
