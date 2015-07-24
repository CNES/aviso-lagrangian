#include "field.hpp"

WrappedField::WrappedField(PyObject* object,
        const lagrangian::Field::UnitType unit_type) :
        lagrangian::Field(unit_type), object_(NULL)
{
    if (import_lagrangian())
    {
        throw std::runtime_error("Import error");
    }
    object_ = object;
    Py_XINCREF(object_);
}

WrappedField::~WrappedField()
{
    if (object_)
        Py_XDECREF(object_);
}

bool WrappedField::Compute(const double t,
        const double x,
        const double y,
        double& u,
        double& v,
        lagrangian::CellProperties& cell) const
{
    std::string error;
    bool result;
    int rc = PythonFieldCompute(object_, t, x, y, &u, &v, &result,
            &error);
    if (rc)
    {
        throw std::runtime_error(error);
    }
    return result;
}
