# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See GNU Lesser General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License along
# with lagrangian. If not, see <http://www.gnu.org/licenses/>.
cimport libcpp
cimport libcpp.string
cimport libcpp.vector
cimport libcpp.utility
cimport boost.date_time.gregorian
cimport boost.date_time.posix_time
cimport cpython.datetime
cimport cpython.ref

cdef extern from "lagrangian/axis.hpp" nogil:
    cdef cppclass AxisUnit "lagrangian::axis::Unit":
        AxisUnit()
        libcpp.bool call "operator()"(libcpp.string.string)

    cdef cppclass LatitudeUnit "lagrangian::axis::LatitudeUnit"(AxisUnit):
        LatitudeUnit()

    cdef cppclass LongitudeUnit "lagrangian::axis::LongitudeUnit"(AxisUnit):
        LongitudeUnit()


cdef extern from "lagrangian/axis.hpp" nogil:
    enum AxisType "lagrangian::Axis::Type":
        kUnknown "lagrangian::Axis::kUnknown"
        kLatitude "lagrangian::Axis::kLatitude"
        kLongitude "lagrangian::Axis::kLongitude"
        kTime "lagrangian::Axis::kTime"
        kX "lagrangian::Axis::kX"
        kY "lagrangian::Axis::kY"
    cdef cppclass Axis "lagrangian::Axis":
        Axis()
        Axis(libcpp.vector.vector[double] points,
             AxisType type,
             libcpp.string.string unit)
        AxisType get_type()
        double GetCoordinateValue(int) except+
        double GetMinValue()
        double GetMaxValue()
        size_t GetNumElements()
        libcpp.bool is_regular()
        int FindIndex(double)
        int FindIndexBounded(double)
        double Normalize(double, double)
        libcpp.bool get_units(libcpp.string.string)
        void Convert(libcpp.string.string) except+
        libcpp.bool FindIndexes(double, int, int)
        double get_start()
        double get_increment()
    libcpp.bool AxisCompare "operator!="(Axis, Axis)


cdef extern from "lagrangian/datetime.hpp" nogil:
    cdef cppclass DateTime "lagrangian::DateTime":
        DateTime()
        DateTime(boost.date_time.posix_time.ptime) except+
        DateTime(libcpp.string.string) except+
        int Year()
        int Month()
        int Day()
        int Hour()
        int Minute()
        int Second()
        int Microsecond()
        libcpp.string.string ToString(libcpp.string.string) except+


cdef extern from "field.hpp" nogil:
    enum UnitType "lagrangian::Field::UnitType":
        kMetric "lagrangian::Field::kMetric"
        kAngular "lagrangian::Field::kAngular"

    enum CoordinatesType "lagrangian::Field::CoordinatesType":
        kSphericalEquatorial "lagrangian::Field::kSphericalEquatorial"
        kCartesian "lagrangian::Field::kCartesian"

    cdef cppclass Field "lagrangian::Field":
        Field()
        libcpp.bool Compute(double, double, double, double, double) except+
        libcpp.bool Compute(double, double, double, double, double, CellProperties) except+
        UnitType get_unit_type()
        CoordinatesType get_coordinates_type()
        libcpp.string.string GetUnit() except+

    cdef cppclass WrappedField(Field):
        WrappedField()
        WrappedField(cpython.ref.PyObject*, UnitType, CoordinatesType)


cdef extern from "lagrangian/field/vonkarman.hpp" nogil:
    cdef cppclass Vonkarman "lagrangian::field::Vonkarman"(Field):
        Vonkarman()
        Vonkarman(double,
                  double,
                  double,
                  double,
                  double,
                  double,
                  double,
                  double)
        libcpp.bool Compute(double, double, double, double, double)


cdef extern from "lagrangian/field/time_serie.hpp" nogil:
    cdef cppclass TimeSerie "lagrangian::field::TimeSerie"(Field):
        TimeSerie(libcpp.string.string, UnitType, CoordinatesType, ReaderType) except+
        void Fetch(double, double) except+
        DateTime StartTime()
        DateTime EndTime()


cdef extern from "reader.hpp" nogil:
    cdef cppclass CellProperties "lagrangian::CellProperties":
        CellProperties()
        libcpp.bool Contains(double, double)
        void Update(double,
                    double,
                    double,
                    double,
                    int,
                    int,
                    int,
                    int)
        double x0()
        double x1()
        double y0()
        double y1()
        int ix0()
        int ix1()
        int iy0()
        int iy1()

    cdef cppclass Reader "lagrangian::Reader":
        void Open(libcpp.string.string) except+
        void Load(libcpp.string.string, libcpp.string.string) except+
        void Load(libcpp.string.string) except+
        double Interpolate(double,
                           double,
                           double) except+
        double Interpolate(double,
                           double,
                           double,
                           CellProperties) except+
        DateTime GetDateTime(libcpp.string.string) except+

    cdef cppclass WrappedReader(Reader):
        WrappedReader(cpython.ref.PyObject*)


cdef extern from "lagrangian/reader/factory.hpp" nogil:
    enum ReaderType "lagrangian::reader::Factory::Type":
        kNetCDF "lagrangian::reader::Factory::kNetCDF"

    cdef cppclass Factory "lagrangian::reader::Factory":
        pass

    cdef cppclass NetcdfReader "lagrangian::reader::Netcdf"(Reader):
        pass

    cdef Reader* NewReader"lagrangian::reader::Factory::NewReader"(ReaderType)


cdef extern from "lagrangian/runge_kutta.hpp" nogil:
    cdef cppclass RungeKutta "lagrangian::RungeKutta":
        RungeKutta(double, Field*)
        libcpp.bool Compute(double,
                            double,
                            double,
                            double,
                            double) except+
        libcpp.bool Compute(double,
                            double,
                            double,
                            double,
                            double,
                            CellProperties) except+


cdef extern from "lagrangian/stencil.hpp" nogil:
    cdef cppclass Iterator "lagrangian::Iterator":
        Iterator(double, double, double)
        libcpp.bool GoAfter()
        void operator++()
        double call "operator()"()
        double inc()

    cdef cppclass Position "lagrangian::Position":
        Position()
        double get_xi(size_t) except+
        double get_yi(size_t) except+
        double get_time()
        libcpp.bool get_completed()
        void set_completed()
        void Missing()
        libcpp.bool IsMissing()
        double MaxDistance()
        libcpp.bool Compute(RungeKutta,
                            Iterator,
                            CellProperties)
        void StrainTensor(double&, double&, double&, double&)

    cdef cppclass Triplet "lagrangian::Triplet"(Position):
        Triplet()
        Triplet(double, double, double, double, bool)

    cdef cppclass Quintuplet "lagrangian::Quintuplet"(Position):
        Quintuplet()
        Quintuplet(double, double, double, double, bool)


cdef extern from "lagrangian/integration.hpp" nogil:
    cdef cppclass Integration "lagrangian::Integration":
        Integration(DateTime,
                    DateTime,
                    boost.date_time.posix_time.time_duration,
                    Field*) except+
        Iterator GetIterator()
        void Fetch(double)
        libcpp.bool Compute(Iterator, double, double, double, double) except+

    cdef cppclass Path "lagrangian::Path"(Integration):
        Path(DateTime,
             DateTime,
             boost.date_time.posix_time.time_duration,
             Field*) except+

    enum Mode "lagrangian::FiniteLyapunovExponentsIntegration::Mode":
        kFSLE "lagrangian::FiniteLyapunovExponentsIntegration::kFSLE"
        kFTLE "lagrangian::FiniteLyapunovExponentsIntegration::kFTLE"

    enum Stencil "lagrangian::FiniteLyapunovExponentsIntegration::Stencil":
        kTriplet "lagrangian::FiniteLyapunovExponentsIntegration::kTriplet"
        kQuintuplet "lagrangian::FiniteLyapunovExponentsIntegration::kQuintuplet"

    cdef cppclass FiniteLyapunovExponents "lagrangian::FiniteLyapunovExponents":
        FiniteLyapunovExponents()
        void set_delta_t(double)
        void set_final_separation(double)
        void set_lambda1(double)
        void set_lambda2(double)
        void set_theta1(double)
        void set_theta2(double)

        double get_delta_t()
        double get_final_separation()
        double get_lambda1()
        double get_lambda2()
        double get_theta1()
        double get_theta2()

    cdef cppclass FiniteLyapunovExponentsIntegration "lagrangian::FiniteLyapunovExponentsIntegration"(Integration):
        FiniteLyapunovExponentsIntegration(DateTime,
                                           DateTime,
                                           boost.date_time.posix_time.time_duration,
                                           Mode,
                                           double,
                                           double,
                                           Field*) except+
        Position* SetInitialPoint(double, double, Stencil, bool) except+
        libcpp.bool Separation(Position*)
        Mode get_mode()
        libcpp.bool Compute(Iterator, Position*, CellProperties)
        libcpp.bool ComputeExponents(Position*, FiniteLyapunovExponents)

cdef extern from "lagrangian/map.hpp" nogil:
    cdef cppclass MapProperties "lagrangian::MapProperties":
        MapProperties(int, int, double, double, double)
        double GetXValue(int)
        double GetYValue(int)
        int get_nx()
        int get_ny()
        double get_step()
        double get_x_min()
        double get_y_min()

    cdef cppclass Map "lagrangian::Map"[T](MapProperties):
        Map()
        Map(int, int, double, double, double)
        T GetItem(int, int)

    cdef cppclass MapOfFiniteLyapunovExponents "lagrangian::MapOfFiniteLyapunovExponents":
        MapOfFiniteLyapunovExponents (int, int, double, double, double) except+
        void Initialize(FiniteLyapunovExponentsIntegration, Stencil) except+
        void Initialize(FiniteLyapunovExponentsIntegration, Reader*, Stencil) except+
        void Compute(FiniteLyapunovExponentsIntegration) except+
        Map[double]* GetMapOfLambda1(double, FiniteLyapunovExponentsIntegration) except+
        Map[double]* GetMapOfLambda2(double, FiniteLyapunovExponentsIntegration) except+
        Map[double]* GetMapOfTheta1(double, FiniteLyapunovExponentsIntegration) except+
        Map[double]* GetMapOfTheta2(double, FiniteLyapunovExponentsIntegration) except+
        Map[double]* GetMapOfDeltaT(double, FiniteLyapunovExponentsIntegration) except+
        Map[double]* GetMapOfFinalSeparation(double,
                                             FiniteLyapunovExponentsIntegration) except+


cdef extern from "lagrangian/trace.hpp" namespace "lagrangian" nogil:
    void Debug(libcpp.string.string);
    void SetVerbose(libcpp.bool);
    libcpp.string.string Version();


# Convert C++ object DateTime to a Python datetime object
cdef inline object to_pydatetime(DateTime datetime):
    return cpython.datetime.datetime_new(datetime.Year(),
                                         datetime.Month(),
                                         datetime.Day(),
                                         datetime.Hour(),
                                         datetime.Minute(),
                                         datetime.Second(),
                                         datetime.Microsecond(),
                                         None)


# Convert a Python datetime object to a C++ object DateTime
cdef inline DateTime from_pydatetime(cpython.datetime.datetime datetime):
    cdef:
        boost.date_time.gregorian.gregorian_date date = \
            boost.date_time.gregorian.gregorian_date(
                cpython.datetime.PyDateTime_GET_YEAR(datetime),
                cpython.datetime.PyDateTime_GET_MONTH(datetime),
                cpython.datetime.PyDateTime_GET_DAY(datetime))
        boost.date_time.posix_time.time_duration duration = \
            boost.date_time.posix_time.time_duration(
                cpython.datetime.PyDateTime_DATE_GET_HOUR(datetime),
                cpython.datetime.PyDateTime_DATE_GET_MINUTE(datetime),
                cpython.datetime.PyDateTime_DATE_GET_SECOND(datetime),
                0)
        boost.date_time.posix_time.ptime ptime

    duration += boost.date_time.posix_time.microseconds(
        cpython.datetime.PyDateTime_DATE_GET_MICROSECOND(datetime))
    ptime = boost.date_time.posix_time.ptime(date, duration)

    return DateTime(ptime)


# Extract microseconds from a boost::posix_time::time_duration instance
cdef inline long get_microseconds(
        boost.date_time.posix_time.time_duration duration):
    cdef:
        long ticks
        long fractional
        long result

    ticks = duration.ticks_per_second()
    fractional = duration.fractional_seconds()
    if ticks > 1000000:
        result = <long>(fractional / (ticks / 1000000))
    else:
        result = <long>(fractional * (1000000 / ticks))
    return result


# Convert a Python timedelta object to a C++ object boost::posix_time::time_duration
cdef inline boost.date_time.posix_time.time_duration from_pytimedelta(
        cpython.datetime.timedelta delta):
    cdef:
        int days
        libcpp.bool is_negative
        boost.date_time.posix_time.time_duration duration

    days = cpython.datetime.timedelta_days(delta)
    is_negative = days < 0
    if is_negative:
        days -= 1
    duration = (boost.date_time.posix_time.hours(24) *
                days +
                boost.date_time.posix_time.seconds(
                    cpython.datetime.timedelta_seconds(delta)) +
                boost.date_time.posix_time.microseconds(
                    cpython.datetime.timedelta_microseconds(delta)))
    if is_negative:
        duration = duration.invert_sign()
    return duration
