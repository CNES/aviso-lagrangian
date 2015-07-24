#include "lagrangian/reader.hpp"
#include "lagrangian_api.h"

class WrappedReader: public lagrangian::Reader
{
private:
    PyObject* object_;
public:

    WrappedReader(PyObject* object);

    virtual ~WrappedReader();

    void Open(const std::string& filename);

    void Load(const std::string& name, const std::string& unit = "");

    double Interpolate(const double longitude,
            const double latitude,
            const double fill_value = 0,
            lagrangian::CellProperties& cell =
                    lagrangian::CellProperties::NONE()) const;

    lagrangian::DateTime GetDateTime(const std::string& name) const;
};
