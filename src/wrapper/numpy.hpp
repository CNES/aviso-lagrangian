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

#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#define NO_IMPORT_ARRAY

#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <numpy/noprefix.h>
#include <vector>

namespace bp = boost::python;

namespace wrapper
{

bp::numeric::array MakeVector(int n, PyArray_TYPES t = PyArray_DOUBLE)
{
    bp::object obj(bp::handle<>(PyArray_FromDims(1, &n, t)));
    return bp::extract<bp::numeric::array>(obj);
}

bp::numeric::array MakeMatrix(std::vector<int> dimens,
        PyArray_TYPES t = PyArray_DOUBLE)
{
    bp::object obj(bp::handle<>(PyArray_FromDims(dimens.size(),
            &dimens[0],
            t)));
    return bp::extract<bp::numeric::array>(obj);
}

}