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
import argparse
import fnmatch
import os
import lagrangian
import netCDF4
import numpy


def directory(path):
    """
    Check if path is a directory
    """
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError('%r must be a directory' % path)
    return path


def usage():
    """
    Parse commands line
    """
    parser = argparse.ArgumentParser(description='Convert the U and V '
                                     'components speeds from metric to '
                                     'angular system units')

    parser.add_argument('src',
                        help='Directory containing the NetCDF grids to '
                        'convert',
                        type=directory)
    parser.add_argument('dst',
                        help='Directory containing the NetCDF grids '
                        'converted',
                        type=directory)
    parser.add_argument('u',
                        help='NetCDF variable containing the U component')
    parser.add_argument('v',
                        help='NetCDF variable containing the V component')
    parser.add_argument('--mask',
                        help='Select only the files matching the mask',
                        default="*.nc")
    return parser.parse_args()


def get_files(dirname, pattern):
    """
    Get the list of files to convert
    """
    result = []
    for root, _dirs, files in os.walk(dirname):
        result += [os.path.join(root, item)
                   for item in files
                   if fnmatch.fnmatch(item, pattern)]
    return result


def get_axis(variables):
    """
    Returns the axes of the map
    """
    x = None
    y = None
    for name, variable in variables.items():
        if variable.ndim == 1 and variable.dimensions[0] == name:
            if lagrangian.LongitudeUnit()(variable.units.encode('ASCII')):
                x = name
            elif lagrangian.LatitudeUnit()(variable.units.encode('ASCII')):
                y = name
    if x is None:
        raise RuntimeError("Could not find the X-axis")
    if y is None:
        raise RuntimeError("Could not find the Y-axis")
    return x, y


def copy(src, dst, u, v):
    """
    Copy a netCDF
    """
    dst.setncatts(src.__dict__)

    for dim_name, dim in src.dimensions.items():
        if dim.isunlimited():
            raise NotImplementedError()
        else:
            dst.createDimension(dim_name, len(dim))

    for var_name in src.variables:
        old_var = src.variables[var_name]
        if hasattr(old_var, '_FillValue'):
            fill_value = old_var._FillValue
        else:
            fill_value = None

        new_var = dst.createVariable(var_name,
                                     old_var.dtype,
                                     old_var.dimensions,
                                     fill_value=fill_value)
        atts = old_var.__dict__

        if '_FillValue' in atts:
            del atts['_FillValue']

        new_var.setncatts(atts)

        if var_name not in [u, v]:
            new_var[:] = old_var[:]


def read_var(variable):
    """
    Read velocity component and convert it in m/s if needed
    """
    offset, scale = lagrangian.Units.GetConverter(
        variable.units.encode('ASCII'), "m/s")
    return variable[:] * scale + offset


def convert(x, y, u, v):
    """
    Convert U V components from metric to angular system units
    """
    assert u.shape == v.shape

    nx = len(x)
    ny = len(y)

    if nx == u.shape[0]:
        assert u.shape == (nx, ny)
        transpose = False
    else:
        assert u.shape == (ny, nx)
        transpose = True

    # Keep longitude between -180, 180
    x[x > 180] -= 360

    x0 = numpy.empty([nx, ny])
    y0 = numpy.empty([nx, ny])

    # Build a matrix of latitude and longitude
    for ix in range(nx):
        y0[ix, :] = y
    for ix in range(ny):
        x0[:, ix] = x

    if transpose:
        x0 = x0.transpose()
        y0 = y0.transpose()

    # Conversion from spherical to cartesian coordinates and move it
    # for one second using U and V component
    lon = numpy.radians(x0)
    lat = numpy.radians(y0)

    sin_x = numpy.sin(lon)
    cos_x = numpy.cos(lon)
    sin_y = numpy.sin(lat)
    cos_y = numpy.cos(lat)

    xc = -u * sin_x - v * cos_x * sin_y
    yc = u * cos_x - v * sin_y * sin_x
    zc = v * cos_y

    xc += 6371000 * cos_y * cos_x
    yc += 6371000 * cos_y * sin_x
    zc += 6371000 * sin_y

    # Conversion from cartesian to spherical coordinates
    x1 = numpy.degrees(numpy.arctan2(yc, xc))
    y1 = numpy.degrees(numpy.arcsin(
        zc / numpy.sqrt(xc * xc + yc * yc + zc * zc)))

    dx = x1 - x0
    dy = y1 - y0

    # Return the velocity in degrees/s
    return (dx + 180) % 360 - 180, dy


def main():
    """
    Main function
    """
    args = usage()

    for item in get_files(args.src, args.mask):
        path = os.path.join(args.dst, os.path.basename(item))
        print(path)

        try:
            src = netCDF4.Dataset(item)
            dst = netCDF4.Dataset(path, 'w', format='NETCDF4_CLASSIC')

            x_axis_name, y_axis_name = get_axis(src.variables)

            copy(src, dst, args.u, args.v)

            x = src.variables[x_axis_name][:]
            y = src.variables[y_axis_name][:]

            u = read_var(src.variables[args.u])
            v = read_var(src.variables[args.v])

            u, v = convert(x, y, u, v)

            dst.variables[args.u][:] = u * 1e6
            dst.variables[args.u].units = "microdegrees/s"
            dst.variables[args.v][:] = v * 1e6
            dst.variables[args.v].units = "microdegrees/s"

            src.close()
            dst.close()
        except:
            if os.path.exists(path):
                os.unlink(path)
            raise


if __name__ == '__main__':
    main()
