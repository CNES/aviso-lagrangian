#include "reader.hpp"

WrappedReader::WrappedReader(PyObject* object) :
        object_(NULL)
{
    if (import_lagrangian())
    {
        throw std::runtime_error("Import error");
    }
    object_ = object;
    Py_XINCREF(object_);
}

WrappedReader::~WrappedReader()
{
    if (object_)
        Py_XDECREF(object_);
}

void WrappedReader::Open(const std::string& filename)
{
    std::string error;
    int rc = PythonReaderOpen(object_, filename, &error);
    if (rc)
    {
        throw std::runtime_error(error);
    }
}

void WrappedReader::Load(const std::string& name, const std::string& unit)
{
    std::string error;
    int rc = PythonReaderLoad(object_, name, unit, &error);
    if (rc)
    {
        throw std::runtime_error(error);
    }
}

double WrappedReader::Interpolate(const double longitude,
        const double latitude,
        const double fill_value,
        lagrangian::CellProperties& cell) const
{
    std::string error;
    double result;
    int rc = PythonFieldInterpolate(object_, longitude, latitude, fill_value,
            cell, &result, &error);
    if (rc)
    {
        throw std::runtime_error(error);
    }
    return result;
}

lagrangian::DateTime WrappedReader::GetDateTime(const std::string& name) const
{
    std::string error;
    lagrangian::DateTime result;
    int rc = PythonFieldGetDateTime(object_, name, &result, &error);
    if (rc)
    {
        throw std::runtime_error(error);
    }
    return result;

}
