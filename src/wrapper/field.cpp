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
    along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
 */
#include "field.hpp"

WrappedField::WrappedField(PyObject* object,
                           const lagrangian::Field::UnitType unit_type)
    : lagrangian::Field(unit_type), object_(nullptr) {
  if (import_lagrangian__core() != 0) {
    throw std::runtime_error("Import error");
  }
  object_ = object;
  Py_XINCREF(object_);
}

WrappedField::~WrappedField() {
  if (object_ != nullptr) {
    Py_XDECREF(object_);
  }
}

bool WrappedField::Compute(const double t, const double x, const double y,
                           double& u, double& v,
                           lagrangian::CellProperties& /*cell*/) const {
  std::string error;
  bool result;
  int rc = PythonFieldCompute(object_, t, x, y, &u, &v, &result, &error);
  if (rc != 0) {
    throw std::runtime_error(error);
  }
  return result;
}
