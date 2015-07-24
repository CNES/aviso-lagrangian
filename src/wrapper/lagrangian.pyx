# cython: c_string_type=unicode, c_string_encoding=utf8
# distutils: sources=src/wrapper/field.cpp src/wrapper/reader.cpp
cimport cpp_lagrangian
cimport cpython
cimport cpython.datetime
cimport cython
cimport cpp_lagrangian
cimport libcpp.string
cimport libcpp.vector
cimport numpy

# datetime module initialization
cpython.datetime.PyDateTime_IMPORT


# numpy initialization
numpy.import_array()


@cython.internal
cdef class AxisUnit:
    """
    Units known for a given type of axis.
    """
    cdef cpp_lagrangian.AxisUnit* wrapped

    def __dealloc__(self):
        del self.wrapped
    
    def __call__(self, str unit not None):
        """
        Checks if the unit can define this type of axis.
        """
        cdef libcpp.string.string cpp_unit = unit.encode('utf8')
        return self.wrapped.call(cpp_unit)


cdef class LatitudeUnit(AxisUnit):
    """
    Units setting out latitudes
    """
    def __cinit__(self):
        self.wrapped = new cpp_lagrangian.LatitudeUnit()


cdef class LongitudeUnit(AxisUnit):
    """
    Units setting out longitudes
    """
    def __cinit__(self):
        self.wrapped = new cpp_lagrangian.LongitudeUnit()


cpdef enum AxisType:
    kUnknown = cpp_lagrangian.kUnknown
    kLatitude = cpp_lagrangian.kLatitude
    kLongitude = cpp_lagrangian.kLongitude
    kTime = cpp_lagrangian.kTime
    kX = cpp_lagrangian.kX
    kY = cpp_lagrangian.kY


cdef class Axis:
    """
    A coordinate axis is a Variable that specifies one of the coordinates
    of a Variable's values.
 
    Mathematically it is a vector function F from index space to Sn: ::

        F(i, j, k,...) -> (S1, S2, ...Sn)

    where i, j, k are integers, and S is the set of reals (R).

    The components of F are just its coordinate axes: ::
    
        F = (A1, A2, ...An)
         A1(i, j, k, ...) -> S1
         A2(i, j, k, ...) -> S1
         An(i, j, k, ...) -> Sn
    """
    cdef cpp_lagrangian.Axis* wrapped

    @cython.boundscheck(False)
    def __cinit__(self,
                  numpy.ndarray[double, ndim=1] points not None,
                  AxisType axis_type,
                  str unit=None):
        cdef:
            libcpp.vector.vector[double] cpp_points
            libcpp.string.string cpp_unit
            size_t size
            size_t idx
        
        size = numpy.PyArray_SIZE(points)

        if unit is None:
            unit = ""

        cpp_unit = unit.encode('utf8')
        cpp_points = libcpp.vector.vector[double](size)

        for idx in range(size):
            cpp_points[idx] = points[idx]
        
        self.wrapped = new cpp_lagrangian.Axis(cpp_points,
                                               axis_type,
                                               cpp_unit)
    
    def __dealloc__(self):
        del self.wrapped
    
    @property
    def type(self):
        """
        Get type of axis
        """ 
        return self.wrapped.get_type()

    def get_coordinate_value(self, int index):
        """
        Get the ith coordinate value.
        """
        return self.wrapped.GetCoordinateValue(index)

    def get_min_value(self):
        """
        Get the minimum coordinate value
        """
        return self.wrapped.GetMinValue()

    def get_max_value(self):
        """
        Get the maximum coordinate value
        """
        return self.wrapped.GetMaxValue()

    def get_num_elements(self):
        """
        Get the number of values for this axis
        """
        return self.wrapped.GetNumElements()
    
    @property
    def is_regular(self):
        """
        The axis values are spaced regularly. Return true if: ::
        
            value(i) = self.start + i * self.increment
        """
        return self.wrapped.is_regular()
    
    def find_index(self, double coordinate):
        """
        Given a coordinate position, find what element contains it.
        This mean that: ::
             
             edge[i] <= pos < edge[i+1] (if values are ascending)
             edge[i] > pos >= edge[i+1] (if values are descending)
        """
        return self.wrapped.FindIndex(coordinate)

    def find_index_bounded(self, double coordinate):
        """
        Given a coordinate position, find what element contains it or
        is closest to it.
        """
        return self.wrapped.FindIndexBounded(coordinate)
    
    def normalize(self, double coordinate, double circle):
        """
        Standardization of longitude
        """
        return self.wrapped.Normalize(coordinate, circle)
    
    @property
    def units(self):
        """
        Get the unit string for this axis
        """
        cdef:
            libcpp.string.string cpp_units
            libcpp.bool result
        result = self.wrapped.get_units(cpp_units)
        return cpp_units if result else None
    
    def convert(self, str unit not None):
        """
        Converts the axis data from unit `self.units` to unit. Can
        be called only if the property `self.units` is not None
        """
        cdef libcpp.string.string cpp_unit

        cpp_unit = unit.encode('utf8')
        self.wrapped.Convert(cpp_unit)
    
    def find_indexes(self, double coordinate):
        """
        Given a coordinate position, find grids elements around it.
        This mean that: ::
        
            points[i0] <= coordinate < points[i1]
        """
        cdef int i0, i1
        
        if self.wrapped.FindIndexes(coordinate, i0, i1):
            return i0, i1
        return None
    
    @property
    def start(self):
        """
        Get starting value if is_regular
        """
        return self.wrapped.get_start()
    
    @property
    def increment(self):
        """
        Get increment value if is_regular
        """
        return self.wrapped.get_increment()
    
    def __richcmp__(Axis this, Axis other, int operator):
        cdef libcpp.bool result
        
        if other is None:
            return False
        result = cpp_lagrangian.AxisCompare(this.wrapped[0],
                                            other.wrapped[0])
        if operator == 2:
            return not result
        elif operator == 3:
            return result
        return NotImplemented


cdef class DateTime:
    """
    A datetime object is a single object containing all the information from a
    date object and a time object. Like a date object, datetime assumes the
    current Gregorian calendar extended in both directions; like a time object,
    datetime assumes there are exactly 3600*24 seconds in every day.
    """
    cdef cpp_lagrangian.DateTime* wrapped

    def __cinit__(self, object argument=None):
        cdef libcpp.string.string cpp_string

        self.wrapped = new cpp_lagrangian.DateTime()

        if argument is None:
            return

        if isinstance(argument, cpython.datetime.datetime):
            self.wrapped[0] = cpp_lagrangian.from_pydatetime(argument)
        elif isinstance(argument, str):
            cpp_string = argument.encode('utf8')
            self.wrapped[0] = cpp_lagrangian.DateTime(cpp_string)
        else:
            raise TypeError("a string or a datetime object is required")

    def __dealloc__(self):
        del self.wrapped

    def __str__(self):
        return self.wrapped.ToString("%Y-%m-%dT%M:%M:%s")

    def __call__(self):
        return cpp_lagrangian.to_pydatetime(self.wrapped[0])


# Utility function to check the implementation of a pure virtual method in
# the Python code
cdef get_python_function(object self, str method):
    if not hasattr(self, method):
        raise NotImplementedError(
            "unimplemented pure virtual method '%s'" % method)


# Call, from C++, a Python function that implement the pure virtual method
# lagrangian::Field::Compute(const double t,
#       const double x,
#       const double y,
#       double& u,
#       double& v,
#       lagrangian::CellProperties& cell=lagrangian::CellProperties::NONE())
cdef public api int PythonFieldCompute(object self,
                                       double t,
                                       double x,
                                       double y,
                                       double* u,
                                       double* v,
                                       libcpp.bool* result,
                                       libcpp.string.string* error):
    cdef:
        int rc = 0
        str function_name = "compute"
    try:
        function = getattr(self, function_name);
        u[0], v[0], result[0] = function(t, x, y)
        error[0] = ""
    except Exception as err:
        rc = 1
        err = "%r %s" % (self, err)
        error[0] = err.encode('UTF-8')
    finally:
        return rc


cpdef enum UnitType:
    kMetric = cpp_lagrangian.kMetric
    kAngular = cpp_lagrangian.kAngular


cdef class Field:
    """
    Abstract class defining a field where it is possible to calculate a
    speed
    """
    cdef cpp_lagrangian.Field* wrapped

    def __cinit__(self, *args):
        if type(self) is Field:
            raise RuntimeError("cannot instantiate abstract class")
    
    def __dealloc__(self):
        del self.wrapped

    @property
    def unit_type(self):
        """
        Unit type used by this field.
        """
        return self.wrapped.get_unit_type()

    def get_unit(self):
        """
        Unit used by this field.
        """
        cdef libcpp.string.string cpp_unit = self.wrapped.GetUnit()
        return cpp_unit


cdef class PythonField(Field):
    """
    Python base class for implementing the class a field where it is possible
    to calculate a speed.

    To implement this class, you must implement the method ``compute``
    respecting the following signature: ::

        def compute (double t, double x, double y):
            return (u, v, defined)

    where ``u`` and ``v`` are the velocities computed, and ``defined`` a boolean
    indicating whether the calculated velocities are valid or not.
    """
    def __cinit__(self, UnitType unit_type=cpp_lagrangian.kMetric):

        # Compute must be implemented in a derived class
        if not hasattr(self, "compute"):
            raise NotImplementedError(
                "unimplemented pure virtual method 'compute'")
        
        self.wrapped = new cpp_lagrangian.WrappedField(
            <cpython.ref.PyObject*>self, unit_type)


cdef class Vonkarman(Field):
    """
    Vonkarman field
    """
    def __cinit__(self,
                  double a = 1,
                  double w = 35.06,
                  double r0 = 0.35,
                  double tc = 1,
                  double alpha = 2,
                  double y0 = 0.3,
                  double l = 2,
                  double u0 = 14):
        self.wrapped = new cpp_lagrangian.Vonkarman(
            a, w, r0, tc, alpha, y0, l, u0)

    def compute(self, double t, double x, double y):
        """
        Compute the field to the spatiotemporal position wanted
        """
        cdef double u, v

        self.wrapped.Compute(t, x, y, u, v)
        return (u, v)


cdef class CellProperties:
    """
    Cell properties of the grid used for the interpolation.
    """
    cdef cpp_lagrangian.CellProperties* wrapped

    def __cinit__(self):
        self.wrapped = new cpp_lagrangian.CellProperties()

    def __dealloc__(self):
        del self.wrapped
   
    def contains(self, double x, double y):
        """
        Test if the coordinate is in the cell.
        """
        return self.wrapped.Contains(x, y)

    def update(self,
               double x0,
               double x1,
               double y0,
               double y1,
               int ix0,
               int ix1,
               int iy0,
               int iy1):
        """
        Update the cell properties
        """
        self.wrapped.Update(x0, x1, y0, y1, ix0, ix1, iy0, iy1)
   
    @property
    def x0(self):
        """
        Get the first longitude of the cell
        """
        return self.wrapped.x0()

    @property
    def x1(self):
        """
        Get the last longitude of the cell
        """
        return self.wrapped.x1()

    @property
    def y0(self):
        """
        Get the first latitude of the cell
        """
        return self.wrapped.y0()

    @property
    def y1(self):
        """
        Get the last latitude of the cell
        """
        return self.wrapped.y1()

    @property
    def ix0(self):
        """
        Get the index of the first longitude in the grid
        """
        return self.wrapped.ix0()

    @property
    def ix1(self):
        """
        Get the index of the last longitude in the grid
        """
        return self.wrapped.ix1()

    @property
    def iy0(self):
        """
        Get the index of the first latitude in the grid
        """
        return self.wrapped.iy0()

    @property
    def iy1(self):
        """
        Get the index of the last latitude in the grid
        """
        return self.wrapped.iy1()


cdef class RungeKutta:
    """
    Fourth-order Runge-Kutta method
    """
    cdef cpp_lagrangian.RungeKutta* wrapped
    cdef Field field

    def __cinit__(self, double size_of_interval, Field field):
        self.wrapped = new cpp_lagrangian.RungeKutta(size_of_interval,
                                                     field.wrapped)

        # Stores the Python object "Field " in the instance members, to be
        # sure that the C ++ object will not be destroyed before being used
        self.field = field

    def __dealloc__(self):
        del self.wrapped

    def compute(self,
                double t,
                double x,
                double y,
                CellProperties cell=None):
        """
        Move a point in a field
        """
        cdef:
            double xi, yi
            libcpp.bool defined
        if cell is None:
            defined = self.wrapped.Compute(t, x, y, xi, yi)
        else:
            defined = self.wrapped.Compute(t, x, y, xi, yi, cell.wrapped[0])
        return (xi, yi) if defined else None


cdef class Iterator:
    """
    Definition of an iterator over a time period
    """
    cdef cpp_lagrangian.Iterator* wrapped

    def __cinit__(self, double begin, double end, double inc):
        self.wrapped = new cpp_lagrangian.Iterator(begin, end, inc)

    @staticmethod
    cdef Iterator Null():
        return Iterator(0, 0, 1)

    def __iter__(self):
        cdef double ix

        while self.wrapped.GoAfter():
            yield self.wrapped.call()
            cython.operator.preincrement(self.wrapped[0])

    def __dealloc__(self):
        del self.wrapped


cdef class Position:
    """
    Define the position of N points Mₖ = (xₖ, yₖ): ::
     
                Mₖ₊₁
                |
        Mₖ₊ᵢ ⎯⎯ M₀ ⎯⎯  Mₖ
                |
                Mₖ₊ₙ
    """
    cdef cpp_lagrangian.Position* wrapped

    def __cinit__(self):
        if type(self) is Position:
            self.wrapped = new cpp_lagrangian.Position()

    def __dealloc__(self):
        del self.wrapped

    cdef void assign(self, cpp_lagrangian.Position* position):
        del self.wrapped
        self.wrapped = position

    def get_xi(self, int idx):
        """
        Get the longitude of the point idx
        """
        return self.wrapped.get_xi(idx)

    def get_yi(self, int idx):
        """
        Get the latitude of the point idx
        """
        return self.wrapped.get_yi(idx)

    @property
    def time(self):
        """
        Get the time at the end of the integration
        """
        return self.wrapped.get_time()

    @property
    def completed(self):
        """
        Test if the integration is over
        """
        return self.wrapped.get_completed()

    def set_completed(self):
        """
        Indicate that the integration is complete.
        """
        self.wrapped.set_completed()

    def missing(self):
        """
        Set the instance to represent a missing position.
        """
        self.wrapped.Missing()

    def is_missing(self):
        """
        Test if the integration is defined.
        """
        return self.wrapped.IsMissing()

    def max_distance(self):
        """
        Compute the distance max
        """
        return self.wrapped.MaxDistance()

    def compute(self,
                RungeKutta rk not None,
                Iterator it not None,
                CellProperties cell not None):
        """
        To move a particle with a velocity field.
        """
        return self.wrapped.Compute(rk.wrapped[0],
                                    it.wrapped[0],
                                    cell.wrapped[0])

    def strain_tensor(self):
        """
        TODO
        """
        cdef double a00, a01, a10, a11

        self.wrapped.StrainTensor(a00, a01, a10, a11)
        return (a00, a01, a10, a11)


cdef class Triplet(Position):
    """
    Define the position of 3 points
    """
    def __cinit__(self, double x, double y, double delta):
        self.wrapped = new cpp_lagrangian.Triplet(x, y, delta)


cdef class Quintuplet(Position):
    """
    Define the position of 5 points
    """
    def __cinit__(self, double x, double y, double delta):
        self.wrapped = new cpp_lagrangian.Quintuplet(x, y, delta)


cdef class AbstractIntegration:
    """
    Handles the time integration
    """
    cdef cpp_lagrangian.Integration* wrapped
    cdef Field field

    def __cinit__(self, *args):
        if type(self) is AbstractIntegration:
            raise RuntimeError("cannot instantiate abstract class")

    # Stores the Python object "Field " in the instance members, to be
    # sure that the C ++ object will not be destroyed before being used
    cdef void set_field(self, Field field):
        self.field = field

    def __dealloc__(self):
        del self.wrapped

    def get_iterator(self):
        """
        Return an iterator that describes the integration period
        """
        cdef:
            Iterator result

        result = Iterator.Null()
        result.wrapped[0] = self.wrapped.GetIterator()

        return result

    def fetch(self, double t):
        """
        Perform the tasks before a new time step (eg load grids required)
        """
        self.wrapped.Fetch(t)

    def compute(self, Iterator it , double x0, double y0):
        """
        Calculate the new position of the particle
        """
        cdef:
            double x1, y1
            libcpp.bool defined

        defined = self.wrapped.Compute(it.wrapped[0], x0, y0, x1, y1)
        return (x1, y1) if defined else None   



cdef class Integration(AbstractIntegration):
    """
    Handles the time integration
    """
    def __cinit__(self,
                  cpython.datetime.datetime start_time,
                  cpython.datetime.datetime end_time,
                  cpython.datetime.timedelta delta_t,
                  Field field):
        self.set_field(field)
        self.wrapped = new cpp_lagrangian.Integration(
            cpp_lagrangian.from_pydatetime(start_time),
            cpp_lagrangian.from_pydatetime(end_time),
            cpp_lagrangian.from_pytimedelta(delta_t),
            field.wrapped)


cdef class Path(AbstractIntegration):
    """
    Handles the movement of a particle using the Runge-Kutta method.
    """
    def __cinit__(self,
                  cpython.datetime.datetime start_time,
                  cpython.datetime.datetime end_time,
                  cpython.datetime.timedelta delta_t,
                  Field field):
        self.set_field(field)
        self.wrapped = new cpp_lagrangian.Path(
            cpp_lagrangian.from_pydatetime(start_time),
            cpp_lagrangian.from_pydatetime(end_time),
            cpp_lagrangian.from_pytimedelta(delta_t),
            field.wrapped)


cpdef enum Mode:
    kFSLE = cpp_lagrangian.kFSLE
    kFTLE = cpp_lagrangian.kFTLE


cpdef enum Stencil:
    kTriplet = cpp_lagrangian.kTriplet
    kQuintuplet = cpp_lagrangian.kQuintuplet


cdef class FiniteLyapunovExponents(AbstractIntegration):
    """
    Handles the computation of Lyapunov Exponent
    
    Finite Size Lyapunov Exponent (FSLE) is a scalar local notion that
    represents the rate of separation of initially neighbouring particles
    over a finite-time window [t₀, t₀ + T], where T is the time two
    particules need to be advected in order to be separated from a given
    distance d.
    
    Let x(t) = x(t; x₀, t₀) be the position of a lagrangian particle
    at time t, started at x₀ at t=t₀ and advected by the time-dependent
    fluid flow u(x, t).
    
    The Forward Finite-Time Lyapunov Exponent at a point x₀
    and for the advection time T is defined as the growth factor of
    the norm of the perturbation dx0 started around x₀ and advected
    by the flow after the finite advection time T.
    
    Maximal stretching occurs when dx0 is aligned with the eigenvector
    associated with the maximum eigenvalue δmax of the Cauchy-Green strain
    tensor Δ: ::
    
        Δ = [ ∇Φ₀ᵀ (x₀) ]^* [ ∇Φ₀ᵀ (x₀) ]
    
    where Φ₀ᵀ : x₀ ➜ x(t, x₀, t₀) is the flow map of the advection equation:
    it links the location x₀ of a lagragian particule at t=t₀ to its position
    x(t,x₀,t₀) at time t. (* denotes the transposition operator).
    
    FTLE is defined as ::
    
        σ = ( 1 / (2*T) ) * log( λmax( Δ ) )
    
    Finite-Size Lyapunov Exponent is similary defined: T is choosen so that
    neighbouring particules separate from a given distance d.
    
    Exponents(const Position& position) function implements the computation of
    the lyapunov exponents based on maximal and minimal eigenvalues and
    orientation of eigenvectors of Δ given the elements of ∇Φ₀ᵀ matrix.
    
    For more details see:
    
        1. G. Haller, Lagrangian coherent structures and the rate of strain in
           two-dimensional turbulence Phys. Fluids A 13 (2001) 3365-3385
           (http://georgehaller.com/reprints/approx.pdf) Remark: In this
           paper, FTLE is referred to as the Direct Lyapunov Exponent (DLE)
        
        2. http://mmae.iit.edu/shadden/LCS-tutorial/FTLE-derivation.html
    """
    def __cinit__(self,
                  cpython.datetime.datetime start_time,
                  cpython.datetime.datetime end_time,
                  cpython.datetime.timedelta delta_t,
                  Mode mode,
                  double min_separation,
                  double delta,
                  Field field):
        self.set_field(field)
        self.wrapped = new cpp_lagrangian.FiniteLyapunovExponents(
            cpp_lagrangian.from_pydatetime(start_time),
            cpp_lagrangian.from_pydatetime(end_time),
            cpp_lagrangian.from_pytimedelta(delta_t),
            mode,
            min_separation,
            delta,
            field.wrapped)

    def set_initial_point(self,
                          double x,
                          double y,
                          Stencil stencil):
        """
        Set the value of the initial point
        """
        cdef:
            cpp_lagrangian.FiniteLyapunovExponents* fle
            cpp_lagrangian.Position* cpp_position
            Position result

        fle = <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped
        cpp_position = fle.SetInitialPoint(x, y, stencil)

        result = Position()
        result.assign(cpp_position)

        return result

    def separation(self, Position position):
        """
        Determine whether the particle is deemed to be separate
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.Separation(position.wrapped)

    @property
    def mode(self):
        """
        Get mode of integration
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.get_mode()

    def compute(self, Iterator it, Position position, CellProperties cell):
        """
        Calculate the integration
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.Compute(it.wrapped[0],
                           position.wrapped,
                           cell.wrapped[0])

    def exponents(self, Position position):
        """
        Compute the eigenvalue and the orientation of the eigenvectors
        of the Cauchy-Green strain tensor
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.Exponents(position.wrapped)

    @property
    def lambda1(self):
        """
        Get the orientation of the eigenvectors associated to the
        maximum eigenvalues of Cauchy-Green strain tensor
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.get_lambda1()

    @property
    def lambda2(self):
        """
        Get the orientation of the eigenvectors associated to the
        minimum eigenvalues of Cauchy-Green strain tensor
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.get_lambda2()

    @property
    def theta1(self):
        """
        FLE associated to the maximum eigenvalues of Cauchy-Green
        strain tensor
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.get_theta1()

    @property
    def theta2(self):
        """
        FLE associated to the minimum eigenvalues of Cauchy-Green
        strain tensor
        """
        cdef cpp_lagrangian.FiniteLyapunovExponents* fle = \
            <cpp_lagrangian.FiniteLyapunovExponents*> self.wrapped

        return fle.get_theta2()


# Call, from C++, a Python function that implement the pure virtual method
# void lagrangian::Reader::Open(const std::string& filename) 
cdef public api int PythonReaderOpen(object self,
                                     libcpp.string.string filename,
                                     libcpp.string.string* error):
    cdef:
        int rc = 0
        str py_filename
    try:
        py_filename = filename
        function = getattr(self, "open");
        function(py_filename)
        error[0] = ""
    except Exception as err:
        rc = 1
        err = "%r %s" % (self, err)
        error[0] = err.encode('UTF-8')
    finally:
        return rc


# Call, from C++, a Python function that implement the pure virtual method
# void lagrangian::Reader::Load(const std::string& name,
#       const std::string& unit) 
cdef public api int PythonReaderLoad(object self,
                                     libcpp.string.string name,
                                     libcpp.string.string unit,
                                     libcpp.string.string* error):
    cdef:
        int rc = 0
        str py_name
        str py_unit
    try:
        py_name = name
        py_unit = unit
        function = getattr(self, "load");
        function(py_name, py_unit)
        error[0] = ""
    except Exception as err:
        rc = 1
        err = "%r %s" % (self, err)
        error[0] = err.encode('UTF-8')
    finally:
        return rc


# Call, from C++, a Python function that implement the pure virtual method
# double lagrangian::Reader::Interpolate(const double longitude,
#       const double latitude,
#       const double fill_value=0,
#       CellProperties& cell=CellProperties::NONE())
cdef public api int PythonFieldInterpolate(object self,
                                           double longitude,
                                           double latitude,
                                           double fill_value,
                                           cpp_lagrangian.CellProperties cell,
                                           double* result,
                                           libcpp.string.string* error):
    cdef:
        int rc = 0
        CellProperties py_cell
    try:
        py_cell = CellProperties()
        py_cell.wrapped[0] = cell
        function = getattr(self, "interpolate");
        result[0] = function(longitude, latitude, fill_value, py_cell)
        error[0] = ""
    except Exception as err:
        rc = 1
        err = "%r %s" % (self, err)
        error[0] = err.encode('UTF-8')
    finally:
        return rc

# Call, from C++, a Python function that implement the pure virtual method
# DateTime lagrangian::Reader::GetDateTime(const std::string& name)
cdef public api int PythonFieldGetDateTime(object self,
                                           libcpp.string.string name,
                                           cpp_lagrangian.DateTime* result,
                                           libcpp.string.string* error):
    cdef:
        int rc = 0
        str py_name
        object py_result
    try:
        py_name = name
        function = getattr(self, "get_datetime");
        py_result = function(py_name)
        result[0] = cpp_lagrangian.from_pydatetime(py_result)
        error[0] = ""
    except Exception as err:
        rc = 1
        err = "%r %s" % (self, err)
        error[0] = err.encode('UTF-8')
    finally:
        return rc


@cython.internal
cdef class AbstractReader:
    """
    Abstract class that defines a velocity reader fields.
    """
    cdef cpp_lagrangian.Reader* wrapped

    def __dealloc__(self):
        del self.wrapped

@cython.internal
cdef class Reader(AbstractReader):
    """
    TODO
    """
    def open(self, str filename not None):
        """
        Opens a file in read-only.
        """
        cdef libcpp.string.string cpp_string = filename.encode('utf8')
        self.wrapped.Open(cpp_string)
                
    def load(self, str name not None, str unit=None):
        """
        Load into memory grid data
        """
        cdef:
            libcpp.string.string cpp_name = name.encode('utf8')
            libcpp.string.string cpp_unit
        if unit is None:
            unit = ""
        cpp_unit = unit.encode('utf8')
        self.wrapped.Load(cpp_name, cpp_unit)
    
    def interpolate(self,
                    double longitude,
                    double latitude,
                    double fill_value=0,
                    CellProperties cell=None):
        """
        Computes the velocity of the grid point requested
        """
        cdef:
            cpp_lagrangian.CellProperties cpp_cell
        
        if cell is None:
            return self.wrapped.Interpolate(
                longitude,
                latitude,
                fill_value)    
        return self.wrapped.Interpolate(
            longitude,
            latitude,
            fill_value,
            cell.wrapped[0])    

    def get_datetime(self, str name not None):
        """
        Returns the date of the grid.
        """
        cdef libcpp.string.string cpp_string = name.encode('utf8')
        
        return cpp_lagrangian.to_pydatetime(
            self.wrapped.GetDateTime(cpp_string))


cdef class Netcdf(Reader):
    """
    Grid NetCDF CF reader.

    The grid must contain at least one variable and two vectors defining the
    axes of the longitudes and latitudes of the variable. For example : ::

        dimensions:
          y = 915 ;
          x = 1080 ;
        variables:
           double y(y) ;
               y:long_name = "Latitudes" ;
               y:units = "degrees_north" ;
           double x(x) ;
               x:long_name = "Longitudes" ;
               x:units = "degrees_east" ;
           float u(x, y) ;
               u:_FillValue = 999f ;
               u:long_name = "U" ;
               u:units = "cm/s" ;
               u:date = "2012-01-01 00:00:00.000000 UTC" ;
            float v(y, x) ;
               v:_FillValue = 999f ;
               v:long_name = "U" ;
               v:units = "cm/s" ;
               V:date = "2012-01-01 00:00:00.000000 UTC" ;

    .. note:: The variable to be read must set an attribute named "date" that
    define the date of data contained in the variable.
    """
    def __cinit__(self):
        self.wrapped = new cpp_lagrangian.NetcdfReader()


cdef class PythonReader(AbstractReader):
    """
    Python base class for implementing a velocity reader fields.
    """
    def __cinit__(self):
        
        self.wrapped = new cpp_lagrangian.WrappedReader(
            <cpython.ref.PyObject*>self)
        
        # pure virtual methods must be implemented in a derived class
        for method in ['open', 'load', 'interpolate', 'get_datetime']:
            if not hasattr(self, method):
                raise NotImplementedError(
                    "unimplemented pure virtual method '%s'" % method)


cpdef enum ReaderType:
    kNetCDF = cpp_lagrangian.kNetCDF


cdef class Factory:
    """
    Reader Factory
    """
    @staticmethod
    def new_reader(ReaderType reader_type):
        """
        Get an instance of a given reader
        """
        cdef Reader result = Reader()
        result.wrapped = cpp_lagrangian.NewReader(reader_type)
        return result


cdef class TimeSerie(Field):
    """
    Time series of velocity field
    """
    def __cinit__(self,
                  str ini not None,
                  UnitType unit_type=kMetric,
                  ReaderType reader_type=kNetCDF):
        cdef cpp_ini = ini.encode('utf8')
        self.wrapped = new cpp_lagrangian.TimeSerie(
            cpp_ini, unit_type, reader_type)

    def fetch(self, double t0, double t1):
        """
        Loads the grids used to interpolate the velocities in the
        interval [t0, t1]
        """
        cdef cpp_lagrangian.TimeSerie* ts

        ts = <cpp_lagrangian.TimeSerie*> self.wrapped
        ts.Fetch(t0, t1)

    def compute(self,
                double t,
                double x,
                double y,
                CellProperties cell=None):
        """
        Interpolates the velocity to the wanted spatio temporal position.
        """
        cdef:
            double u, v
            libcpp.bool defined

        if cell is None:
            defined = self.wrapped.Compute(t, x, y, u, v)
        else:
            defined = self.wrapped.Compute(t, x, y, u, v, cell.wrapped[0])

        return (u, v) if defined else None

    def start_time(self):
        """
        Returns the date of the first grid constituting the time series.
        """
        cdef cpp_lagrangian.TimeSerie* ts

        ts = <cpp_lagrangian.TimeSerie*> self.wrapped
        return cpp_lagrangian.to_pydatetime(ts.StartTime())

    def end_time(self):
        """
        Returns the date of the last grid constituting the time series.
        """
        cdef cpp_lagrangian.TimeSerie* ts

        ts = <cpp_lagrangian.TimeSerie*> self.wrapped
        return cpp_lagrangian.to_pydatetime(ts.EndTime())


cdef class MapProperties:
    """
    Properties of a regular grid
    """
    cdef cpp_lagrangian.MapProperties* wrapped

    def __cinit__(self,
                  int nx,
                  int ny,
                  double x_min,
                  double y_min,
                  double step):
        self.wrapped = new cpp_lagrangian.MapProperties(
            nx, ny, x_min, y_min, step)
    
    def __dealloc__(self):
        del self.wrapped

    def get_x_value(self, int ix):
        """
        Get the longitude value
        """
        return self.wrapped.GetXValue(ix)

    def get_y_value(self, int ix):
        """
        Get the latitude value
        """
        return self.wrapped.GetYValue(ix)

    @property
    def nx(self):
        """
        Get the number of longitudes in the grid
        """
        return self.wrapped.get_nx()

    @property
    def ny(self):
        """
        Get the number of latitudes in the grid
        """
        return self.wrapped.get_ny()

    @property
    def step(self):
        """
        Get the step between two consecutive longitudes and latitudes
        """
        return self.wrapped.get_step()

    @property
    def x_min(self):
        """
        Get the minimal longitude
        """
        return self.wrapped.get_x_min()

    @property
    def y_min(self):
        """
        Get the minimal latitude
        """
        return self.wrapped.get_y_min()

    @cython.boundscheck(False)
    def get_x_axis(self):
        cdef:
            numpy.ndarray result
            numpy.npy_intp dims[1]
            double* data
            int ix, iy

        dims[0] = self.wrapped.get_nx()
        result = numpy.PyArray_EMPTY(1,
                                     dims,
                                     numpy.NPY_DOUBLE,
                                     0)
        ptr = <double*>numpy.PyArray_DATA(result)
        for ix in range(self.wrapped.get_nx()):
            ptr[ix] = self.wrapped.GetXValue(ix);
        return result

    @cython.boundscheck(False)
    def get_y_axis(self):
        cdef:
            numpy.ndarray result
            numpy.npy_intp dims[1]
            double* data
            int ix, iy

        dims[0] = self.wrapped.get_ny()
        result = numpy.PyArray_EMPTY(1,
                                     dims,
                                     numpy.NPY_DOUBLE,
                                     0)
        ptr = <double*>numpy.PyArray_DATA(result)
        for ix in range(self.wrapped.get_ny()):
            ptr[ix] = self.wrapped.GetYValue(ix);
        return result


cdef class MapOfFiniteLyapunovExponents:
    """
    Handles a map of Finite Size or Time Lyapunov Exponents
    """
    cdef:
        cpp_lagrangian.MapOfFiniteLyapunovExponents* wrapped
        FiniteLyapunovExponents fle

    def __cinit__(self,
                  MapProperties map_properties,
                  FiniteLyapunovExponents fle,
                  Stencil stencil=kTriplet,
                  Netcdf netcdf_reader=None):
        cdef:
            cpp_lagrangian.FiniteLyapunovExponents* cpp_fle
            cpp_lagrangian.NetcdfReader* cpp_reader

        self.wrapped = new cpp_lagrangian.MapOfFiniteLyapunovExponents(
            map_properties.wrapped.get_nx(),
            map_properties.wrapped.get_ny(),
            map_properties.wrapped.get_x_min(),
            map_properties.wrapped.get_y_min(),
            map_properties.wrapped.get_step())
        self.fle = fle

        cpp_fle = <cpp_lagrangian.FiniteLyapunovExponents*> self.fle.wrapped
        cpp_reader = <cpp_lagrangian.NetcdfReader*> netcdf_reader.wrapped

        if netcdf_reader is not None:
            self.wrapped.Initialize(cpp_fle[0],
                                    cpp_reader[0],
                                    stencil)
        else:
            self.wrapped.Initialize(cpp_fle[0],
                                    stencil)

    def __dealloc__(self):
        del self.wrapped

    def compute(self):
        """
        """
        cdef:
            cpp_lagrangian.FiniteLyapunovExponents* cpp_fle

        cpp_fle = <cpp_lagrangian.FiniteLyapunovExponents*> self.fle.wrapped
        self.wrapped.Compute(cpp_fle[0])

    @cython.boundscheck(False)
    cdef numpy.ndarray get_map(self, cpp_lagrangian.Map[double]* map_of):
        # Utility function to return the computed matrix to Python
        cdef:
            numpy.ndarray result
            numpy.npy_intp dims[2]
            double* data
            int ix, iy

        try:
            dims[0] = map_of.get_nx()
            dims[1] = map_of.get_ny()

            result = numpy.PyArray_EMPTY(2,
                                         dims,
                                         numpy.NPY_DOUBLE,
                                         0)
            ptr = <double*>numpy.PyArray_DATA(result)

            for ix in range(map_of.get_nx()):
                for iy in range(map_of.get_ny()):
                    ptr[ix * map_of.get_ny() + iy] = map_of.GetItem(ix, iy)

            return result
        finally:
            del map_of

    def get_map_of_lambda1(self, double nan):
        """
        Get the map of the orientation of the eigenvectors associated
        to the maximum eigenvalues of Cauchy-Green strain tensor
        """
        cdef:
            cpp_lagrangian.FiniteLyapunovExponents* cpp_fle = \
                <cpp_lagrangian.FiniteLyapunovExponents*> self.fle.wrapped

        return self.get_map(self.wrapped.GetMapOfLambda1(nan, cpp_fle[0]))

    def get_map_of_lambda2(self, double nan):
        """
        Get the map of the orientation of the eigenvectors associated
        to the minimum eigenvalues of Cauchy-Green strain tensor
        """
        cdef:
            cpp_lagrangian.FiniteLyapunovExponents* cpp_fle = \
                <cpp_lagrangian.FiniteLyapunovExponents*> self.fle.wrapped

        return self.get_map(self.wrapped.GetMapOfLambda2(nan, cpp_fle[0]))

    def get_map_of_theta1(self, double nan):
        """
        Get the map of the FLE associated to the maximum eigenvalues of
        Cauchy-Green strain tensor
        """
        cdef:
            cpp_lagrangian.FiniteLyapunovExponents* cpp_fle = \
                <cpp_lagrangian.FiniteLyapunovExponents*> self.fle.wrapped

        return self.get_map(self.wrapped.GetMapOfTheta1(nan, cpp_fle[0]))

    def get_map_of_theta2(self, double nan):
        """
        Get the map of the FLE associated to the minimum eigenvalues of
        Cauchy-Green strain tensor
        """
        cdef:
            cpp_lagrangian.FiniteLyapunovExponents* cpp_fle = \
                <cpp_lagrangian.FiniteLyapunovExponents*> self.fle.wrapped

        return self.get_map(self.wrapped.GetMapOfTheta2(nan, cpp_fle[0]))

def debug(str msg not None):
    """
    Display a debugging message
    """
    cdef cpp_string = msg.encode('utf8')

    cpp_lagrangian.Debug(cpp_string)


def set_verbose(cpython.bool value):
    """
    Enable or disable verbose mode
    """
    cpp_lagrangian.SetVerbose(value)


def version():
    """
    Return the version number
    """
    return cpp_lagrangian.Version()

