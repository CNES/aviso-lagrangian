#!/usr/bin/env python

# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# lagrangian.  If not, see <http://www.gnu.org/licenses/>.
import matplotlib
matplotlib.use('Agg')

import argparse
import logging
import mpl_toolkits.basemap
import netCDF4
import matplotlib.pyplot
import numpy
import sys


class AxisUnit(list):
    """
    Units management for axes
    """

    def __init__(self):
        """
        Default constructor
        """
        super(AxisUnit, self).__init__()

    def append(self, item):
        """
        Add a unit at the end of the list of known units
        """
        super(AxisUnit, self).append(item)

    @property
    def units(self):
        """
        Returns the list of known units
        """
        return self


class AxisLatitudeUnit(AxisUnit):
    """
    Units known to the axis defining the latitude
    """

    def __init__(self):
        super(AxisLatitudeUnit, self).__init__()
        self.append("degrees_north")
        self.append("degree_north")
        self.append("degree_N")
        self.append("degrees_N")
        self.append("degreeN")
        self.append("degreesN")


class AxisLongitudeUnit(AxisUnit):
    """
    Units known to the axis defining the longitude
    """

    def __init__(self):
        super(AxisLongitudeUnit, self).__init__()
        self.append("degrees_east")
        self.append("degree_east")
        self.append("degree_E")
        self.append("degrees_E")
        self.append("degreeE")
        self.append("degreesE")


class Axis(object):
    """
    Coordinate axis
    """
    UNKNOWN = -1
    LATITUDE = 0
    LONGITUDE = 1

    def __init__(self, axis_type, points):
        """
        Default constructor
        """
        if axis_type == Axis.LATITUDE:
            self._unit = AxisLatitudeUnit()[0]
            self._type = Axis.LATITUDE
        elif axis_type == Axis.LONGITUDE:
            self._unit = AxisLongitudeUnit()[0]
            self._type = Axis.LONGITUDE
        else:
            self._type = Axis.UNKNOWN
            self._unit = None
        self._points = points

    @classmethod
    def from_variable(cls, name, variable):
        """
        Construct a coordinate axis from an existing NetCDF variable
        """
        if variable.ndim != 1 or variable.dimensions[0] != name:
            raise Exception('%s is not an axis' % name)

        # Determine axis from units
        if variable.units in AxisLatitudeUnit():
            axis_type = Axis.LATITUDE
        elif variable.units in AxisLongitudeUnit():
            axis_type = Axis.LONGITUDE
        else:
            axis_type = Axis.UNKNOWN

        # Store axis value
        points = variable[:] if axis_type != Axis.UNKNOWN else None
        return cls(axis_type, points)

    def __getitem__(self, item):
        """
        Get the ith coordinate value.
        """
        return self._points[item]

    @property
    def type(self):
        """
        Get type of axis
        """
        return self._type

    def normalize(self):
        """
        Standardization of longitude beetween [-180,180]
        """
        if self._type != Axis.LONGITUDE:
            raise Exception('Invalid argument')
        self._points = numpy.where(self._points >= 180,
                                   self._points - 360,
                                   self._points)
        self._points = numpy.where(self._points < -180,
                                   self._points + 360,
                                   self._points)

    @staticmethod
    def normalize_longitude_range(x_min, x_max):
        """
        Standardization of longitude range beetween [-180,180]
        """
        axis = Axis(Axis.LONGITUDE, numpy.array([x_min, x_max]))
        axis.normalize()

        if x_max - x_min > axis._points[1] - axis._points[0]:
            axis._points[0] -= 180
            axis._points[1] += 180

        return [axis._points[0], axis._points[1]]


class Variable(object):
    """
    Variable
    """
    @classmethod
    def ncattr(cls, attr, group):
        """
        Returns the value for the NetCDF attribute attr
        """
        return group.getncattr(attr) if attr in group.ncattrs() else ''

    def __init__(self,
                 x=None,
                 y=None,
                 z=None,
                 title='',
                 long_name='',
                 unit=''):
        """
        Default constructor
        """
        self._x = x
        self._y = y
        self._z = z
        self._unit = unit
        self._title = title
        self._long_name = long_name

    @classmethod
    def from_netcdf(cls, filename, variable_name, x_axis=None, y_axis=None):
        """
        Constructs a new instance from an existing NetCDF variable
        """
        logging.debug('reading %s' % filename)
        dataset = netCDF4.Dataset(filename)
        variable = dataset.variables[variable_name]
        if not variable.size:
            return None

        # The variable must be a vector or matrix otherwise it is a valid
        # variable
        ndims = len(variable.shape)
        if ndims < 1 or ndims > 2:
            raise Exception('%s must be a vector or a matrix' % variable_name)
        z = variable[:]

        # Reading X axis from an existing variable
        if x_axis:
            x_axis = Axis(Axis.LONGITUDE, dataset.variables[x_axis][:])

        # Reading Y axis from an existing variable
        if y_axis:
            y_axis = Axis(Axis.LATITUDE, dataset.variables[y_axis][:])

        # Looking for longitude & latitude axis in the NetCDF file
        for name in dataset.variables:
            try:
                axis = Axis.from_variable(name, dataset.variables[name])
                if axis.type == Axis.LONGITUDE and x_axis is None:
                    logging.debug('reading x axis from %s' % name)
                    x_axis = axis
                elif axis.type == Axis.LATITUDE and y_axis is None:
                    logging.debug('reading y axis from %s' % name)
                    y_axis = axis
            except:
                pass

        # If we did not find the definition of the axes we throw an exception
        if (x_axis is None) or (y_axis is None):
            raise RuntimeError('Unable to Find the description of latitude '
                               'and longitude axes.')

        # Longitude must be in [-180,180]
        x_axis.normalize()

        # Store axis values
        x = x_axis[:]
        y = y_axis[:]
        if ndims == 2:

            # We must transpose the matrix if values are z[nlat, nlon]
            if variable.shape[0] == len(x):
                logging.debug('transpose %s' % variable_name)
                z = z.transpose(1, 0)

            # Longitude must be increasing
            indices = numpy.argsort(x)
            x = x[indices]
            z = z[:, indices]

        # Build the new object
        return cls(x,
                   y,
                   z,
                   Variable.ncattr('title', dataset),
                   Variable.ncattr('long_name', variable),
                   Variable.ncattr('units', variable))

    def select_zone(self, x_min, y_min, x_max, y_max):
        """
        Selecting values in a geographical area
        """
        [x_min, x_max] = Axis.normalize_longitude_range(x_min, x_max)

        logging.debug('select area [%.1f, %.1f, %.1f, %.1f]' % (x_min,
                                                                x_max,
                                                                y_min,
                                                                y_max))

        if x_min > x_max or y_min > y_max:
            raise RuntimeError('The area definition is invalid')

        ndims = len(self._z.shape)

        index = numpy.where((self._x >= x_min) & (self._x <= x_max))
        self._x = self._x[index]
        if ndims == 2:
            self._z = self._z[:, index[0]]
        else:
            self._y = self._y[index]
            self._z = self._z[index]

        index = numpy.where((self._y >= y_min) & (self._y <= y_max))
        self._y = self._y[index]
        if ndims == 2:
            self._z = self._z[index[0], :]
        else:
            self._x = self._x[index]
            self._z = self._z[index]

        if len(self._x) == 0 or len(self._y) == 0:
            raise RuntimeError('No data selected in the defined area')

    @property
    def title(self):
        """
        Returns the variable title
        """
        return self._title

    @title.setter
    def title(self, value):
        """
        Sets the variable title
        """
        self._title = value

    @property
    def long_name(self):
        """
        Returns the variable description
        """
        if len(self._unit):
            return '%s (%s)' % (self._long_name, self._unit)
        else:
            return self._long_name

    @long_name.setter
    def long_name(self, value):
        """
        Sets the variable description
        """
        self._long_name = value

    @property
    def latitudes(self):
        """
        Returns the latitudes of the variable
        """
        return self._y

    @property
    def longitudes(self):
        """
        Returns the longitude of the variable
        """
        return self._x

    @property
    def values(self):
        """
        Returns the values of the variable
        """
        return self._z

    def dump(self):
        """
        Displays information about the variable loaded in memory
        """
        if len(self._x):
            logging.debug('x_min=%f, x_max=%f' % (numpy.min(self._x),
                                                  numpy.max(self._x)))
        if len(self._y):
            logging.debug('y_min=%f, y_max=%f' % (numpy.min(self._y),
                                                  numpy.max(self._y)))
        if len(self._z):
            logging.debug('z_mean=%f, z_std=%f, z_min=%f, z_max=%f' %
                          (numpy.mean(self._z),
                           numpy.std(self._z),
                           numpy.min(self._z),
                           numpy.max(self._z)))


class OptionType(object):
    """
    Manage a list of options known for the program.
    """
    def __init__(self, default, *args):
        if len(args) == 0:
            raise TypeError("Required argument 'args' not found")
        if isinstance(args[0], dict):
            self._data = args[0]
        else:
            self._data = {}
            self._data.update(zip(args, range(len(args))))
        self._default = default

    def __getattr__(self, key):
        return self._data[key]

    def __getitem__(self, key):
        return self._data[key]

    def __str__(self):
        ids = sorted(self._data.keys())
        if len(ids) > 1:
            return ', '.join(ids[:-1]) + ' or ' + ids[-1]
        else:
            return ' '.join(ids)

    def __iter__(self):
        for key in sorted(self._data):
            yield key

    def __len__(self):
        return len(self._data.keys())

    @property
    def default(self):
        return self._default


class ContinentType(OptionType):
    """
    Manage list of continents known for the program
    """

    def __init__(self):
        super(ContinentType, self).__init__(
            'filled',
            'filled',
            'wireframe',
            'none')


class ProjectionType(OptionType):
    """
    Manage list of projection known for the program
    """

    def __init__(self):
        super(ProjectionType, self).__init__(
            'equidistant', {
                'equidistant': 'cyl',
                'gall': 'gall',
                'mercator': 'merc',
                'miller': 'mill'
            })


class ResolutionType(OptionType):
    """
    Manage list of resolution known for the program
    """

    def __init__(self):
        super(ResolutionType, self).__init__(
            'low', {
                'crude': 'c',
                'low': 'l',
                'intermediate': 'i',
                'high': 'h',
                'full': 'f'
            })


class ColormapType(OptionType):
    """
    Manage list of colormaps known for matplotlib
    """
    def __init__(self):
        super(ColormapType, self).__init__(
            'jet', {
                m.lower(): m
                for m in matplotlib.cm.datad
                if not m.endswith("_r")
            })


class Mapping(object):
    """
    Plotting 2D data on maps
    """
    X_MIN = 'llcrnrlon'
    X_MAX = 'urcrnrlon'
    Y_MIN = 'llcrnrlat'
    Y_MAX = 'urcrnrlat'

    def __init__(self, colormap, projection, resolution):
        """
        Default constructor
        """
        self._basemap = None
        self._cm = colormap
        self._continent = ContinentType().filled
        self._figure = matplotlib.pyplot.figure()
        self._legend = ''
        self._meridian_linewidth = 0.5
        self._projection = projection
        self._range = None
        self._resolution = resolution
        self._show_colormap = True
        self._size = 4
        self._x = None
        self._y = None
        self._z = None
        self._zone = {}
        self._zorder = 0

    def invert_colormap(self):
        """
        Invert colormap
        """
        logging.debug('invert colormap')
        if not self._cm.endswith("_r"):
            self._cm += '_r'

    def __contour(self, cm):
        """
        Displays grid
        """
        logging.debug('interpolate values to map projection')
        (z, x, y) = self._basemap.transform_scalar(self._z,
                                                   self._x,
                                                   self._y,
                                                   len(self._x),
                                                   len(self._y),
                                                   returnxy=True)
        args = (x, y, z)

        if self._range is not None:
            args += (self._range,)

        return self._basemap.contourf(*args,
                                      extend='both',
                                      cmap=cm,
                                      zorder=self._zorder,
                                      animated=True)

    def __display_data(self):
        """
        Plot data
        """
        options = self._zone
        options['resolution'] = getattr(ResolutionType(), self._resolution)
        options['projection'] = getattr(ProjectionType(), self._projection)

        logging.debug('set up basemap')
        self._basemap = mpl_toolkits.basemap.Basemap(**options)

        logging.debug('loading colormap: ' + self._cm)
        cm = matplotlib.cm.get_cmap(self._cm)

        # Draws an image or a scatter plot
        if len(self._z.shape) == 2:
            ctr = self.__contour(cm)
        else:
            ctr = self.__scatter(cm)

        def number_of_decimal(x):
            x = str(x)
            if '.' in x:
                r = len(str(x).split(".")[-1])
                if r > 6:
                    r = 6
                return r
            return 0

        # Draws colormap if user want it
        if self._show_colormap:
            logging.debug('draw colormap')
            fmt = '%%.%df' % max(number_of_decimal(self._range[0]),
                                 number_of_decimal(self._range[-1]))
            cbar = self._figure.colorbar(ctr,
                                         orientation='horizontal',
                                         pad=0.06,
                                         aspect=40,
                                         format=fmt,
                                         ticks=numpy.linspace(self._range[0],
                                                              self._range[-1],
                                                              11))
            for text in cbar.ax.get_xticklabels():
                text.set_size(6)
            cbar.set_label(self._legend, fontsize=8)

    def __draw_latitude(self):
        """
        Draw latitudes lines
        """
        logging.debug('draw and label parallels')
        self._basemap.drawparallels(self.__range(self._zone[self.Y_MIN],
                                                 self._zone[self.Y_MAX]),
                                    color='grey',
                                    fmt='%0.1f',
                                    fontsize=6,
                                    labels=[1, 0, 0, 0],
                                    labelstyle='+/-',
                                    linewidth=self._meridian_linewidth,
                                    zorder=self._zorder)

    def __draw_longitude(self):
        """
        Draw longitudes lines
        """
        logging.debug('draw and label meridians')
        self._basemap.drawmeridians(self.__range(self._zone[self.X_MIN],
                                                 self._zone[self.X_MAX]),
                                    color='grey',
                                    fmt='%0.1f',
                                    fontsize=6,
                                    labels=[0, 0, 0, 1],
                                    labelstyle='+/-',
                                    linewidth=self._meridian_linewidth,
                                    zorder=self._zorder)

    @staticmethod
    def __range(min_range, max_range):
        """Create range to draw longitudes or latitudes lines
        """
        max_range = round(max_range)
        min_range = round(min_range)
        step = round((max_range-min_range) * 0.1)
        if step == 0:
            step = 1
        return numpy.arange(min_range,
                            max_range,
                            step)

    def __scatter(self, cm):
        """
        Displays scattered data
        """
        logging.debug('plot points on the map')

        # Converts lat/lon from degrees to basemap projection coordinates
        (x, y) = self._basemap(self._x, self._y)

        # Display a cross for small symbols otherwise a circle
        marker = 'x' if self._size < 0.2 else 'o'

        return self._basemap.scatter(x,
                                     y,
                                     s=self._size,
                                     c=self._z,
                                     marker=marker,
                                     linewidths=0,
                                     cmap=cm)

    @property
    def continent(self):
        """
        Returns the display type of continents
        """
        return self._continent

    @continent.setter
    def continent(self, value):
        """
        Set up the display type of continents
        """
        self._continent = getattr(ContinentType(), value)

    @property
    def legend(self):
        """
        Returns the legend of the figure
        """
        return self._legend

    @legend.setter
    def legend(self, value):
        """
        Sets the legend of the figure
        """
        self._legend = value

    @property
    def meridian_linewidth(self):
        """
        Returns line width for meridians
        """
        return self._meridian_linewidth

    @meridian_linewidth.setter
    def meridian_linewidth(self, value):
        """
        Set up line width for meridians
        """
        self._meridian_linewidth = value

    def set_range(self, min_range, max_range, n_colors):
        """
        Set color range
        """
        step = (max_range - min_range) / n_colors
        if step == 0:
            step = 1
        logging.debug('set range to [%.4f, %.4f], step=%.4f' % (min_range,
                                                                max_range,
                                                                step))
        self._range = numpy.arange(min_range, max_range + step, step)

    def set_zone(self, xmin, ymin, xmax, ymax):
        """
        Selects area to display
        """
        [xmin, xmax] = Axis.normalize_longitude_range(xmin, xmax)

        self._zone[self.X_MIN] = xmin
        self._zone[self.X_MAX] = xmax
        self._zone[self.Y_MIN] = ymin
        self._zone[self.Y_MAX] = ymax

    @property
    def show_colormap(self):
        """
        Returns true if user wants to draws colormap on the figure otherwise
        false
        """
        return self._show_colormap

    @show_colormap.setter
    def show_colormap(self, value):
        """
        Shows or not the colormap
        """
        self._show_colormap = value

    def show_map_above_continents(self):
        """
        Displays data over the continents
        """
        self._zorder = 10

    @property
    def size(self):
        """
        Returns the point size used in the scatter plot
        """
        return self._legend

    @size.setter
    def size(self, value):
        """
        Sets the point size used in the scatter plot
        """
        self._size = value

    @property
    def x(self):
        """
        Returns the longitude of the variable to map
        """
        return self._x

    @x.setter
    def x(self, value):
        """
        Sets the longitude of the variable to map
        """
        self._x = value
        if self.X_MIN not in self._zone:
            self._zone[self.X_MIN] = self._x.min()
            self._zone[self.X_MAX] = self._x.max()

    @property
    def y(self):
        """
        Returns the latitude of the variable to map
        """
        return self._y

    @y.setter
    def y(self, value):
        """
        Sets the latitude of the variable to map
        """
        self._y = value
        if self.Y_MIN not in self._zone:
            self._zone[self.Y_MIN] = self._y.min()
            self._zone[self.Y_MAX] = self._y.max()

    @property
    def z(self):
        """
        Returns the values of the variable to map
        """
        return self._z

    @z.setter
    def z(self, value):
        """
        Sets the values of the variable to map
        """
        self._z = value
        if self._range is None and len(self._z) != 0:
            self.set_range(numpy.min(self._z), numpy.max(self._z), 250)

    def display(self, output, title, dpi):
        """
        Mapping data
        """
        self.__display_data()
        if self._continent != ContinentType().none:
            logging.debug('draw coastlines')
            self._basemap.drawcoastlines(linewidth=0.25)
            if self._continent != ContinentType().wireframe:
                logging.debug('fill continents')
                self._basemap.fillcontinents(color='sandybrown', zorder=0)
        if self._meridian_linewidth:
            self.__draw_latitude()
            self.__draw_longitude()

        # Update title
        ax = self._figure.gca()
        ax.set_title(title, fontsize=10)

        # Create image
        logging.debug("write `%s'" % output)
        self._figure.savefig(output, dpi=dpi, bbox_inches='tight',
                             pad_inches=0.4)


class DrawColormaps(argparse.Action):
    """
    Draw colormaps in an image file
    """
    def __call__(self, parser, namespace, values, option_string=None):

        # Creating an array to view colormaps
        array = numpy.linspace(0, 1, 256).reshape(1, -1)
        array = numpy.vstack((array, array))

        # Type of colormaps known
        maps = ColormapType()
        nmaps = len(maps)

        # Set up figure properties
        fig = matplotlib.pyplot.figure(figsize=(5, 10))
        fig.subplots_adjust(top=0.99, bottom=0.01, left=0.2, right=0.99)

        # list of the colormaps
        for i, m in enumerate(maps):

            # Select sub plot to fill
            ax = matplotlib.pyplot.subplot(nmaps, 1, i + 1)

            # Draw colormap and legend
            matplotlib.pyplot.axis("off")
            matplotlib.pyplot.imshow(array,
                                     aspect='auto',
                                     cmap=matplotlib.pyplot.get_cmap(maps[m]),
                                     origin='lower')
            pos = list(ax.get_position().bounds)
            fig.text(pos[0] - 0.01, pos[1], m, fontsize=10,
                     horizontalalignment='right')

        # Write image and terminate the program
        matplotlib.pyplot.savefig(values, dpi=200, facecolor='gray')
        sys.exit(0)


def type_resolution(value):
    """
    Checks resolution type
    """
    if value.lower() not in ResolutionType():
        raise argparse.ArgumentTypeError('%s is not a type of resolution known'
                                         % value)
    return value


def type_colormap(value):
    """
    Checks colormap name
    """
    colormap_type = ColormapType()
    value = value.lower()
    if value not in colormap_type:
        raise argparse.ArgumentTypeError('%s is not a name of colormap known'
                                         % value)
    return colormap_type[value]


def type_projection(value):
    """
    Checks projection type
    """
    if value.lower() not in ProjectionType():
        raise argparse.ArgumentTypeError('%s is not a type of projection known'
                                         % value)
    return value


def type_continent(value):
    """
    Checks continent type
    """
    if value.lower() not in ContinentType():
        raise argparse.ArgumentTypeError('%s is not a type of continent known'
                                         % value)
    return value


def colormap_options(parser):
    """
    Definition of options that alters the appearance of the colormap
    """
    colormap = parser.add_argument_group('colormap', 'Setting up the colormap')
    colormap.add_argument('--invert',
                          help='Invert colors',
                          action='store_true')
    colormap.add_argument('--colormap',
                          help='Name of the colormap. Can be: %s'
                          % ColormapType(),
                          type=type_colormap,
                          metavar='NAME',
                          default=ColormapType().default)
    colormap.add_argument('--draw',
                          help='Plot colormaps in PATH and terminate '
                          'the program',
                          metavar='PATH',
                          action=DrawColormaps)
    colormap.add_argument('--hide',
                          help='Do not draw the colormap',
                          action='store_true')
    colormap.add_argument('--range',
                          nargs=3,
                          help='Definition of the colormap range : upper '
                          'value, lower value, number of colors',
                          metavar=('MIN', 'MAX', 'N'),
                          type=float)


def mapping_options(parser):
    """
    Definition of options that alters the appearance of the image
    """
    mapping = parser.add_argument_group('figure', 'Changing the mapping')
    mapping.add_argument('--dpi',
                         help='Image resolution in dots per inch.',
                         type=int,
                         default=200)
    mapping.add_argument('--map_over',
                         help='Show map image above continents',
                         action='store_true')
    mapping.add_argument('--meridian_width',
                         help='Set up line width for meridians. Set the size '
                         'to zero for this, if you want to hide the meridians',
                         type=float,
                         metavar='N')
    mapping.add_argument('--continent',
                         help='Draw continents. Can be %s.' % ContinentType(),
                         type=type_continent,
                         default=ContinentType().default,
                         metavar='TYPE')
    mapping.add_argument('--projection',
                         help='The desired cylindrical projection. Can be %s'
                         % ProjectionType(),
                         metavar='NAME',
                         type=type_projection,
                         default=ProjectionType().default)
    mapping.add_argument('--resolution',
                         help='Resolution of boundary database to use. Can '
                         'be %s' % ResolutionType(),
                         metavar='NAME',
                         type=type_resolution,
                         default=ResolutionType().default)
    mapping.add_argument('--zone',
                         nargs=4,
                         help='Definition of the area to display',
                         metavar=('XMIN', 'YMIN', 'XMAX', 'YMAX'),
                         type=float)


def caption_options(parser):
    """
    Definition of options that alters captions on the figure
    """
    caption = parser.add_argument_group('legend', 'Changing labels')
    caption.add_argument('--legend',
                         help='Legend for the colormap')
    caption.add_argument('--title',
                         nargs='+',
                         help='Figure title. The string "%%(title)" will be '
                         'replaced by the title of the input file.',
                         metavar='TEXT')


def netcdf_options(parser):
    """
    Definition of NetCDF axis to read
    """
    netcdf = parser.add_argument_group('NetCDF', 'Defining axis to read')
    netcdf.add_argument('--x_axis',
                        help='NetCDF variable name defines the axis of '
                        'longitudes',
                        metavar='NAME')
    netcdf.add_argument('--y_axis',
                        help='NetCDF variable name defines the axis of '
                        'latitudes',
                        metavar='NAME')


def scatter_options(parser):
    """
    Definition of options that alters scatter plot
    """
    scatter = parser.add_argument_group('scatter plot',
                                        'Setting up scatter plot')
    scatter.add_argument('--size', help='Dot size to display.', type=float)


def usage():
    """
    Program syntax
    """
    parser = argparse.ArgumentParser(description='Georeferenced mapping data')

    # positionnal arguments
    parser.add_argument('inputfile',
                        help='The path to the file to map. Must be a NetCDF '
                        'file or an ASCII file')
    parser.add_argument('outputfile',
                        help='The path to the image to create')
    parser.add_argument('variable',
                        help='NetCDF variable name to map',
                        metavar='variable_name')

    # optional arguments
    parser.add_argument('--debug',
                        help='Print debugging message',
                        action='store_true')

    # mapping options
    mapping_options(parser)

    # colormap options
    colormap_options(parser)

    # title & legend options
    caption_options(parser)

    # netcdf options
    netcdf_options(parser)

    # options for scatter plot
    scatter_options(parser)
    return parser.parse_args()


def setup_logger(enabled):
    """
    Enable/disable debugging message
    """
    logging.basicConfig(format='%(filename)s: %(lineno)d: %(message)s')
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG if enabled else logging.INFO)


def main():
    """
    Main function
    """
    # Parse arguments
    args = usage()

    # Enable/disable debugging trace
    setup_logger(args.debug)

    # Reads NetCDF file
    var = Variable.from_netcdf(args.inputfile,
                               args.variable,
                               x_axis=args.x_axis,
                               y_axis=args.y_axis)

    # Set the caption and the title with the data supplied by the user
    if args.legend:
        var.long_name = args.legend
    if args.title:
        for (index, value) in enumerate(args.title):
            if value == '%(title)':
                args.title[index] = var.title
        var.title = "\n".join(args.title)

    # Creating mapping
    mapping = Mapping(args.colormap, args.projection, args.resolution)

    # Set mapping properties with data supplied by the user
    mapping.show_colormap = not args.hide
    if mapping.show_colormap:
        mapping.legend = var.long_name
    if args.invert:
        mapping.invert_colormap()
    if args.map_over:
        mapping.show_map_above_continents()
    if args.range:
        mapping.set_range(args.range[0], args.range[1], args.range[2])
    if args.zone:
        var.select_zone(args.zone[0],
                        args.zone[1],
                        args.zone[2],
                        args.zone[3])
        mapping.set_zone(args.zone[0],
                         args.zone[1],
                         args.zone[2],
                         args.zone[3])
    if args.size:
        mapping.size = args.size
    if args.meridian_width is not None:
        mapping.meridian_linewidth = args.meridian_width
    if args.continent:
        mapping.continent = args.continent

    var.dump()

    # Stores data that must be drawed
    mapping.x = var.longitudes
    mapping.y = var.latitudes
    mapping.z = var.values

    # Creates output file
    mapping.display(args.outputfile, var.title, args.dpi)


if __name__ == '__main__':
    main()
