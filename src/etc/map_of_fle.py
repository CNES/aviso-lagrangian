#!/usr/bin/env python

# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of GNU Lesser General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See GNU Lesser General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License along with
# lagrangian.  If not, see <http://www.gnu.org/licenses/>.
import datetime
import argparse
import signal
import multiprocessing
import sys
import dateutil.parser
import netCDF4
import numpy
import lagrangian


SYSTEM_UNITS = {
    "metric": lagrangian.kMetric,
    "angular": lagrangian.kAngular
}


STENCIL = {
    "triplet": lagrangian.kTriplet,
    "quintuplet": lagrangian.kQuintuplet
}


MODE = {
    "fsle": lagrangian.kFSLE,
    "ftle": lagrangian.kFTLE
}


# List of ignored signals
UNINTERRUPTIBLE = ['SIG_BLOCK', 'SIG_DFL', 'SIGSTOP', 'SIGKILL', 'SIGCHLD',
                   'SIGCLD', 'SIGTERM', 'SIG_IGN', 'SIG_SETMASK',
                   'SIG_UNBLOCK', 'SIGWINCH']


# List of intercepted signals
SIGNAL = dict((value, name) for name, value in signal.__dict__.items()
              if name.startswith('SIG') and name not in UNINTERRUPTIBLE)


def timedelta_type(value):
    """
    The option should define a time duration
    """
    try:
        value = float(value)
    except ValueError:
        raise argparse.ArgumentTypeError("invalid float value: %r" % value)
    return datetime.timedelta(days=value)


def datetime_type(value):
    """
    The option should define a datetime
    """
    try:
        value = dateutil.parser.parse(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError(
            "invalid date time %r: %s" % (value, error))
    return value


class TimeDirection(object):
    """
    Define the time integration direction
    """
    BACKWARD = 0
    FORWARD = 1

    @staticmethod
    def default():
        """
        Return the default time integration direction
        """
        return "backward"

    @classmethod
    def choices(cls):
        """
        Return a hash that represents the possible choices for the
        integration time.
        """
        return dict(backward=cls.BACKWARD, forward=cls.FORWARD)


def positive_value(value):
    """
    Check options for defining values strictly positive.
    """
    try:
        value = float(value)
    except ValueError:
        msg = "invalid float value: %r" % value
        raise argparse.ArgumentTypeError(msg)
    if value < 0:
        msg = "value must be positive: %f" % value
        raise argparse.ArgumentTypeError(msg)
    return value


def usage():
    """
    Program syntax
    """
    parser = argparse.ArgumentParser(description='Compute Map of FLE',
                                     add_help=False)
    grid = parser.add_argument_group('grid arguments',
                                     'Set the grid properties to create')
    grid.add_argument('--resolution',
                      help='output grid resolution in degrees',
                      type=positive_value,
                      metavar='STEP',
                      default=1)
    grid.add_argument('--x_min',
                      help='x min in degrees',
                      type=float,
                      default=-180)
    grid.add_argument('--x_max',
                      help='x max in degrees',
                      type=float,
                      default=180)
    grid.add_argument('--y_min',
                      help='y min in degrees',
                      type=float,
                      default=-90)
    grid.add_argument('--y_max',
                      help='y max in degrees',
                      type=float,
                      default=90)

    integration = parser.add_argument_group('integration arguments',
                                            'Define integration')
    integration.add_argument('--mode',
                             help="define the integration mode: "
                                  "fsle to compute Finite Size "
                                  "Lyapunov Exponent, ftle to compute "
                                  "Finite Time Lyapunov Exponent.",
                             choices=MODE.keys(),
                             default="fsle")
    integration.add_argument('--time_direction',
                             help='time integration direction',
                             choices=TimeDirection.choices().keys(),
                             default=TimeDirection.default())
    integration.add_argument('--stencil',
                             help='type of stencil used to compute the finite '
                                  'difference.',
                             choices=STENCIL.keys(),
                             default="triplet")
    integration.add_argument('--initial_separation',
                             help='initial separation in degrees of '
                                  'neighbouring particules',
                             type=positive_value,
                             metavar='DELTA',
                             default=None)
    integration.add_argument('--advection_time',
                             help='time of advection in number of days '
                                  'elapsed since the beginning of the '
                                  'simulation. This option set the maximum '
                                  'time of advection in FSLE mode',
                             default=datetime.timedelta(days=200),
                             metavar='DURATION',
                             type=timedelta_type)
    integration.add_argument('--final_separation',
                             help='maximum final separation in degrees',
                             type=positive_value,
                             metavar='DELTA',
                             default=-1)
    integration.add_argument('--integration_time_step',
                             help='particles time integration step in hours',
                             type=positive_value,
                             metavar='DURATION',
                             default=6)
    integration.add_argument('--diagnostic',
                             help='write additional variables containing  '
                                  'diagnostics, depending on calculation mode.'
                                  ' These variables are the '
                                  'effective final separation distance and '
                                  'advection time, in FSLE mode, or the '
                                  'effective final separation distance, in '
                                  'FTLE mode.',
                             action='store_true')

    data = parser.add_argument_group('reader arguments',
                                     'Set options of the NetCDF reader.')
    data.add_argument('--unit',
                      help='system of units for velocity',
                      choices=SYSTEM_UNITS.keys(),
                      default="metric")
    data.add_argument('--mask',
                      help='netCDF grid for fixing undefined cells',
                      nargs=2,
                      metavar=('PATH', 'VARNAME'))

    parser.add_argument('--verbose',
                        help='Verbose mode',
                        action='store_true')
    parser.add_argument('--version',
                        help='print the version number and exit',
                        action='store_true')

    args, _ = parser.parse_known_args()
    if args.version:
        print(lagrangian.version())
        sys.exit(0)

    parser.add_argument('--help', '-h',
                        action='help',
                        default=argparse.SUPPRESS,
                        help='show this help message and exit')
    parser.add_argument('configuration',
                        help='configuration file')
    parser.add_argument('output',
                        help='map of FLE in NetCDF format')
    parser.add_argument('start_time',
                        help='start of the integration',
                        type=datetime_type)

    args = parser.parse_args()
    if MODE[args.mode] == lagrangian.kFTLE:
        if args.final_separation != -1:
            parser.error('argument --final_separation not allowed in FTLE '
                         'mode')
    return args


def convert_from_sec_to_day_inv(array, fill_value):
    """
    Converting values from second^{-1} to days^{-1}
    """
    result = numpy.ma.masked_equal(array, fill_value)
    result *= 86400
    return result.data


def convert_from_sec_to_day(array, fill_value):
    """
    Converting values from second to days
    """
    result = numpy.ma.masked_equal(array, fill_value)
    result /= 86400
    return result.data


def check_period(ts, start_time, end_time):
    """
    Check the consistency of the defined period with the time series
    """
    if start_time > end_time:
        start_time, end_time = end_time, start_time

    if start_time < ts.start_time():
        raise RuntimeError("The start date (%s) is before the beginning "
                           "of the time series (%s)" %
                           (start_time.strftime("%Y-%m-%dT%H:%M:%S"),
                            ts.start_time().strftime("%Y-%m-%dT%H:%M:%S")))

    if end_time > ts.end_time():
        raise RuntimeError("The end date (%s) is after the ending "
                           "of the time series (%s)" %
                           (end_time.strftime("%Y-%m-%dT%H:%M:%S"),
                            ts.end_time().strftime("%Y-%m-%dT%H:%M:%S")))


def calculation():
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

    # Fill value for double in NetCDF file
    NC_FILL_DOUBLE = netCDF4.default_fillvals['f8']

    # Initializes the time series to process
    ts = lagrangian.TimeSerie(args.configuration,
                              SYSTEM_UNITS[args.unit])
    delta = datetime.timedelta(0, args.integration_time_step * 60 * 60)

    start_time = lagrangian.DateTime(args.start_time)()

    # Calculate the end of the integration depending on the advection time
    # direction
    if TimeDirection.choices()[args.time_direction] == TimeDirection.BACKWARD:
        end_time = lagrangian.DateTime(start_time - args.advection_time)()
    else:
        end_time = lagrangian.DateTime(start_time + args.advection_time)()

    check_period(ts, start_time, end_time)

    nx = int((args.x_max - args.x_min) / args.resolution) + 1
    ny = int((args.y_max - args.y_min) / args.resolution) + 1

    # By default the initial separation is the grid step
    if args.initial_separation is None:
        args.initial_separation = args.resolution

    # Build the map properties
    map_properties = lagrangian.MapProperties(
        nx,
        ny,
        args.x_min,
        args.y_min,
        args.resolution)

    # Initializes the FLE to process
    fle = lagrangian.FiniteLyapunovExponentsIntegration(
        start_time,
        end_time,
        delta,
        MODE[args.mode],
        args.final_separation,
        args.initial_separation,
        ts)

    # The nodes of the grid result, located on land are undefined. To speed
    # up the calculation we use a external grid to remove these cells from
    # the calculation.
    if args.mask:
        reader = lagrangian.Netcdf()
        reader.open(args.mask[0])
        reader.load(args.mask[1])
    else:
        reader = None

    # Initializes the map to process
    map_of_fle = lagrangian.MapOfFiniteLyapunovExponents(
        map_properties,
        fle,
        STENCIL[args.stencil],
        reader)

    # Computes map
    map_of_fle.compute()

    # Creates the NetCDF file
    rootgrp = netCDF4.Dataset(args.output, 'w', format='NETCDF4')
    rootgrp.createDimension('lon', nx)
    rootgrp.createDimension('lat', ny)
    rootgrp.title = 'Map of %s' % args.mode.upper()

    # Write parameter values used for the integration
    for item in args._get_kwargs():
        name, value = item
        setattr(rootgrp, name, str(value))

    x_axis = rootgrp.createVariable('lon', 'f8', ('lon',))
    x_axis.standard_name = "longitude"
    x_axis.units = 'degrees_east'
    x_axis.axis = "X"
    x_axis[:] = map_properties.get_x_axis()

    y_axis = rootgrp.createVariable('lat', 'f8', ('lat',))
    y_axis.standard_name = "latitude"
    y_axis.units = 'degrees_north'
    y_axis.axis = "Y"
    y_axis[:] = map_properties.get_y_axis()

    theta1 = rootgrp.createVariable('theta1',
                                    'f8',
                                    ('lon', 'lat',),
                                    fill_value=NC_FILL_DOUBLE)
    theta1.long_name = 'Orientation of the eigenvectors associated to the' \
        'maximum eigenvalues of Cauchy-Green strain tensor'
    theta1.units = 'degree'
    theta1[:] = map_of_fle.get_map_of_theta1(NC_FILL_DOUBLE)

    theta2 = rootgrp.createVariable('theta2',
                                    'f8',
                                    ('lon', 'lat',),
                                    fill_value=NC_FILL_DOUBLE)
    theta2.long_name = 'Orientation of the eigenvectors associated to the' \
        'minimum eigenvalues of Cauchy-Green strain tensor'
    theta2.units = 'degree'
    theta2[:] = map_of_fle.get_map_of_theta2(NC_FILL_DOUBLE)

    lambda1 = rootgrp.createVariable('lambda1',
                                     'f8',
                                     ('lon', 'lat',),
                                     fill_value=NC_FILL_DOUBLE)
    lambda1.long_name = 'FLE associated to the maximum eigenvalues of ' \
        'Cauchy-Green strain tensor'
    lambda1.units = '1/day'
    lambda1[:] = convert_from_sec_to_day_inv(
        map_of_fle.get_map_of_lambda1(NC_FILL_DOUBLE), NC_FILL_DOUBLE)

    lambda2 = rootgrp.createVariable('lambda2',
                                     'f8',
                                     ('lon', 'lat',),
                                     fill_value=NC_FILL_DOUBLE)
    lambda2.long_name = 'FLE associated to the minimum eigenvalues of ' \
        'Cauchy-Green strain tensor'
    lambda2.units = '1/day'
    lambda2[:] = convert_from_sec_to_day_inv(
        map_of_fle.get_map_of_lambda2(NC_FILL_DOUBLE), NC_FILL_DOUBLE)

    if args.diagnostic:
        diag_sep = rootgrp.createVariable(
            'separation_distance',
            'f8',
            ('lon', 'lat',),
            fill_value=NC_FILL_DOUBLE)
        diag_sep.long_name = 'effective final separation distance'
        diag_sep.units = 'degree'
        diag_sep[:] = map_of_fle.get_map_of_final_separation(
            NC_FILL_DOUBLE)

        if MODE[args.mode] == lagrangian.kFSLE:
            diag_advtime = rootgrp.createVariable(
                'advection_time',
                'f8',
                ('lon', 'lat',),
                fill_value=NC_FILL_DOUBLE)
            diag_advtime.long_name = 'actual advection time'
            diag_advtime.units = 'number of days elapsed ' \
                'since %s' % start_time.isoformat()
            diag_advtime[:] = convert_from_sec_to_day(
                map_of_fle.get_map_of_delta_t(NC_FILL_DOUBLE),
                NC_FILL_DOUBLE)

    rootgrp.close()


def signal_handler(signum, _):
    """
    Signal handler
    """
    print('Stop with signal %s' % SIGNAL[signum])
    for process in multiprocessing.process.active_children():
        process.terminate()
    sys.exit(255)


def main():
    """
    Main function
    """
    # Initialize signal handler
    for value in SIGNAL:
        signal.signal(value, signal_handler)

    # As the calculation is long, the calculation process is started in
    # parallel. Thus the user can stop the program
    process = multiprocessing.Process(target=calculation)
    process.start()
    process.join()


if __name__ == '__main__':
    main()
