# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify
# it under the terms of GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# lagrangian is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License
# along with lagrangian. If not, see <http://www.gnu.org/licenses/>.
import argparse
import datetime
import pickle
import sys
import time
from fractions import Fraction
from typing import Any, Dict

import dateutil.parser
import lagrangian
import netCDF4
import numpy

try:
    import dask.array
    import dask.distributed

    HAVE_DASK = True
except ImportError:
    HAVE_DASK = False

SYSTEM_UNITS = dict(
    metric=lagrangian.UnitType.METRIC, angular=lagrangian.UnitType.ANGULAR
)

STENCIL = dict(
    triplet=lagrangian.Stencil.TRIPLET, quintuplet=lagrangian.Stencil.QUINTUPLET
)

MODE = dict(fsle=lagrangian.IntegrationMode.FSLE, ftle=lagrangian.IntegrationMode.FTLE)


def timedelta_type(value: str) -> datetime.timedelta:
    """The option should define a time duration"""
    try:
        days = float(value)
    except ValueError:
        raise argparse.ArgumentTypeError("invalid float value: %r" % value)
    return datetime.timedelta(days=days)


def datetime_type(value: str) -> datetime.datetime:
    """The option should define a datetime"""
    try:
        date = dateutil.parser.parse(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError("invalid date time %r: %s" % (value, error))
    return date


class TimeDirection(object):
    """Define the time integration direction"""

    BACKWARD = 0
    FORWARD = 1

    @staticmethod
    def default() -> str:
        """Return the default time integration direction"""
        return "backward"

    @classmethod
    def choices(cls) -> Dict[str, int]:
        """Return a hash that represents the possible choices for the
        integration time.
        """
        return dict(backward=cls.BACKWARD, forward=cls.FORWARD)


def positive_value(value: str) -> float:
    """Check options for defining values strictly positive."""
    try:
        real = float(value)
    except ValueError:
        msg = "invalid float value: %r" % value
        raise argparse.ArgumentTypeError(msg)
    if real < 0:
        msg = "value must be positive: %f" % value
        raise argparse.ArgumentTypeError(msg)
    return real


def usage():
    """
    Program syntax
    """
    parser = argparse.ArgumentParser(description="Compute Map of FLE", add_help=False)
    grid = parser.add_argument_group(
        "grid arguments", "Set the grid properties to create"
    )
    grid.add_argument(
        "--resolution",
        help="output grid resolution in degrees as a fraction",
        type=str,
        metavar="STEP",
        default="1/4",
    )
    grid.add_argument("--x_min", help="x min in degrees", type=float, default=-180)
    grid.add_argument("--x_max", help="x max in degrees", type=float, default=180)
    grid.add_argument("--y_min", help="y min in degrees", type=float, default=-60)
    grid.add_argument("--y_max", help="y max in degrees", type=float, default=60)
    grid.add_argument("--ny", help="number of latitudes", type=int, default=1441)
    grid.add_argument("--nx", help="number of longitudes", type=int, default=4321)

    integration = parser.add_argument_group(
        "integration arguments", "Define integration"
    )
    integration.add_argument(
        "--mode",
        help="define the integration mode: "
        "fsle to compute Finite Size "
        "Lyapunov Exponent, ftle to compute "
        "Finite Time Lyapunov Exponent.",
        choices=MODE.keys(),
        default="fsle",
    )
    integration.add_argument(
        "--time_direction",
        help="time integration direction",
        choices=TimeDirection.choices().keys(),
        default=TimeDirection.default(),
    )
    integration.add_argument(
        "--stencil",
        help="type of stencil used to compute the finite " "difference.",
        choices=STENCIL.keys(),
        default="triplet",
    )
    integration.add_argument(
        "--initial_separation",
        help="initial separation in degrees of " "neighbouring particules",
        type=positive_value,
        metavar="DELTA",
        default=None,
    )
    integration.add_argument(
        "--advection_time",
        help="time of advection in number of days "
        "elapsed since the beginning of the "
        "simulation. This option set the maximum "
        "time of advection in FSLE mode",
        default=datetime.timedelta(days=200),
        metavar="DURATION",
        type=timedelta_type,
    )
    integration.add_argument(
        "--final_separation",
        help="maximum final separation in degrees",
        type=positive_value,
        metavar="DELTA",
        default=-1,
    )
    integration.add_argument(
        "--integration_time_step",
        help="particles time integration step in hours",
        type=positive_value,
        metavar="DURATION",
        default=6,
    )
    integration.add_argument(
        "--diagnostic",
        help="write additional variables containing  "
        "diagnostics, depending on calculation mode. "
        "These variables are the effective final "
        "separation distance and advection time, in "
        "FSLE mode, or the effective final separation "
        "distance, in FTLE mode.",
        action="store_true",
    )
    integration.add_argument(
        "--threads",
        type=int,
        default=0,
        help="number of threads to use for the "
        "computation. If 0 all CPUs are used. If 1 "
        "is given, no parallel computing code is "
        "used at all.",
    )

    if HAVE_DASK:
        cluster = parser.add_argument_group(
            "Runs the calculation on a cluster (divides the calculation by "
            "geographical area)."
        )
        cluster.add_argument(
            "--scheduler_file",
            help="Path to a file with DASK scheduler " "information",
            metavar="PATH",
            default=None,
        )
        cluster.add_argument(
            "--local-cluster",
            help="Use a dask local cluster for testing purpose",
            action="store_true",
        )

    data = parser.add_argument_group(
        "reader arguments", "Set options of the NetCDF reader."
    )
    data.add_argument(
        "--unit",
        help="system of units for velocity",
        choices=SYSTEM_UNITS.keys(),
        default="metric",
    )
    data.add_argument(
        "--mask",
        help="netCDF grid for fixing undefined cells",
        nargs=2,
        metavar=("PATH", "VARNAME"),
    )

    parser.add_argument("--verbose", help="Verbose mode", action="store_true")
    parser.add_argument(
        "--version", help="print the version number and exit", action="store_true"
    )

    args, _ = parser.parse_known_args()
    if args.version:
        print(lagrangian.version())
        sys.exit(0)

    parser.add_argument(
        "--help",
        "-h",
        action="help",
        default=argparse.SUPPRESS,
        help="show this help message and exit",
    )
    parser.add_argument("configuration", help="configuration file")
    parser.add_argument("output", help="map of FLE in NetCDF format")
    parser.add_argument(
        "start_time", help="start of the integration", type=datetime_type
    )

    args = parser.parse_args()
    if (
        MODE[args.mode] == lagrangian.IntegrationMode.FTLE
        and args.final_separation != -1
    ):
        parser.error("argument --final_separation not allowed in FTLE " "mode")
    return args


def convert_from_sec_to_day_inv(
    array: numpy.ndarray, fill_value: float
) -> numpy.ndarray:
    """Converting values from second^{-1} to days^{-1}"""
    result = numpy.ma.masked_equal(array, fill_value)
    result *= 86400
    return result.data


def convert_from_sec_to_day(array: numpy.ndarray, fill_value: float) -> numpy.ndarray:
    """Converting values from second to days"""
    result = numpy.ma.masked_equal(array, fill_value)
    result /= 86400
    return result.data


class Inherit:
    """Serialization of C++ classes to make them compatible with Dask."""

    BASE: Any = None

    @staticmethod
    def _get_args(args, kwargs):
        args = list(args)
        for idx, item in enumerate(args):
            if isinstance(item, Inherit):
                args[idx] = item._base
        for k, v in enumerate(kwargs.items()):
            if isinstance(v, Inherit):
                kwargs[k] = v._base
        return tuple(args), kwargs

    def __init__(self, *args, **kwargs) -> None:
        self._state = (args, kwargs)
        args, kwargs = self._get_args(args, kwargs)
        self._base = self.BASE(*args, **kwargs)

    def __getattr__(self, name: str) -> Any:
        return getattr(self._base, name)

    def __getstate__(self) -> bytes:
        return pickle.dumps(self._state)

    def __setstate__(self, state: bytes) -> None:
        self._state = pickle.loads(state)
        args, kwargs = self._get_args(*self._state)
        self._base = self.BASE(*args, **kwargs)


class MapProperties(Inherit):
    """Derives class "lagrangian.MapProperties" in order to serialize this
    object."""

    BASE = lagrangian.MapProperties


class TimeSerie(Inherit):
    """Derives class "lagrangian.field.TimeSerie" in order to serialize
    this object."""

    BASE = lagrangian.field.TimeSerie


class FiniteLyapunovExponentsIntegration(Inherit):
    """Derives class "lagrangian.FiniteLyapunovExponentsIntegration" in order
    to serialize this object."""

    BASE = lagrangian.FiniteLyapunovExponentsIntegration


class MapOfFiniteLyapunovExponents(Inherit):
    """Derives class "lagrangian.MapOfFiniteLyapunovExponents" in order
    to serialize this object."""

    BASE = lagrangian.MapOfFiniteLyapunovExponents


def check_period(
    ts: TimeSerie, start_time: datetime.datetime, end_time: datetime.datetime
) -> None:
    """
    Check the consistency of the defined period with the time series
    """
    if start_time > end_time:
        start_time, end_time = end_time, start_time

    if start_time < ts.start_time():
        raise RuntimeError(
            "The start date (%s) is before the beginning "
            "of the time series (%s)"
            % (
                start_time.strftime("%Y-%m-%dT%H:%M:%S"),
                ts.start_time().strftime("%Y-%m-%dT%H:%M:%S"),
            )
        )

    if end_time > ts.end_time():
        raise RuntimeError(
            "The end date (%s) is after the ending "
            "of the time series (%s)"
            % (
                end_time.strftime("%Y-%m-%dT%H:%M:%S"),
                ts.end_time().strftime("%Y-%m-%dT%H:%M:%S"),
            )
        )


def worker_task(
    args: argparse.Namespace,
    fle: FiniteLyapunovExponentsIntegration,
    map_properties: MapProperties,
    threads: int,
):

    # The nodes of the grid result, located on land are undefined. To speed
    # up the calculation we use a external grid to remove these cells from
    # the calculation.
    if args.mask:
        reader = lagrangian.reader.NetCDF()
        reader.open(args.mask[0])
        reader.load(args.mask[1])
    else:
        reader = None

    # Initializes the map to process
    map_of_fle = MapOfFiniteLyapunovExponents(
        map_properties, fle, STENCIL[args.stencil], reader
    )

    # Computes map
    map_of_fle.compute(threads)

    result = [
        map_of_fle.map_of_theta1(),
        map_of_fle.map_of_theta2(),
        map_of_fle.map_of_lambda1(),
        map_of_fle.map_of_lambda2(),
    ]
    if args.diagnostic:
        result += [map_of_fle.map_of_final_separation(), map_of_fle.map_of_delta_t()]
    return numpy.stack(result)


def build_dask_array(
    args: argparse.Namespace,
    fsle: FiniteLyapunovExponentsIntegration,
    map_properties: MapProperties,
    workers: int,
    threads_per_worker: int,
) -> "dask.array.Array":  # type: ignore
    x_axis = map_properties.x_axis()
    y_axis = map_properties.y_axis()
    y_chunks = numpy.array_split(y_axis, workers)
    chunks = (
        (6 if args.diagnostic else 4,),
        (x_axis.size,),
        tuple(item.size for item in y_chunks),
    )
    dsk = dict()
    name = "lagrangian"
    for (i, y_chunk) in enumerate(y_chunks):
        map_properties_ = MapProperties(
            x_axis.size, len(y_chunk), x_axis[0], y_chunk[0], map_properties.step
        )
        dsk[(name, 0, 0, i)] = (
            worker_task,
            args,
            fsle,
            map_properties_,
            threads_per_worker,
        )

    return dask.array.Array(dsk, name, chunks, "float64")  # type: ignore


def write_netcdf(
    args: argparse.Namespace,
    exponents: numpy.ndarray,
    map_properties: MapProperties,
    nx: int,
    ny: int,
    start_time: datetime.datetime,
):
    """Write the NetCDF product"""
    # Fill value for double in NetCDF file
    NC_FILL_DOUBLE = netCDF4.default_fillvals["f8"]

    # Creates the NetCDF file
    rootgrp = netCDF4.Dataset(args.output, "w", format="NETCDF4")
    rootgrp.createDimension("lon", nx)
    rootgrp.createDimension("lat", ny)
    rootgrp.title = "Map of %s" % args.mode.upper()

    # Write parameter values used for the integration
    for item in args._get_kwargs():
        name, value = item
        setattr(rootgrp, name, str(value))

    x_axis = rootgrp.createVariable("lon", "f8", ("lon",))
    x_axis.standard_name = "longitude"
    x_axis.units = "degrees_east"
    x_axis.axis = "X"
    x_axis[:] = map_properties.x_axis()

    y_axis = rootgrp.createVariable("lat", "f8", ("lat",))
    y_axis.standard_name = "latitude"
    y_axis.units = "degrees_north"
    y_axis.axis = "Y"
    y_axis[:] = map_properties.y_axis()

    theta1 = rootgrp.createVariable(
        "theta1",
        "f8",
        (
            "lon",
            "lat",
        ),
        fill_value=NC_FILL_DOUBLE,
    )
    theta1.long_name = (
        "Orientation of the eigenvectors associated to the"
        "maximum eigenvalues of Cauchy-Green strain tensor"
    )
    theta1.units = "degree"
    theta1[:] = exponents[0, :]

    theta2 = rootgrp.createVariable(
        "theta2",
        "f8",
        (
            "lon",
            "lat",
        ),
        fill_value=NC_FILL_DOUBLE,
    )
    theta2.long_name = (
        "Orientation of the eigenvectors associated to the"
        "minimum eigenvalues of Cauchy-Green strain tensor"
    )
    theta2.units = "degree"
    theta2[:] = exponents[1, :]

    lambda1 = rootgrp.createVariable(
        "lambda1",
        "f8",
        (
            "lon",
            "lat",
        ),
        fill_value=NC_FILL_DOUBLE,
    )
    lambda1.long_name = (
        "FLE associated to the maximum eigenvalues of " "Cauchy-Green strain tensor"
    )
    lambda1.units = "1/day"
    lambda1[:] = convert_from_sec_to_day_inv(exponents[2, :], NC_FILL_DOUBLE)

    lambda2 = rootgrp.createVariable(
        "lambda2",
        "f8",
        (
            "lon",
            "lat",
        ),
        fill_value=NC_FILL_DOUBLE,
    )
    lambda2.long_name = (
        "FLE associated to the minimum eigenvalues of " "Cauchy-Green strain tensor"
    )
    lambda2.units = "1/day"
    lambda2[:] = convert_from_sec_to_day_inv(exponents[3, :], NC_FILL_DOUBLE)

    if args.diagnostic:
        separation_distance = rootgrp.createVariable(
            "separation_distance",
            "f8",
            (
                "lon",
                "lat",
            ),
            fill_value=NC_FILL_DOUBLE,
        )
        separation_distance.long_name = "effective final separation distance"
        separation_distance.units = "degree"
        separation_distance[:] = exponents[4, :]

        if MODE[args.mode] == lagrangian.IntegrationMode.FSLE:
            advection_time = rootgrp.createVariable(
                "advection_time",
                "f8",
                (
                    "lon",
                    "lat",
                ),
                fill_value=NC_FILL_DOUBLE,
            )
            advection_time.long_name = "actual advection time"
            advection_time.units = (
                "number of days elapsed " "since %s" % start_time.isoformat()
            )
            advection_time[:] = convert_from_sec_to_day(exponents[5, :], NC_FILL_DOUBLE)

    rootgrp.close()


def main():
    """
    Main function
    """
    # Initializes arguments from command line
    args = usage()

    # Set debug
    lagrangian.set_verbose(args.verbose)

    # Display set parameter values for the integration
    for item in args._get_kwargs():
        option, value = item
        lagrangian.debug("%s: %s" % (option, value))

    # Check X & Y range
    if not args.x_min < args.x_max:
        raise RuntimeError("Invalid definition of x range.")
    if not args.y_min < args.y_max:
        raise RuntimeError("Invalid definition of y range.")

    # Initializes the time series to process
    ts = TimeSerie(args.configuration, SYSTEM_UNITS[args.unit])
    delta = datetime.timedelta(0, args.integration_time_step * 60 * 60)

    start_time = args.start_time

    # Calculate the end of the integration depending on the advection time
    # direction
    if TimeDirection.choices()[args.time_direction] == TimeDirection.BACKWARD:
        end_time = start_time - args.advection_time
    else:
        end_time = start_time + args.advection_time

    check_period(ts, start_time, end_time)

    # nx = int((args.x_max - args.x_min) / args.resolution) + 1
    # ny = int((args.y_max - args.y_min) / args.resolution) + 1

    nx = args.nx
    ny = args.ny

    args.resolution = float(Fraction(args.resolution))

    # By default the initial separation is the grid step
    if args.initial_separation is None:
        args.initial_separation = args.resolution

    # Build the map properties
    map_properties = MapProperties(nx, ny, args.x_min, args.y_min, args.resolution)

    # Initializes the FLE to process
    fle = FiniteLyapunovExponentsIntegration(
        start_time,
        end_time,
        delta,
        MODE[args.mode],
        args.final_separation,
        args.initial_separation,
        ts,
    )

    if HAVE_DASK and (args.scheduler_file is not None or args.local_cluster):
        if args.local_cluster:
            client = dask.distributed.Client(dask.distributed.LocalCluster())
        else:
            client = dask.distributed.Client(scheduler_file=args.scheduler_file)
        lagrangian.debug(str(client))

        while True:
            workers = len(client.scheduler_info()["workers"])
            threads_per_worker = min(
                item["nthreads"] for item in client.scheduler_info()["workers"].values()
            )
            if workers == 0:
                time.sleep(5)
            else:
                break

        array = build_dask_array(args, fle, map_properties, workers, threads_per_worker)
        exponents = array.compute()
    else:
        exponents = worker_task(args, fle, map_properties, args.threads)

    write_netcdf(args, exponents, map_properties, nx, ny, start_time)


if __name__ == "__main__":
    main()
