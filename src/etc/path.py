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
# lagrangian.  If not, see <http://www.gnu.org/licenses/>.import lagrangian
import datetime
import argparse
import os
import re
import sys
import dateutil.parser
import lagrangian


class FileType(object):
    """
    FileType factory
    """
    def __init__(self, mode='r'):
        self.__mode = mode

    def __call__(self, path):
        try:
            return open(path, self.__mode)
        except (OSError, IOError) as e:
            raise argparse.ArgumentTypeError("can't open '%s': %s" % (path, e))


def path_type(value):
    """
    The option must be define a path to file
    """
    path = os.path.abspath(value)
    if not os.path.isfile(path):
        raise argparse.ArgumentTypeError('%s is not a file' % value)
    return path


def date_type(value):
    """
    The option must be define a date
    """
    try:
        value = dateutil.parser.parse(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError(
            "invalid date time %r: %s" % (value, error))
    return value


def usage():
    """
    Parse commands line
    """
    parser = argparse.ArgumentParser(description='Compute path')

    parser.add_argument('--version',
                        help='print the version number and exit',
                        action='store_true')
    parser.add_argument('--output',
                        help='Output positions',
                        type=FileType('w'),
                        default=sys.stdout)

    # Checks whether the user wishes to display the version number
    args, _ = parser.parse_known_args()
    if args.version:
        print(lagrangian.version())
        sys.exit(0)

    parser.add_argument('configuration',
                        help='Configuration file',
                        type=path_type)
    parser.add_argument('input',
                        help='Input positions',
                        type=FileType('r'))
    parser.add_argument('start_time',
                        help='Start time',
                        type=date_type)
    parser.add_argument('end_time',
                        help='End time',
                        type=date_type)
    return parser.parse_args()


class Position:
    """
    Handle a buoy
    """
    IDENT = 0

    def __init__(self, x, y):
        self.__ident = Position.IDENT
        self.__process = True
        self.__x = x
        self.__y = y

        Position.IDENT += 1

    def update(self, process, x, y):
        """
        Update the position
        """
        self.__process = process
        self.__x = x
        self.__y = y

    @property
    def x(self):
        """
        Longitude in degrees
        """
        return self.__x

    @property
    def y(self):
        """
        Latitude in degrees
        """
        return self.__y

    @property
    def process(self):
        """
        Test whether the buoy drift is completed
        """
        return self.__process

    def __str__(self):
        return "%d\t%f\t%f" % (self.__ident, self.__x, self.__y)


def decode_x(value):
    """
    Parse a longtiude from a string
    """
    try:
        value = float(value)
    except ValueError:
        raise RuntimeError("%r could not be interpreted as a float" % value)
    while value > 180:
        value -= 180
    if value < -180 or value > 180:
        raise RuntimeError("%f does not represent a correct longitude"
                           % value)
    return value


def decode_y(value):
    """
    Parse a latitude from a string
    """
    try:
        value = float(value)
    except ValueError:
        raise RuntimeError("%r could not be interpreted as a float" % value)
    if value < -90 or value > 90:
        raise RuntimeError("%f does not represent a correct latitude"
                           % value)
    return value


def main():
    """
    Main program
    """
    args = usage()

    ts = lagrangian.TimeSerie(args.configuration)
    delta = datetime.timedelta(0, 6 * 60 * 60)

    if args.start_time < ts.start_time():
        raise RuntimeError("The start date (%s) is before the beginning "
                           "of the time series (%s)" %
                           (args.start_time.ToString("%Y-%m-%dT%H:%M:%S"),
                            ts.start_time().ToString("%Y-%m-%dT%H:%M:%S")))

    if args.end_time > ts.end_time():
        raise RuntimeError("The end date (%s) is after the ending "
                           "of the time series (%s)" %
                           (args.end_time.ToString("%Y-%m-%dT%H:%M:%S"),
                            ts.end_time().ToString("%Y-%m-%dT%H:%M:%S")))

    path = lagrangian.Path(args.start_time, args.end_time, delta, ts)
    positions = []

    # Loading input file
    ix = 1
    for line in args.input:
        try:
            line = line[:line.find('#')].strip()
            if line:
                columns = re.split(r'\s+', line)
                if len(columns) < 2:
                    raise RuntimeError("missing position")
                positions.append(Position(decode_x(columns[0]),
                                          decode_y(columns[1])))
        except Exception as err:
            raise RuntimeError("Parse error in input file at "
                               "line %d: %s" % (ix, ''.join(err.args)))
        ix += 1

    it = path.get_iterator()

    for time_step in it:
        path.fetch(time_step)
        for item in positions:
            if item.process:
                result = path.compute(it, item.x, item.y)
                if result is not None:
                    args.output.write(
                        "%s\t%s\n" % (
                            item,
                            datetime.datetime.fromtimestamp(
                                time_step).isoformat()))
                    item.update(True, result[0], result[1])
                else:
                    item.update(False, item.x, item.y)


if __name__ == '__main__':
    main()
