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

#include "field_wrapper.h"

namespace wrapper
{

// ___________________________________________________________________________//

Field::Field(lagrangian::Field::UnitType const unit_type) :
    lagrangian::Field(unit_type), bp::wrapper<lagrangian::Field>()
{
}

bool Field::Compute(double const t,
        double const x,
        double const y,
        double& u,
        double &v) const
{
    // pure virtual
    throw;
}

// ___________________________________________________________________________//

VonkarmanField::VonkarmanField(lagrangian::field::Vonkarman const & arg) :
    lagrangian::field::Vonkarman(arg),
            bp::wrapper<lagrangian::field::Vonkarman>()
{
    // copy constructor
}

VonkarmanField::VonkarmanField(double const a,
        double const w,
        double const r0,
        double const tc,
        double const alpha,
        double const y0,
        double const l,
        double const u0) :
    lagrangian::field::Vonkarman(a, w, r0, tc, alpha, y0, l, u0),
            bp::wrapper<lagrangian::field::Vonkarman>()
{
}

bp::tuple VonkarmanField::WrapperCompute(double const t,
        double const x,
        double const y) const
{
    double u, v;

    bool result = lagrangian::field::Vonkarman::Compute(t, x, y, u, v);

    return bp::make_tuple(result, u, v);
}

// ___________________________________________________________________________//

TimeSerieField::TimeSerieField(lagrangian::field::TimeSerie const & arg) :
    lagrangian::field::TimeSerie(arg),
            bp::wrapper<lagrangian::field::TimeSerie>()
{
}

TimeSerieField::TimeSerieField(std::string const & configuration_file,
        lagrangian::Field::UnitType const unit_type,
        lagrangian::reader::Factory::Type reader_type) :
    lagrangian::field::TimeSerie(configuration_file, unit_type, reader_type),
            bp::wrapper<lagrangian::field::TimeSerie>()
{
}

bp::tuple TimeSerieField::WrapperCompute(double const t,
        double const x,
        double const y) const
{
    double u, v;

    bool result = lagrangian::field::TimeSerie::Compute(t, x, y, u, v);

    return bp::make_tuple(result, u, v);
}

// ___________________________________________________________________________//

void FieldPythonModule()
{
    // lagrangian::reader::Factory::Type
    bp::enum_<lagrangian::reader::Factory::Type>("ReaderType")
            .value("kNetCDF", lagrangian::reader::Factory::kNetCDF)
            .export_values();
    // lagrangian::Field
    {
        bp::class_<Field, boost::noncopyable>
                FieldExposer = bp::class_<Field, boost::noncopyable>(
                        "Field",
                        bp::init<lagrangian::Field::UnitType>((
                                bp::arg("unit_type"))));
        bp::enum_<lagrangian::Field::UnitType>("UnitType")
                .value("kMetric", lagrangian::Field::kMetric)
                .value("kAngular", lagrangian::Field::kAngular)
                .export_values();
        { //lagrangian::Field::GetUnit

            FieldExposer.def(
                "GetUnit",
                (std::string const(lagrangian::Field::*)() const)
                    (&lagrangian::Field::GetUnit));
        }
        { //lagrangian::Field::get_unit_type
            FieldExposer.def(
                "get_unit_type",
                (lagrangian::Field::UnitType const(lagrangian::Field::*)() const)
                    (&lagrangian::Field::get_unit_type));
        }
    }
    //
    // lagrangian::field::Vonkarman
    //
    bp::class_<VonkarmanField, bp::bases<lagrangian::Field> >(
            "VonkarmanField",
            bp::init<bp::optional<double,
                     double,
                     double,
                     double,
                     double,
                     double,
                     double,
                     double> >((
                             bp::arg("a") = 1,
                             bp::arg("w") = 35.06,
                             bp::arg("r0") = 0.35,
                             bp::arg("tc") = 1,
                             bp::arg("alpha") = 2,
                             bp::arg("y0") = 0.3,
                             bp::arg("l") = 2,
                             bp::arg("u0") = 14)))
    .def(
        "Compute",
        (bp::tuple(VonkarmanField::*)
            (double const,
             double const,
             double const) const)
                 (&VonkarmanField::WrapperCompute),
        (bp::arg("t"),
         bp::arg("x"),
         bp::arg("y")));
    //
    // lagrangian::TimeSerieField
    //
    bp::class_< TimeSerieField, bp::bases<lagrangian::Field> >(
            "TimeSerieField",
            bp::init<std::string const &,
                     bp::optional<lagrangian::Field::UnitType,
                         lagrangian::reader::Factory::Type> >((
                             bp::arg("configuration_file"),
                             bp::arg("unit_type") = lagrangian::Field::kMetric,
                             bp::arg("reader_type") = lagrangian::reader::Factory::kNetCDF)))
        .def(
            "Compute",
            (bp::tuple (TimeSerieField::*)
                (double const,
                 double const,
                 double const) const)
                     (&TimeSerieField::WrapperCompute),
            (bp::arg("t"),
             bp::arg("x"),
             bp::arg("y")))
        .def(
            "EndTime",
            (lagrangian::JulianDay (lagrangian::field::TimeSerie::*)() const)
                (&lagrangian::field::TimeSerie::EndTime))
        .def(
            "StartTime",
            (lagrangian::JulianDay (lagrangian::field::TimeSerie::*)() const)
                (&lagrangian::field::TimeSerie::StartTime));
}

} // namespace wrapper
