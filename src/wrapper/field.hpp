#include "lagrangian/field.hpp"
#include "lagrangian_api.h"

class WrappedField: public lagrangian::Field
{
private:
    PyObject* object_;
public:

    WrappedField(PyObject* object,
            const lagrangian::Field::UnitType unit_type =
                    lagrangian::Field::kMetric);

    virtual ~WrappedField();

    bool Compute(const double t,
            const double x,
            const double y,
            double& u,
            double& v,
            lagrangian::CellProperties& cell =
                    lagrangian::CellProperties::NONE()) const;
};
